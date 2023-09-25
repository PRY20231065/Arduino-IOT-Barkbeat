# 1 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\main\\main.ino"
# 2 "C:\\Users\\eduiz\\OneDrive\\Documentos\\Arduino\\Proyecto-Tesis\\Arduino-IOT-Barkbeat\\main\\main.ino" 2


//Base methods

void setup()
{
  Serial.begin(115200);
  initBLE();
}

void loop()
{
  loopBLE();

}

void initBLE()
{
  BLEDevice::init("IoT Barkbeat Device");
  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());
  // Create the BLE Service
  BLEService *pService = pServer->createService("0304e97e-41bb-40d7-8cd1-3c745a98ddd3" /* UART service UUID*/);
  // Create a BLE Characteristic
  pTxCharacteristic = pService->createCharacteristic(
      "52cdeb43-e03d-4330-9de2-a3b5246f1745",
      BLECharacteristic::PROPERTY_NOTIFY);

  pTxCharacteristic->addDescriptor(new BLE2902());

  BLECharacteristic *pRxCharacteristic = pService->createCharacteristic(
      "a52b093a-3e56-401d-b2b2-7460b8258827",
      BLECharacteristic::PROPERTY_WRITE);

  pRxCharacteristic->setCallbacks(new MyCallbacks());
  // Start the service
  pService->start();
  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify…");
}

void loopBLE(){
  if (deviceConnected)
  {
    //Serial.print(deviceConnected);

    if (rxVal == "rxdata")
    {
      // Do anything here
    }

    // disconnecting
    if (!deviceConnected && oldDeviceConnected)
    {
      delay(500); // give the bluetooth stack the chance to get things ready
      pServer->startAdvertising(); // restart advertising
      Serial.println("start advertising");
      oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected)
    {
      // do stuff here on connecting
      oldDeviceConnected = deviceConnected;
    }
  }
}
