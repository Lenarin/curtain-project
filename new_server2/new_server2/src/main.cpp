#include <Arduino.h>
#include <ArduinoJson.h>
#include <BLE_Repeater.h>

#define SERIAL_BAUD_RATE 115200

#define RXD2 16 //RXX2 pin
#define TXD2 17 //TX2 pin

BLE_Repeater *repeater;

void setup() {
  Serial.begin(SERIAL_BAUD_RATE);
  Serial2.begin(SERIAL_BAUD_RATE, SERIAL_8N1, RXD2, TXD2);

  repeater = new BLE_Repeater();

  Serial.println("Ready");
}

void loop() {
  while (!(Serial.available() || Serial2.available())) {}
  if (Serial.available() || Serial2.available()) {
    const char *input;

    if (Serial.available())
      input = Serial.readString().c_str();
    else
      input = Serial2.readString().c_str();

    Serial.println(input);

    DynamicJsonDocument doc(4096);
    deserializeJson(doc, input);

    String command = doc["command"];

    if (command == "scan") {
      Serial.println("Scanning...");
      repeater->Scan();
      DynamicJsonDocument resp(128);
      resp["status"] = "ok";
      resp["command"] = "scan";
      serializeJson(resp, Serial2);
    }
    else if (command == "get") {
      auto res = repeater->GetConnections();
      DynamicJsonDocument resp(4096);
      resp["status"] = "ok";
      resp["command"] = "get";
      JsonArray data = resp.createNestedArray("curtains");
      for (int i = 0; i < res->size(); i++) {
        JsonObject curtain = data.createNestedObject();
        curtain["name"] = (*res)[i].name.c_str();
        curtain["guid"] = (*res)[i].guid.c_str();
        curtain["position"] = (*res)[i].position;
        curtain["maxPosition"] = (*res)[i].maxPosition;
        curtain["minPosition"] = (*res)[i].minPosition;
      }

      serializeJson(resp, Serial2);
    }
    else if (command == "set") {
      JsonArray curtains = doc["curtains"];
      for (auto curtain : curtains) {
        repeater->SendPacket(curtain);
      }

      DynamicJsonDocument resp(128);
      resp["status"] = "ok";
      resp["command"] = "set";
      serializeJson(resp, Serial2);
    }
  }
  delay(10);
}