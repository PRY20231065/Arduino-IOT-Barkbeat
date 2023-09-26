#line 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\lib\\ecg_sensor\\AD8232.h"
//*******************Libraries*************************
#ifndef AD8232_H
#define AD8232_H

#include <NTPClient.h>
#include <Arduino.h>

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

char created_time[20];
/**
 * Pines
 */
#define LOplus 41
#define LOminus 40
#define SENSORPIN A0

void initSensorAd8232(NTPClient &ntp)
{
    

    ntp.update();
    epochseconds = ntp.getEpochTime();
    epochmilliseconds = epochseconds * 1000;
    // Serial.print("epochmilliseconds=");
    // Serial.println(epochmilliseconds);
    current_millis = millis();
    // Serial.print("current_millis=");
    // Serial.println(current_millis);
}

void loopSensorAd8232(char (&payload)[1000], char (&topic)[150], char (&dog_uuid)[37], PubSubClient &client)
{
    //sprintf(payload, "{\"%s\": [", VARIABLE_LABEL);
    sprintf(payload, "{ \"dog_id\": \"%s\", \"ecg\": [", dog_uuid);

    float sensor = 0;

    for (int i = 1; i <= 2; i++)
    {

        //Serial.print(">value:");
        sensor = analogRead(SENSORPIN);
        //Serial.println(sensor);

        dtostrf(sensor, 4, 2, str_sensor);
        sprintf(payload, "%s{\"value\":", payload);      // Adds the value
        sprintf(payload, "%s %s,", payload, str_sensor); // Adds the value
        current_millis_at_sensordata = millis();
        timestampp = epochmilliseconds + (current_millis_at_sensordata - current_millis);
        dtostrf(timestampp, 10, 0, str_millis);
        sprintf(payload, "%s \"timestamp\": %s},", payload, str_millis); // Adds the value

        /*if(i = 1){
            dtostrf(timestampp, 10, 0, created_time);
        }*/

        delay(15);
    }

    //Serial.print(">value:");
    sensor = analogRead(SENSORPIN);
    //Serial.println(sensor);

    dtostrf(sensor, 4, 2, str_sensor);
    current_millis_at_sensordata = millis();
    timestampp = epochmilliseconds + (current_millis_at_sensordata - current_millis);
    dtostrf(timestampp, 10, 0, str_millis);
    sprintf(payload, "%s{\"value\":%s, \"timestamp\": %s}], \"created_time\": %s}", payload, str_sensor, str_millis, str_millis);
    //sprintf(payload, "%s \"created_time\": %s ",payload, created_time);
    //Serial.println("Publishing data to MQTT");

   

    client.publish(topic, payload);
    
    Serial.println(payload);
}

#endif