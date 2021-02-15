#include <CurtainsStateService.h>

CurtainsStateService::CurtainsStateService(
    AsyncWebServer* server,
    SecurityManager* securityManager) :
    _webSocket(
        CurtainsState::read,
        CurtainsState::update,
        this,
        server,
        CURTAINS_SETTINGS_SOCKET_PATH,
        securityManager,
        AuthenticationPredicates::IS_AUTHENTICATED
    )
{


};

void CurtainsStateService::begin() {
    _state.inScan = false;
    _state.curtains = new std::unordered_map<std::string, CurtainState*>();
}

void CurtainsStateService::loop() {
    if (Serial.available() || Serial2.available()) {
        const char *input;

        if (Serial.available())
        input = Serial.readString().c_str();
        else
        input = Serial2.readString().c_str();


        DynamicJsonDocument doc(4096);
        deserializeJson(doc, input);

        String command = doc["command"];
        Serial.println(command.c_str());

        this->update([&](CurtainsState& state) {
            String command = doc["command"];
            Serial.println(command.c_str());

            if (command == "scan") {
                Serial.println("Scan command!");
                DynamicJsonDocument resp(4096);
                resp["command"] = "get";
                serializeJson(resp, Serial2);
                return StateUpdateResult::CHANGED;
            }
            else if (command == "get") {
                Serial.println("Get command!");
                Serial.println(input);
                JsonArray curtains = doc["curtains"];
                state.curtains->clear();
                for (auto x: curtains) {
                    CurtainState *curtain = new CurtainState();
                    curtain->guid = x["guid"].as<String>().c_str();
                    curtain->maxPosition = x["maxPosition"];
                    curtain->minPosition = x["minPosition"];
                    curtain->name = x["name"].as<String>().c_str();
                    curtain->position = x["position"];

                    (*state.curtains)[curtain->guid] = curtain;
                }
                state.inScan = false;
                return StateUpdateResult::CHANGED;
            }

            return StateUpdateResult::UNCHANGED;
        }, "serial");
    }
}

void CurtainsStateService::onConfigUpdated() {

}