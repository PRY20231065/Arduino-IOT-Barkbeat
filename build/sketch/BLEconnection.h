#line 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\main\\BLEconnection.h"
#include <BLEDevice.h>  //libraries that are used in communication
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>

BLEServer *pServer = NULL;  //creating BLEserver globally
BLECharacteristic *pTxCharacteristic;
bool deviceConnected = false;
bool oldDeviceConnected = false;
String txValue;  //variable used in data transmission
String rxVal;    //variable used in data receiving

#define SERVICE_UUID "0304e97e-41bb-40d7-8cd1-3c745a98ddd3"  // UART service UUID
#define CHARACTERISTIC_UUID_RX "a52b093a-3e56-401d-b2b2-7460b8258827"
#define CHARACTERISTIC_UUID_TX "52cdeb43-e03d-4330-9de2-a3b5246f1745"

//Clases

class MyServerCallbacks : public BLEServerCallbacks {
  void onConnect(BLEServer *pServer) {
    deviceConnected = true;
  };
  void onDisconnect(BLEServer *pServer) {
    deviceConnected = false;
  }
};

class MyCallbacks : public BLECharacteristicCallbacks {
  void onWrite(BLECharacteristic *pCharacteristic) {
    std::string rxValue = pCharacteristic->getValue();
    Serial.println("* ********");

    if (rxValue.length() > 0) {

      Serial.println("* ********");
      Serial.print("Received Value: ");
      for (int i = 0; i < rxValue.length(); i++) {
        Serial.print(rxValue[i]);
        rxVal += String(rxValue[i]);
      }
      Serial.println();
      Serial.println("* ********");
      Serial.print(rxVal);
    }
  }

  
};

