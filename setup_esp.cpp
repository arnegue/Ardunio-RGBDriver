#include "setup_esp.h"
#include "serialRGB.h"
#include <WiFi.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

const char* ssid = "xxx";
const char* password =   "xxx";

AsyncWebServer server(80);

void setup_esp() {
  //Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi..");
  }

  Serial.print("My IP is: ");
  Serial.println(WiFi.localIP());

  // Just Basic HTML Request
  //server.setDefaultFile("default.html") // Would not work since onNotFound could not fire when requesting
  server.serveStatic("/index.html", SPIFFS, "/index.html");
  server.serveStatic("/", SPIFFS, "/index.html");
  /*server.on("/", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Requested html");
    request->send(SPIFFS, "/index.html", "text/html");
    });

    server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest * request) {
    Serial.println("Requested html");
    request->send(SPIFFS, "/index.html", "text/html");
    });*/

  server.onNotFound([](AsyncWebServerRequest * request) {
    bool found = false;
    String with_slash = request->url();
    String bufferA = "";
    // Remove fist char. Mabye there is a better way with strcopy but then char* and string is difficult
    for(int i = 0; i < with_slash.length(); i++) {
      if (i != 0) {
        bufferA += with_slash[i];
      }
    }
    
    Serial.print("Requested URL: ");
    Serial.println(bufferA);
    int bytesReceived = bufferA.length();
    
    if (bytesReceived == AMOUNT_CHARS_OLD && bufferA[0] == '(' && bufferA[2] == ',' && bufferA[4] == ',' && bufferA[6] == ')') { // (x,x,x)
      Serial.println("Old Protocol");
      oldProtocol(bufferA[1], bufferA[3], bufferA[5]);
      bytesReceived = 0;
      found = true;
    } else if (bytesReceived == AMOUNT_CHARS_NEW && bufferA[0] == '('  && bufferA[AMOUNT_CHARS_NEW - 1] == ')') {
      Serial.println("New Protocol");
      //newProtocol(); // new protocol wont work right now since it uses serial buffer
      bytesReceived = 0;
      found = true;
    } else if (bytesReceived > AMOUNT_CHARS_NEW) {
      Serial.println("Received too much, reset");
      bytesReceived = 0;
      found = false;
    }

    //returnNotFound();
    if (found) {      
      request->send(200);
    } else {
      Serial.print("Did not find \"");
      Serial.print(bufferA);
      Serial.print("\" B[0]=");
      Serial.print( bufferA[0]);
      Serial.print("with length ");
      Serial.println(bufferA.length());
      request->send(404);
    }
  });
  
  server.begin();
}
