/*******************Libraries*************************/
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "lib/ecg_sensor/AD8232.h"
#include "lib/bluetooth/BLEconnection.h"

/*******************Constants**************************/
#define WIFISSID "RAMON - 1 "
#define PASSWORD "ramon560"
#define MQTT_HOST "3.208.219.123"
#define MQTT_USER "pry20231065"
#define MQTT_PASS "WgR4YMg724634tHQnXpz"
#define TOPIC_1_ECG "AD8232"
#define TOPIC_2_PULSE "MAX30102"

/********************Classes***************************/
WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
BLEServer *pServer;

//****************Variables****************************
char payload[500];
char topic[20];
char dog_uuid[37];

//*****************Methods*****************************
void callback(char *topic, byte *payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = NULL;
    Serial.write(payload, length);
    Serial.println(topic);
}

//****************start functions*********************
void startWifi()
{
    WiFi.begin(WIFISSID, PASSWORD);
    Serial.println();
    Serial.print("Waiting for WiFi...");
    while (WiFi.status() != WL_CONNECTED)
    {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.println("WiFi Connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
}

void startMQTT()
{
    client.setServer(MQTT_HOST, 1883);
    client.setCallback(callback);
    client.connect("barkbeat", MQTT_USER, MQTT_PASS);
    Serial.print("Connect MQTT: ");
    Serial.println(client.connected());
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

    BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_RX,
        BLECharacteristic::PROPERTY_WRITE);

    pRxCharacteristic->setCallbacks(new MyCallbacks());
    // Start the service
    pService->start();
    // Start advertising
    pServer->getAdvertising()->start();
    Serial.println("Waiting a client connection to notify…");
}

void loopBLE()
{
    if (deviceConnected)
    {
        // Serial.print(deviceConnected);

        if (rxVal == "rxdata")
        {
            // Do anything here
        }
    }
}

void reconnectBLEserver() //added
{
  // disconnected so advertise
  if (!deviceConnected && oldDeviceConnected) {
    delay(500); // give the bluetooth stack the chance to get things ready
    pServer->startAdvertising(); // restart advertising
    Serial.println("Disconnected: start advertising");
    oldDeviceConnected = deviceConnected;
  }
  // connected so reset boolean control
  if (deviceConnected && !oldDeviceConnected) {
    // do stuff here on connecting
    Serial.println("Reconnected");
    oldDeviceConnected = deviceConnected;
  }
}

void reconnectMQTT()
{
    while (!client.connected())
    {
        Serial.println("Attempting MQTT connection...");

        // Attemp to connect
        if (client.connect("barkbeat", MQTT_USER, MQTT_PASS))
        {
            Serial.println("Connected");
        }
        else
        {
            Serial.print("Failed, rc=");
            Serial.print(client.state());
            Serial.println(" try again in 2 seconds");
            // Wait 2 seconds before retrying
            delay(2000);
        }
    }
}

void setup()
{
    Serial.begin(115200);
    startWifi();
    startMQTT();
    startBLE();

    if (!client.connected())
    {
        reconnectMQTT();
    }

    initSensorAD8232(timeClient);
    sprintf(dog_uuid, "%s", "0b1f6d8e-886f-49c1-8b4c-19605338ec6e");
}

void loop()
{
    if (!client.connected())
    {
        reconnectMQTT();
    }

    reconnectBLEserver();

    if (deviceConnected) // BLE is connected
    {
        sprintf(topic, "%s", TOPIC_1_ECG);
        sprintf(payload, "%s", "");

        loopSensorAD8232(payload, topic, dog_uuid, client);
    }

    // loopAD8232();
}
