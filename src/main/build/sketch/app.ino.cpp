#line 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
//*******************Libraries*************************
#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>
#include <NTPClient.h>
#include <WiFiUdp.h>


//******************Constants**************************
#define WIFISSID "RAMON - 1 "
#define PASSWORD "ramon560"
#define MQTT_HOST "54.165.125.147"
#define MQTT_USER "pry20231065"
#define MQTT_PASS "WgR4YMg724634tHQnXpz"

WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//*****************Methods*****************************
#line 22 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
void callback(char *topic, byte *payload, unsigned int length);
#line 33 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
void startWifi();
#line 49 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
void startMQTT();
#line 59 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
void setup();
#line 68 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
void loop();
#line 22 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
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

void startMQTT(){
    client.setServer(MQTT_HOST, 1883);
    client.setCallback(callback);
    client.connect("barkbeat", MQTT_USER, MQTT_PASS);


}



void setup()
{
    Serial.begin(115200);

    

    client.setCallback(callback);
}

void loop()
{
}
#line 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\prueba.ino"
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

