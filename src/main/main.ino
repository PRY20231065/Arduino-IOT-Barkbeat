//*******************Libraries*************************
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "lib/ecg_sensor/AD8232.h"

//******************Constants**************************
#define WIFISSID "RAMON - 1 "
#define PASSWORD "ramon560"
#define MQTT_HOST "3.208.219.123"
#define MQTT_USER "pry20231065"
#define MQTT_PASS "WgR4YMg724634tHQnXpz"
#define TOPIC_1_ECG "AD8232"
#define VARIABLE_1_LABEL "ECG_Sensor_data"

#define TOPIC_2_PULSE "MAX30102"

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//****************Variables****************************
char payload[1000];
char topic[150];
char dog_uuid[37]; 


/**
 * Variables for  MAX30102
 *
 */



const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];    // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

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

    if (!client.connected())
    {
        reconnectMQTT();
    }

    initSensorAd8232(timeClient);
    sprintf(dog_uuid, "%s","0b1f6d8e-886f-49c1-8b4c-19605338ec6e");

}

void loop()
{
    if (!client.connected())
    {
        reconnectMQTT();
    }

    sprintf(topic, "%s", TOPIC_1_ECG);
    sprintf(payload, "%s", "");

    loopSensorAd8232(payload,topic, dog_uuid, client);
    //loopAD8232();
}



