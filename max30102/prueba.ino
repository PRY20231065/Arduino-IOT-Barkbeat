#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

void setup() {
  // initialize digital pin 2 as an output.

  // initialize the serial communication:
  Serial.begin(115200);
  Serial.println(); // blank line in serial ...
  pinMode(41, INPUT); // Setup for leads off detection LO +
  pinMode(40, INPUT); // Setup for leads off detection LO -
  // initialize the serial BT communication:
  
}

void loopAD8232() {
  if((digitalRead(40) == 1)||(digitalRead(41) == 1)){
    Serial.println('!');  
  }
  else{
    Serial.print(">Value:");
    Serial.println(analogRead(A0));
  }
  delay(1);
}
