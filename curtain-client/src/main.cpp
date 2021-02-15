#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <Arduino.h>
#include <EEPROM.h>

// See the following for generating UUIDs:
// https://www.uuidgenerator.net/

#define SERVICE_UUID                      "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define POSITION_CHARACTERISTIC_UUID      "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define MAX_POSITION_CHARACTERISTIC_UUID  "c0a6e163-d2a0-4a46-8295-e9487f4913fa"
#define MIN_POSITION_CHARACTERISTIC_UUID  "f81d97a3-4db4-4176-8e2c-dc25f7ca8803"
#define NAME_CHARACTERISTIC_UUID          "e911584a-a8ea-4f0e-b088-3e6467a84493"
#define UUID_CHARACTERISTIC_UUID          "107e91b9-e7cb-424f-97cf-0d3034a9cafc"

//#define DEVICE_UUID                       "e80fa056-465b-4a0e-b714-61547bbaf40a"
#define DEVICE_UUID                       "085fc865-93df-4c1c-a52c-481e0681ead2"

EEPROMClass memPosition("eeprom0", 0x500);
EEPROMClass memMaxPosition("eeprom1", 0x500);
EEPROMClass memMinPosition("eeprom2", 0x500);
EEPROMClass memName("eeprom3", 0x500);

class PositionCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      std::string value = characteristic->getValue();

      memPosition.writeString(0, value.c_str());
      memPosition.commit();
    }
};

class MaxPositionCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      std::string value = characteristic->getValue();

      memMaxPosition.writeString(0, value.c_str());
      memMaxPosition.commit();
    }
};

class MinPositionCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      std::string value = characteristic->getValue();

      memMinPosition.writeString(0, value.c_str());
      memMinPosition.commit();
    }
};

class NameCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *characteristic) {
      std::string value = characteristic->getValue();

      memName.writeString(0, value.c_str());
      memName.commit();
    }
};

BLEServer* pServer = NULL;
BLECharacteristic* pCharacteristic = NULL;
bool deviceConnected = false;
bool oldDeviceConnected = false;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
      BLEDevice::startAdvertising();
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};



void setup() {
  Serial.begin(115200);

  if (!memName.begin(memName.length())) {
    Serial.println("Failed to initialise memory for Name");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  if (!memMaxPosition.begin(memMaxPosition.length())) {
    Serial.println("Failed to initialise memory for Max Position");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  if (!memMinPosition.begin(memMinPosition.length())) {
    Serial.println("Failed to initialise memory for Min Position");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  if (!memPosition.begin(memPosition.length())) {
    Serial.println("Failed to initialise memory for Position");
    Serial.println("Restarting...");
    delay(1000);
    ESP.restart();
  }

  // Create the BLE Device
  BLEDevice::init("ESP32");

  // Create the BLE Server
  pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  BLECharacteristic *positionCharacteristic = pService->createCharacteristic(
                                         POSITION_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  BLECharacteristic *maxPositionCharacteristic = pService->createCharacteristic(
                                         MAX_POSITION_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  BLECharacteristic *minPositionCharacteristic = pService->createCharacteristic(
                                         MIN_POSITION_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  BLECharacteristic *nameCharacteristic = pService->createCharacteristic(
                                         NAME_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_WRITE
                                       );

  BLECharacteristic *uuidCharacteristic = pService->createCharacteristic(
                                         UUID_CHARACTERISTIC_UUID,
                                         BLECharacteristic::PROPERTY_READ
                                       );

  positionCharacteristic->setCallbacks(new PositionCallbacks());
  maxPositionCharacteristic->setCallbacks(new MaxPositionCallbacks());
  minPositionCharacteristic->setCallbacks(new MinPositionCallbacks());
  nameCharacteristic->setCallbacks(new NameCallbacks());

  positionCharacteristic->addDescriptor(new BLE2902());
  maxPositionCharacteristic->addDescriptor(new BLE2902());
  minPositionCharacteristic->addDescriptor(new BLE2902());
  nameCharacteristic->addDescriptor(new BLE2902());
  uuidCharacteristic->addDescriptor(new BLE2902());

  String minPos;
  String maxPos;
  String position;
  String name;

  name = memName.readString(0);
  maxPos = memMaxPosition.readString(0);
  minPos = memMinPosition.readString(0);
  position = memPosition.readString(0);

  positionCharacteristic->setValue(position.c_str());
  maxPositionCharacteristic->setValue(maxPos.c_str());
  minPositionCharacteristic->setValue(minPos.c_str());
  uuidCharacteristic->setValue(DEVICE_UUID);
  nameCharacteristic->setValue(name.c_str());


  // Start the service
  pService->start();

  // Start advertising
  BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
  pAdvertising->addServiceUUID(SERVICE_UUID);
  pAdvertising->setScanResponse(true);
  pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
  BLEDevice::startAdvertising();
}

void loop() {
    // notify changed value
    if (deviceConnected) {
        Serial.println("Connected!");
        delay(10); // bluetooth stack will go into congestion, if too many packets are sent, in 6 hours test i was able to go as low as 3ms
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}