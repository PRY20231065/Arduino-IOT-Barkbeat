# 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino"
//*******************Libraries*************************
# 3 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino" 2
# 4 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino" 2
# 5 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino" 2
# 6 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino" 2
# 7 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino" 2
# 8 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino" 2
# 9 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino" 2

//******************Constants**************************
# 21 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino"
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
byte rates[RATE_SIZE]; // Array of heart rates
byte rateSpot = 0;
long lastBeat = 0; // Time at which the last beat occurred

float beatsPerMinute;
int beatAvg;

//*****************Methods*****************************
void callback(char *topic, byte *payload, unsigned int length)
{
    char p[length + 1];
    memcpy(p, payload, length);
    p[length] = 
# 52 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino" 3 4
               __null
# 52 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\src\\main\\main.ino"
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

void startMQTT()
{
    client.setServer("3.208.219.123", 1883);
    client.setCallback(callback);
    client.connect("barkbeat", "pry20231065", "WgR4YMg724634tHQnXpz");
    Serial.print("Connect MQTT: ");
    Serial.println(client.connected());
}

void reconnectMQTT()
{
    while (!client.connected())
    {
        Serial.println("Attempting MQTT connection...");

        // Attemp to connect
        if (client.connect("barkbeat", "pry20231065", "WgR4YMg724634tHQnXpz"))
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

    sprintf(topic, "%s", "AD8232");
    sprintf(payload, "%s", "");

    loopSensorAd8232(payload,topic, dog_uuid, client);
    //loopAD8232();
}
