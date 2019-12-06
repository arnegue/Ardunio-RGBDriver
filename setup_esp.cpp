#include "setup_esp.h"
#include "serialRGB.h"

#ifdef KITCHEN_ESP

#include <WiFi.h>
#include "SPIFFS.h"
#include "ESPAsyncWebServer.h"

const char* ssid = ""; // Put in here
const char* password =  ""; // Put in here

AsyncWebServer server(80);

char esp_buffer[AMOUNT_CHARS_NEW];
void setup_esp() {
  //Serial.begin(115200);

  if (!SPIFFS.begin(true)) {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }
  WiFi.begin(ssid, password);
  WiFi.setHostname("kuecheRGB");

  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Serial.print("My IP is: ");
  Serial.println(WiFi.localIP());

  // Just Basic HTML Request
  //server.setDefaultFile("default.html") // Would not work since onNotFound could not fire when requesting
  server.serveStatic("/index.html", SPIFFS, "/index.html");
  server.serveStatic("/jq.scp-3.0.5.min.js", SPIFFS, "/jq.scp-3.0.5.min.js");
  server.serveStatic("/jquery-2.0.3.min.js", SPIFFS, "/jquery-2.0.3.min.js");
  server.serveStatic("/wheelcolorpicker.css", SPIFFS, "/wheelcolorpicker.css");
  server.serveStatic("/", SPIFFS, "/index.html");
  server.serveStatic("/jquery-wheelcolorpicker/", SPIFFS, "/jquery-wheelcolorpicker/");

  server.onNotFound([](AsyncWebServerRequest * request) {
    String with_slash = request->url();

    // Remove fist char. Mabye there is a better way with strcopy but then char* and string is difficult
    int i;
    for (i = 1; i < with_slash.length(); i++) {
      esp_buffer[i - 1] = with_slash[i];
    }
    for (i = i - 1; i < AMOUNT_CHARS_NEW; i++) {
      esp_buffer[i] = 0;
    }
#ifdef DEBUG_SERIAL
    Serial.print("Requested URL: ");
    Serial.println(esp_buffer);
#endif
    int bytesReceived = with_slash.length() - 1; // -1 for /

    // Binary
    if (bytesReceived == AMOUNT_CHARS_OLD && esp_buffer[0] == '(' && esp_buffer[2] == ',' && esp_buffer[4] == ',' && esp_buffer[6] == ')') { // (x,x,x)
      oldProtocol(esp_buffer[1], esp_buffer[3], esp_buffer[5]);
#ifdef DEBUG_SERIAL
      Serial.println("Old Protocol (bin)");
#endif
      request->send(200);
    }
    // Decimal (could get up to 999, but that doesn't matter because of byte (255 is max)
    else if (bytesReceived == AMOUNT_CHARS_OLD_DEC && esp_buffer[0] == '(' && esp_buffer[4] == ',' && esp_buffer[8] == ',' && esp_buffer[12] == ')') { // (123,123,123)
      // -48 for ascii number representation
      byte r = (esp_buffer[1] - 48) * (100 + esp_buffer[2]  - 48) * 10 + (esp_buffer[3]  - 48);
      byte g = (esp_buffer[5] - 48) * (100 + esp_buffer[6]  - 48) * 10 + (esp_buffer[7]  - 48);
      byte b = (esp_buffer[9] - 48) * (100 + esp_buffer[10] - 48) * 10 + (esp_buffer[11] - 48);
      oldProtocol(r, g, b);
#ifdef DEBUG_SERIAL
      Serial.println("Old Protocol (DEC)");
#endif
      request->send(200);
      }
    // New Protocol (binary)
    else if (bytesReceived == AMOUNT_CHARS_NEW && esp_buffer[0] == '('  && esp_buffer[AMOUNT_CHARS_NEW - 1] == ')') {
#ifdef DEBUG_SERIAL
      Serial.println("New Protocol");
#endif
      newProtocol(esp_buffer); // new protocol wont work right now since it uses serial buffer
      request->send(200);
    } else if (bytesReceived > AMOUNT_CHARS_NEW) {
#ifdef DEBUG_SERIAL
      Serial.println("Received too much, reset");
#endif
      request->send(404);
    } else {
#ifdef DEBUG_SERIAL
      Serial.println("Could not serve request. Unknown string");
#endif
      request->send(404);
    }
  });

  server.begin();

  pinMode(R_BTN_PIN, INPUT);
  pinMode(G_BTN_PIN, INPUT);
  pinMode(B_BTN_PIN, INPUT);
}


void check_changed(uint8_t pin, uint8_t* rgb_val) {
  // Checks if the state to the specified pin changed (compared to l_rgb)
  if (digitalRead(pin) == LOW) {
    *rgb_val = (*rgb_val == 0) ? 255 : 0;
    oldProtocol(l_rgb.r, l_rgb.g, l_rgb.b);
  }
}

void esp_loop() {
#ifdef KITCHEN_ARDUINO // TODO
  check_changed(R_BTN_PIN, &l_rgb.r);
  check_changed(G_BTN_PIN, &l_rgb.g);
  check_changed(B_BTN_PIN, &l_rgb.b);
  delay(200);

#ifdef DEBUG_SERIAL
  Serial.print("(");
  Serial.print(l_rgb.r);
  Serial.print(",");
  Serial.print(l_rgb.g);
  Serial.print(",");
  Serial.print(l_rgb.b);
  Serial.println(")");
#endif
#endif
}
#endif
