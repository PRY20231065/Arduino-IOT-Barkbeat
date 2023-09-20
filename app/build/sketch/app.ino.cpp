#line 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\app\\app.ino"
#include <Arduino.h>

#line 3 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\app\\app.ino"
void setup();
#line 15 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\app\\app.ino"
void loop();
#line 3 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\app\\app.ino"
void setup() {
  // initialize digital pin 2 as an output.

  // initialize the serial communication:
  Serial.begin(115200);
  Serial.println(); // blank line in serial ...
  pinMode(41, INPUT); // Setup for leads off detection LO +
  pinMode(40, INPUT); // Setup for leads off detection LO -
  // initialize the serial BT communication:
  
}

void loop() {
  if((digitalRead(40) == 1)||(digitalRead(41) == 1)){
    Serial.println('!');  

  }
  else{
    // send the value of analog input 0 to serial:
    Serial.print(">Value:");
    Serial.println(analogRead(A0));
    //Do the same for blutooth
    
  }
  //Wait a little to keep serial data from saturating
  delay(1);
}

