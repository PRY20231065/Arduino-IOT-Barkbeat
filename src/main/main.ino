/*******************Libraries*************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "lib/ecg_sensor/AD8232.h"
#include "lib/bluetooth/BLEconnection.h"
#include "lib/pulse_sensor/MAX30102.h"

/*******************Constants**************************/
// #define WIFISSID ""
// #define PASSWORD ""
#define MQTT_HOST "3.208.219.123"
#define MQTT_USER "pry20231065"
#define MQTT_PASS "WgR4YMg724634tHQnXpz"
#define TOPIC_1_ECG "AD8232"
#define TOPIC_2_PULSE "MAX30102"
#define TOPIC_3_TEMP "TEMP"

/********************Classes***************************/
WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
BLEServer *pServer;

//****************Variables****************************
char payload1[500];
char topic1[20];
char payload2[500];
char topic2[20];
char payload3[500];
char topic3[20];
char dog_uuid[37];
char wifi_ssid[100];
char wifi_passw[50];
bool new_wifi_credentials = false;
bool status_scan_ecg = false;
bool status_scan_pulse = true;
bool initialized_ecg_sensor = false;
bool initialized_pulse_sensor = false;

DynamicJsonDocument doc(512);
//*****************Methods*****************************
void callback(char *topic, byte *payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    Serial.write(payload, length);
    Serial.println(topic);
}

bool areWIFICredentialsEmpty()
{
    return (strlen(wifi_ssid) == 0 && strlen(wifi_passw) == 0);
}

//****************start functions*********************
void startWifi()
{
    WiFi.disconnect();

    WiFi.begin(wifi_ssid, wifi_passw);
    Serial.println();
    Serial.print("Waiting for WiFi...");

    int connectionAttempts = 0;

    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
        if (connectionAttempts > 10) // 10 intentos
        {
            pTxCharacteristic->setValue("{\"success\":false,\"msg\":\"Error de conexión. Compruebe las credenciales de red proporcionadas e inténtelo de nuevo.\"}");
            pTxCharacteristic->notify();
            pTxCharacteristic->setValue("{\"success\":false,\"msg\":\"Error en la conexión MQTT al servidor cloud.\"}");
            pTxCharacteristic->notify();
            sprintf(wifi_ssid, "%s", "");
            sprintf(wifi_passw, "%s", "");
            return;
        }
        connectionAttempts += 1;
    }

    Serial.println("");
    Serial.println("WiFi Connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    pTxCharacteristic->setValue("{\"success\":true,\"msg\":\"El dispositivo IoT ahora está conectado a Internet.\"}");
    pTxCharacteristic->notify();
}

void startMQTT()
{
    client.setServer(MQTT_HOST, 1883);
    client.setCallback(callback);
    client.setKeepAlive(240); // 4 minutos de conexión, terminado se desconecta
}

void startBLE()
{
    BLEDevice::init("IoT Barkbeat Device");
    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    // Create a BLE Characteristic

    pTxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TX,
        BLECharacteristic::PROPERTY_NOTIFY);
    pTxCharacteristic->addDescriptor(new BLE2902());

    pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);
    pRxCharacteristic->addDescriptor(new BLE2902());
    pRxCharacteristic->setCallbacks(new MyCallbacks());

    // Start the service
    pService->start();
    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify…");
}

void reconnectBLEserver() // added
{
    // disconnected so advertise
    if (!deviceConnected && oldDeviceConnected)
    {
        delay(500);                  // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("Disconnected: start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connected so reset boolean control
    if (deviceConnected && !oldDeviceConnected)
    {
        // do stuff here on connecting
        Serial.println("Reconnected");
        oldDeviceConnected = deviceConnected;
    }
}

void reconnectMQTT()
{
    if (WiFi.status() == WL_CONNECTED)
    {
        while (!client.connected())
        {
            Serial.println("Attempting MQTT connection...");

            // Attemp to connect
            if (client.connect("barkbeat", MQTT_USER, MQTT_PASS))
            {
                Serial.println("Connected");
                pTxCharacteristic->setValue("{\"success\":true,\"msg\":\"Se ha establecido la conexión con el servidor.\"}");
                pTxCharacteristic->notify();
            }
            else
            {
                pTxCharacteristic->setValue("{\"success\":false,\"msg\":\"Error en la conexión MQTT al servidor cloud.\"}");
                pTxCharacteristic->notify();
                Serial.print("Failed, rc=");
                Serial.print(client.state());
                Serial.println(" try again in 2 seconds");
                // Wait 2 seconds before retrying
                delay(2000);
            }
        }
    }
}

void reconnectWIFI()
{
    if (WiFi.status() != WL_CONNECTED && !areWIFICredentialsEmpty() && !(WiFi.getMode() == WIFI_OFF))
    {
        startWifi();
    }
}

void setup()
{
    Serial.begin(115200);
    startBLE();
    startMQTT();
}

/*void loop()
{
    if (!client.connected())
    {
        reconnectMQTT();
    }

    //reconnectBLEserver();

    //if (deviceConnected) // BLE is connected
    //{
        //sprintf(topic, "%s", TOPIC_1_ECG);
        //sprintf(payload, "%s", "");

        //loopSensorAD8232(payload, topic, dog_uuid, client);
    //}

    // loopAD8232();
}*/

void loop()
{

    if (WiFi.status() == WL_CONNECTED && !initialized_ecg_sensor)
    {
        // Wifi debe estar conectado para inicializar las epochs
        initSensorAD8232(timeClient);
        initialized_ecg_sensor = true;
    }

    if (WiFi.status() == WL_CONNECTED && !initialized_pulse_sensor)
    {
        if (initSensorMAX30102(timeClient))
        {
            initialized_pulse_sensor = true;
        }
        else
        {
            pTxCharacteristic->setValue("{\"success\":false,\"msg\":\"MAX30102 was not found. Please check wiring/power. .\"}");
            pTxCharacteristic->notify();
        }
    }

    if (deviceConnected)
    {
        // Si hemos recibido un valor
        if (!rxVal.empty())
        {
            // deserializamos en json
            deserializeJson(doc, rxVal);
            const char *subject_value = doc["subject"];
            // si el asunto son credenciales wifi, strcmp devuelve 0 si las cadenas son iguales
            if (strcmp(subject_value, "wifi-credentials") == 0)
            {
                const char *ssid_value = doc["ssid"];
                const char *password_value = doc["password"];

                sprintf(wifi_ssid, "%s", ssid_value);
                sprintf(wifi_passw, "%s", password_value);
                rxVal.clear();
                startWifi();
                return;
            }
            // si el asunto el uuid de perro
            else if (strcmp(subject_value, "dog-id") == 0)
            {
                const char *uuid_value = doc["uuid"];
                // Serial.println(uuid_value);
                sprintf(dog_uuid, "%s", uuid_value);

                if (strlen(dog_uuid) == 0)
                {
                    pTxCharacteristic->setValue("{\"success\":false,\"msg\":\"No se pudo asociar el perro seleccionado al dispositivo IoT.\"}");
                    pTxCharacteristic->notify();
                }
                else
                {
                    pTxCharacteristic->setValue("{\"success\":true,\"msg\":\"Se asoció el último perro seleccionado al dispositivo IoT.\"}");
                    pTxCharacteristic->notify();
                }
            }

            else if (strcmp(subject_value, "scan-ad8232") == 0)
            {
                status_scan_ecg = doc["status"].as<bool>();
            }

            else if (strcmp(subject_value, "scan-max30102") == 0)
            {
                status_scan_pulse = doc["status"].as<bool>();
            }

            // CLEAN
            rxVal.clear();
        }
    }

    if (!(strlen(dog_uuid) == 0))
    {
        if (status_scan_ecg)
        {
            sprintf(topic1, "%s", TOPIC_1_ECG);
            sprintf(payload1, "%s", "");

            loopSensorAD8232(payload1, topic1, dog_uuid, client);
        }

        if (status_scan_pulse)
        {
            sprintf(topic3, "%s", TOPIC_2_PULSE);
            sprintf(payload3, "%s", "");

            loopSensorMAX30102(payload2, topic2, dog_uuid, client);
        }

        sprintf(topic3, "%s", TOPIC_3_TEMP);
        sprintf(payload3, "%s", "");

        loopScanTemperatureMAX30102(payload3,topic3, dog_uuid, client);
    }

    // aqui ocurre el loop de BLE
    reconnectBLEserver();
    reconnectWIFI();
    reconnectMQTT();
}