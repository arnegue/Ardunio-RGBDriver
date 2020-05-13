#include "Adafruit_NeoPixel.h"
#include "serialRGB.h"

char bufferA[AMOUNT_CHARS_NEW];
int bytesReceived = 0;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(115200);
  pixels.begin();
  oldProtocol(0, 0, 0); // Reset every LED
#ifdef DEBUG_SERIAL
  Serial.print("RESET");
#endif
  delay(100);
  writeNumPixels();

  l_rgb.r = 0;
  l_rgb.g = 0;
  l_rgb.b = 0;
#ifdef KITCHEN_ESP // TODO
  setup_esp();
#endif
}

void loop() {

#ifdef KITCHEN_ESP
  esp_loop();
#endif
}

/**
 * If a client connects via serial, write amount of own RGBs (for new protocol) to client
 */
void writeNumPixels() {
  Serial.print('(');
  Serial.print((char)NUMPIXELS);
  Serial.print(')');  
}


/**
 * New Protocol (NUMPIXELS*RGB) (e.g. for 1 red, 1 green, 1 blue RGBs (NUMPIXELS=3) : 0x28 0xFF 0x00 0x00 0x00 0xFF 0x00 0x00 0x00 0xFF 0x29
 */
void newProtocol(char* buffer_pntr) {
#ifdef DEBUG_SERIAL
  Serial.println("New Protocol");
#endif
  int i = 1;
  int ledIndex = 0;
#ifdef REVERSE_RGB
  for (ledIndex = NUMPIXELS - 1; ledIndex >= 0; ledIndex-=3) {
#else
  for (ledIndex = 0; ledIndex < NUMPIXELS; ledIndex+=3) {
#endif
    pixels.setPixelColor(ledIndex, buffer_pntr[i], buffer_pntr[i + 1], buffer_pntr[i + 2]);
  }
  pixels.show();
}

/**
 * OldProtocol (R,G,B) (e.g. for white: 0x28 0xFF 0x2C 0xFF 0x2C 0xFF 0x29)
 */
void oldProtocol(uint8_t red, uint8_t green, uint8_t blue) {
#ifdef DEBUG_SERIAL
  Serial.println("Old Protocol");
#endif
  for (int ledIndex = 0; ledIndex < NUMPIXELS; ledIndex++) {
    pixels.setPixelColor(ledIndex, red, green, blue);
  }
  pixels.show();

  l_rgb.r = red;
  l_rgb.g = green;
  l_rgb.b = blue;
}

/**
 * On Serials event (incoming bytes), decide which protocol to use
 */
void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar =  (char)Serial.read();
#ifdef DEBUG_SERIAL
    if (bytesReceived == 0) {
      Serial.println("New Message");
    }
#endif
    if (bytesReceived == 0 && inChar != '(') {
#ifdef ERROR_SERIAL
      Serial.print("Received weird message. Resetting, first byte was ");
      Serial.println((int)inChar);
#endif
      bytesReceived = 0;
      return;
    }
    // add it to the inputString:
    bufferA[bytesReceived] = inChar;
    bytesReceived++;

    if (bytesReceived == AMOUNT_CHARS_OLD && bufferA[0] == '(' && bufferA[2] == ',' && bufferA[4] == ',' && bufferA[6] == ')') {
      oldProtocol(bufferA[1], bufferA[3], bufferA[5]);
      bytesReceived = 0;
    } else if (bytesReceived == AMOUNT_CHARS_NEW && bufferA[0] == '('  && bufferA[AMOUNT_CHARS_NEW - 1] == ')') {
      newProtocol(bufferA);
      bytesReceived = 0;
    } else if (bytesReceived > AMOUNT_CHARS_NEW) {
#ifdef ERROR_SERIAL
      Serial.println("Received too much, reset");
#endif
      bytesReceived = 0;
    }
  }
}
