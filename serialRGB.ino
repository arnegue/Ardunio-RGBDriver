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

void writeNumPixels() {
  Serial.print('(');
  Serial.print((char)NUMPIXELS);
  Serial.print(')');  
}

void newProtocol(char* buffer_pntr) {
#ifdef DEBUG_SERIAL
  Serial.println("New Protocol");
#endif
  int i = 1;
  int ledIndex = 0;
#ifdef COMPUTER_NANO
  for (ledIndex = NUMPIXELS - 1; ledIndex >= 0; ledIndex--) {
#else
  for (ledIndex = 0; ledIndex < NUMPIXELS; ledIndex++) {
#endif
    pixels.setPixelColor(ledIndex, pixels.Color(buffer_pntr[i], buffer_pntr[i + 1], buffer_pntr[i + 2]));
    i = i + 3;
  }
  pixels.show();
}


void oldProtocol(uint8_t red, uint8_t green, uint8_t blue) {
#ifdef DEBUG_SERIAL
  Serial.println("Old Protocol");
#endif
  for (int ledIndex = 0; ledIndex < NUMPIXELS; ledIndex++) {
    pixels.setPixelColor(ledIndex, pixels.Color(red, green, blue));
  }
  pixels.show();

  l_rgb.r = red;
  l_rgb.g = green;
  l_rgb.b = blue;
}


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
