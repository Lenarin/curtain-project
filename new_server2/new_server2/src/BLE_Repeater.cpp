#include <BLEDevice.h>
#include <BLEUtils.h>
#include "BLE_Repeater.h"
#include <Arduino.h>
#include <map>

#define SERVICE_UUID                      "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define POSITION_CHARACTERISTIC_UUID      "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define MAX_POSITION_CHARACTERISTIC_UUID  "c0a6e163-d2a0-4a46-8295-e9487f4913fa"
#define MIN_POSITION_CHARACTERISTIC_UUID  "f81d97a3-4db4-4176-8e2c-dc25f7ca8803"
#define NAME_CHARACTERISTIC_UUID          "e911584a-a8ea-4f0e-b088-3e6467a84493"
#define UUID_CHARACTERISTIC_UUID          "107e91b9-e7cb-424f-97cf-0d3034a9cafc"

BLEScan* scaner = NULL;

BLEUUID* serviceId;
BLEUUID* uuidCharUUID;
BLEUUID* positionCharUUID;
BLEUUID* maxPositionCharUUID;
BLEUUID* minPositionCharUUID;
BLEUUID* nameCharUUID;

std::map<String, BLEClient*> connections;
std::map<int, String> connectionReferences;
std::vector<BLEAdvertisedDevice*> toBeConnected;

class ClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    /*Serial.print("Add connection: ");
    connections.insert(
        std::make_pair(
            pclient->getValue(*serviceId, *uuidCharUUID),
            pclient
        )
    );*/
  }

  void onDisconnect(BLEClient* pclient) {
      free(pclient);
  }
};

class AdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
  void onResult(BLEAdvertisedDevice advertisedDevice) {
    if (advertisedDevice.haveServiceUUID() && advertisedDevice.isAdvertisingService(*serviceId)) {
        Serial.println("Inserted");
        toBeConnected.push_back(new BLEAdvertisedDevice(advertisedDevice));
    }
  }
};

BLE_Repeater::BLE_Repeater()
    {
    BLEDevice::init("");

    scaner = BLEDevice::getScan();
    scaner->setAdvertisedDeviceCallbacks(new AdvertisedDeviceCallbacks());
    scaner->setActiveScan(true);
    scaner->setInterval(100);
    scaner->setWindow(50);

    serviceId = new BLEUUID(SERVICE_UUID);
    uuidCharUUID = new BLEUUID(UUID_CHARACTERISTIC_UUID);
    positionCharUUID = new BLEUUID(POSITION_CHARACTERISTIC_UUID);
    maxPositionCharUUID = new BLEUUID(MAX_POSITION_CHARACTERISTIC_UUID);
    minPositionCharUUID = new BLEUUID(MIN_POSITION_CHARACTERISTIC_UUID);
    nameCharUUID = new BLEUUID(NAME_CHARACTERISTIC_UUID);
}

BLE_Repeater::~BLE_Repeater() {
    scaner = NULL;

    BLEDevice::deinit();
}

void BLE_Repeater::SendPacket(JsonObject packet) {
    BLEClient* client = connections[packet["guid"].as<String>()];
    if (packet.containsKey("position")) {
        client->setValue(*serviceId, *positionCharUUID, packet["position"].as<String>().c_str());
    }
    if (packet.containsKey("maxPosition")) {
        client->setValue(*serviceId, *maxPositionCharUUID, packet["maxPosition"].as<String>().c_str());
    }
    if (packet.containsKey("minPosition")) {
        client->setValue(*serviceId, *minPositionCharUUID, packet["minPosition"].as<String>().c_str());
    }
    if (packet.containsKey("name")) {
        client->setValue(*serviceId, *nameCharUUID, packet["name"].as<String>().c_str());
    }
}

bool BLE_Repeater::IsRegistered(std::string adressId) {

};

void BLE_Repeater::connect() {
    for (int i = 0; i < toBeConnected.size(); i++) {
        BLEClient* client = BLEDevice::createClient();
        client->setClientCallbacks(new ClientCallback());
        client->connect(toBeConnected[i]);

        String uuid = client->getValue(*serviceId, *uuidCharUUID).c_str();

        connections.insert(
            std::make_pair(
                uuid,
                client
            )
        );

        connectionReferences.insert(
            std::make_pair(
                client->getConnId(),
                uuid
            )
        );
    }

    toBeConnected.clear();
}

void BLE_Repeater::Scan() {
    DisconectAll();

    scaner->start(3, false);

    scaner->stop();

    connect();
}

void BLE_Repeater::DisconectAll() {
    for (auto x: connections) {
        Serial.println("disconnect");

        x.second->disconnect();
    }
    connections.clear();
    connectionReferences.clear();
}

std::vector<CurtainState>* BLE_Repeater::GetConnections() {
    auto res = new std::vector<CurtainState>();
    for (auto const& x: connections) {
        BLEClient* client = x.second;
        CurtainState* state = new CurtainState();
        state->guid = client->getValue(*serviceId, *uuidCharUUID).c_str();
        state->maxPosition = atoi(client->getValue(*serviceId, *maxPositionCharUUID).c_str());
        state->minPosition = atoi(client->getValue(*serviceId, *minPositionCharUUID).c_str());
        state->name = client->getValue(*serviceId, *nameCharUUID).c_str();
        state->position = atoi(client->getValue(*serviceId, *positionCharUUID).c_str());

        (*res).push_back(*state);
    }

    return res;
}