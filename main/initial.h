//*******************Libraries*************************
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

//******************Constants**************************
#define WIFISSID "RAMON - 1 "
#define PASSWORD "ramon560"
#define MQTT_HOST "54.165.125.147"
#define MQTT_USER "pry20231065"
#define MQTT_PASS "WgR4YMg724634tHQnXpz"
#define TOPIC_1_LABEL "AD8232"
#define TOPIC_2_LABEL "MAX30102"

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//****************Variables****************************
char payload[10000];
char topic[150];

/**
 * Variables for AD832 ECG
 */
char str_sensor[10];
char str_millis[20];
double epochseconds = 0;
double epochmilliseconds = 0;
double current_millis = 0;
double current_millis_at_sensordata = 0;
double timestampp = 0;

/**
 * Variables for  MAX30102
 *
 */

MAX30105 particleSensor;

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

    timeClient.update();
    epochseconds = timeClient.getEpochTime();
    epochmilliseconds = epochseconds * 1000;
    // Serial.print("epochmilliseconds=");
    // Serial.println(epochmilliseconds);
    current_millis = millis();
    // Serial.print("current_millis=");
    // Serial.println(current_millis);
    initMax30102();
    initAD8232();
}

void loop()
{
    if (!client.connected())
    {
        reconnectMQTT();
    }

    sprintf(topic, "%s", TOPIC_1_LABEL);
    sprintf(payload, "%s", "");

    loopMax30102();
    //loopAD8232();
}

void initMax30102()
{
    pinMode(41, INPUT); // Setup for leads off detection LO +
    pinMode(40, INPUT); // Setup for leads off detection LO -
}

void initAD8232()
{
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // Use default I2C port, 400kHz speed
    {
        Serial.println("MAX30102 was not found. Please check wiring/power. ");
        while (1)
            ;
    }
    Serial.println("Place your index finger on the sensor with steady pressure.");

    particleSensor.setup();                    // Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED
}

void loopMax30102()
{
    long irValue = particleSensor.getIR();
    if (checkForBeat(irValue) == true)
    {
        long delta = millis() - lastBeat;
        lastBeat = millis();
        beatsPerMinute = 60 / (delta / 1000.0);
        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
            rates[rateSpot++] = (byte)beatsPerMinute; 
            rateSpot %= RATE_SIZE;                    
            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
                beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
        }
    }
}