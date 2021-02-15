#define ARDUINOJSON_DECODE_UNICODE 1
#include <ESP8266React.h>
#include <CurtainsStateService.h>

#define SERIAL_BAUD_RATE 115200

#define RXD2 16 //RXX2 pin
#define TXD2 17 //TX2 pin

AsyncWebServer server(80);
ESP8266React esp8266React(&server);
CurtainsStateService curtainsStateService = CurtainsStateService(&server, esp8266React.getSecurityManager());

void setup() {
  // start serial and filesystem
  Serial.begin(SERIAL_BAUD_RATE);
  Serial2.begin(SERIAL_BAUD_RATE, SERIAL_8N1, RXD2, TXD2);

  // start the framework and demo project
  esp8266React.begin();
  curtainsStateService.begin();

  // start the server
  server.begin();

}

void loop() {
  // run the framework's loop function
  esp8266React.loop();
  curtainsStateService.loop();
}