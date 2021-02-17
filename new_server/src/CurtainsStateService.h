#ifndef CurtainsStateService_h
#define CurtainsStateService_h

#include <WebSocketTxRx.h>
#include <unordered_map>
#include <vector>

#define CURTAINS_SETTINGS_SOCKET_PATH "/ws/curtainsState"

class CurtainState {
public:
    std::string name;
    std::string guid;
    long position;
    long maxPosition;
    long minPosition;
};

class CurtainsState {
public:
    std::unordered_map<std::string, CurtainState*>* curtains;
    bool inScan;

    static void read(CurtainsState& settings, JsonObject& root) {
        JsonArray data = root.createNestedArray("curtains");
        for (auto el : *settings.curtains) {
            JsonObject curtain = data.createNestedObject();
            curtain["name"] = (*el.second).name.c_str();
            curtain["guid"] = (*el.second).guid.c_str();
            curtain["position"] = (*el.second).position;
            curtain["maxPosition"] = (*el.second).maxPosition;
            curtain["minPosition"] = (*el.second).minPosition;
        }

        root["inScan"] = settings.inScan;
        root["command"] = "set";
    }

    static StateUpdateResult update(JsonObject& root, CurtainsState& cstate) {
        if (root["command"] != NULL) {
            String command = root["command"].as<String>();
            if (command == "refresh") {
                DynamicJsonDocument resp(4096);
                resp["command"] = "scan";
                serializeJson(resp, Serial2);
                cstate.inScan = true;

                return StateUpdateResult::CHANGED;
            }
        }

        if (root["curtains"] != NULL) {
            JsonArray states = root["curtains"];
            DynamicJsonDocument resp(4096);

            resp["command"] = "set";
            auto curtainsResp = resp.createNestedArray("curtains");

            for (JsonVariant state: states) {
                String guid = state["guid"].as<String>();
                auto curtain = (*cstate.curtains)[guid.c_str()];
                auto curtainResp = curtainsResp.createNestedObject();
                curtainResp["guid"] = guid.c_str();

                if (state.containsKey("position")) {
                    long pos = state["position"].as<long>();
                    if (curtain->position != pos) {
                        curtain->position = pos;
                        curtainResp["position"] = pos;
                    }
                }

                if (state.containsKey("name")) {
                    String name = state["name"].as<String>();
                    if (String(curtain->name.c_str()) != name) {
                        curtain->name = name.c_str();
                        curtainResp["name"] = name;
                    }
                }

                if (state.containsKey("maxPosition")) {
                    long pos = state["maxPosition"].as<long>();
                    if (curtain->maxPosition != pos) {
                        curtain->maxPosition = pos;
                        curtainResp["maxPosition"] = pos;
                    }
                }

                if (state.containsKey("minPosition")) {
                    long pos = state["minPosition"].as<long>();
                    if (curtain->minPosition != pos) {
                        curtain->minPosition = pos;
                        curtainResp["minPosition"] = pos;
                    }
                }
            }

            serializeJson(resp, Serial2);

            return StateUpdateResult::CHANGED;
        }
        return StateUpdateResult::UNCHANGED;
    }
};

class CurtainsStateService : public StatefulService<CurtainsState> {
public:
    CurtainsStateService(
        AsyncWebServer* server,
        SecurityManager* securityManager);

    void begin();
    void loop();

private:
    WebSocketTxRx<CurtainsState> _webSocket;

    void onConfigUpdated();
};

#endif