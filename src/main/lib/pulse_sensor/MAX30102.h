#ifndef MAX30102_H
#define MAX30102_H

/***************Libraries*********************/
#include <Wire.h>
#include "MAX30105.h"
#include "heartRate.h"

/***************Global variables**************/
MAX30105 particleSensor;
const byte RATE_SIZE = 4; // Increase this for more averaging. 4 is good.
byte rates[RATE_SIZE];    // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred
float beatsPerMinute = 0;
int beatAvg = 0;
int previousAvg = 0;

bool initSensorMAX30102()
{
    if (!particleSensor.begin(Wire, I2C_SPEED_FAST)) // Use default I2C port, 400kHz speed
    {
        return false;
        // Serial.println("MAX30102 was not found. Please check wiring/power. ");
    }
    Serial.println("Place your index finger on the sensor with steady pressure.");

    particleSensor.setup();                    // Configure sensor with default settings
    particleSensor.setPulseAmplitudeRed(0x0A); // Turn Red LED to low to indicate sensor is running
    particleSensor.setPulseAmplitudeGreen(0);  // Turn off Green LED
    return true;
}

void loopSensorMAX30102(char (&payload)[500], char (&topic)[20], char (&dog_uuid)[37], PubSubClient &client)
{
    sprintf(payload, "{ \"dog_id\": \"%s\", \"beats_per_minute\": ", dog_uuid);

    long irValue = particleSensor.getIR();

    if (checkForBeat(irValue) == true)
    {
        // We sensed a beat!
        long delta = millis() - lastBeat;
        lastBeat = millis();

        beatsPerMinute = 60 / (delta / 1000.0);

        if (beatsPerMinute < 255 && beatsPerMinute > 20)
        {
            rates[rateSpot++] = (byte)beatsPerMinute; // Store this reading in the array
            rateSpot %= RATE_SIZE;                    // Wrap variable

            // Take average of readings
            beatAvg = 0;
            for (byte x = 0; x < RATE_SIZE; x++)
                beatAvg += rates[x];
            beatAvg /= RATE_SIZE;
        }
    }

    if(irValue > 50000){
        //Serial.println("hay dedo");
        //Serial.println(beatsPerMinute);
        //Serial.println(beatAvg);
        //Serial.println("formateamos");
        sprintf(payload, "%s %.2f, ", payload, beatsPerMinute);
        //Serial.println(payload);
        sprintf(payload, "%s \"avg\": %d }", payload, beatAvg);
        //Serial.println(payload);
        
        //Serial.println("publicar");
        if(previousAvg != beatAvg){
            client.publish(topic, payload);
            Serial.println(payload);
        }
        
        //Serial.println("publicado");
        previousAvg = beatAvg;
    }

    

    //Serial.print("IR=");
    //Serial.print(irValue);
    //Serial.print(", BPM=");
    //Serial.print(beatsPerMinute);
    //Serial.print(", Avg BPM=");
    //Serial.print(beatAvg);

    // if (irValue < 50000)
    //   Serial.print(" No finger?");

    // Serial.println();
}

#endif