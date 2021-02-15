#ifndef _BLE_REPEATER_H
#define _BLE_REPEATER_H

#include <functional>
#include <Arduino.h>
#include <unordered_map>
#include <ArduinoJson.h>

class CurtainState {
public:
    String name;
    String guid;
    long position;
    long maxPosition;
    long minPosition;
};

class BLE_Repeater {
public:
    BLE_Repeater();
    ~BLE_Repeater();

    void SendPacket(JsonObject packet);
    void Scan();
    std::vector<CurtainState>* GetConnections();
    bool IsRegistered(std::string adressId);
    void DisconectAll();
private:
    void connect();
};

#endif