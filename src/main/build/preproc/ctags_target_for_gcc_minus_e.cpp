# 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
//*******************Libraries*************************
# 3 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino" 2
# 4 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino" 2
# 5 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino" 2
# 6 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino" 2
# 7 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino" 2


//******************Constants**************************






WiFiClient espClient;
PubSubClient client(espClient);
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");

//*****************Methods*****************************
void callback(char *topic, byte *payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = 
# 26 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino" 3 4
               __null
# 26 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\app.ino"
                   ;
    Serial.write(payload, length);
    Serial.println(topic);
}


//****************start functions*********************
void startWifi()
{
    WiFi.begin("RAMON - 1 ", "ramon560");
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
    client.setServer("54.165.125.147", 1883);
    client.setCallback(callback);
    client.connect("barkbeat", "pry20231065", "WgR4YMg724634tHQnXpz");


}



void setup()
{
    Serial.begin(115200);



    client.setCallback(callback);
}

void loop()
{
}
# 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\prueba.ino"
# 2 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\prueba.ino" 2
# 3 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\prueba.ino" 2
# 4 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\app\\prueba.ino" 2

void setup() {
  // initialize digital pin 2 as an output.

  // initialize the serial communication:
  Serial.begin(115200);
  Serial.println(); // blank line in serial ...
  pinMode(41, 0x01); // Setup for leads off detection LO +
  pinMode(40, 0x01); // Setup for leads off detection LO -
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
