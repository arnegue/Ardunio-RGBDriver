#include "Adafruit_NeoPixel.h"
//#include "avr/power.h"

//#define DEBUG

//#define TABLE_NANO
//#define COMPUTER_NANO
#define KITCHEN_NANO

#ifdef TABLE_NANO
#define NUMPIXELS 42
#define RGB_PIN A0
#endif

#ifdef COMPUTER_NANO
#define NUMPIXELS 38
#define RGB_PIN A5
#endif

#ifdef KITCHEN_NANO
#define NUMPIXELS 65
#define RGB_PIN A3 // TODO
#define R_BTN_PIN 2
#define G_BTN_PIN 3
#define B_BTN_PIN 4
#endif


#define AMOUNT_CHARS_NEW ((NUMPIXELS *3) +2) //  42*LEDs a 3 RGB-Values + 2 braces
#define AMOUNT_CHARS_OLD 7

struct last_rgb_state {
  uint8_t r;
  uint8_t g;
  uint8_t b;
} l_rgb;

char bufferA[AMOUNT_CHARS_NEW];
int bytesReceived = 0;
Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, RGB_PIN, NEO_GRB + NEO_KHZ800);


void setup() {
  Serial.begin(115200);
  pixels.begin();
  oldProtocol(0, 0, 0); // Reset every LED
  Serial.print("RESET");
  delay(100);
  Serial.print('(');
  Serial.print((char)NUMPIXELS);
  Serial.print(')');

  l_rgb.r = 0;
  l_rgb.g = 0;
  l_rgb.b = 0;
#ifdef KITCHEN_NANO
  pinMode(R_BTN_PIN, INPUT);
  pinMode(G_BTN_PIN, INPUT);
  pinMode(B_BTN_PIN, INPUT);
  /* Doesn't work since Nano only supports 2 interrupts (and only on Pin 2 and 3)
    attachInterrupt(digitalPinToInterrupt(R_BTN_PIN), r_changed, RISING);
    attachInterrupt(digitalPinToInterrupt(G_BTN_PIN), g_changed, RISING);
    attachInterrupt(digitalPinToInterrupt(B_BTN_PIN), b_changed, RISING);
  */
#endif
}

void loop() {
#ifdef KITCHEN_NANO
  check_changed(R_BTN_PIN, &l_rgb.r);
  check_changed(G_BTN_PIN, &l_rgb.g);
  check_changed(B_BTN_PIN, &l_rgb.b);
  delay(200);

#ifdef DEBUG
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


#ifdef KITCHEN_NANO
void check_changed(uint8_t pin, uint8_t* rgb_val) {
  if (digitalRead(pin) == LOW) {
    *rgb_val = (*rgb_val == 0) ? 255 : 0;
    oldProtocol(l_rgb.r, l_rgb.g, l_rgb.b);
  }
}
#endif

void newProtocol() {
#ifdef DEBUG
  Serial.println("New Protocol");
#endif
  int i = 1;
  int ledIndex = 0;
#ifdef COMPUTER_NANO
  for (ledIndex = NUMPIXELS - 1; ledIndex >= 0; ledIndex--) {
#else
  for (ledIndex = 0; ledIndex < NUMPIXELS; ledIndex++) {
#endif
    pixels.setPixelColor(ledIndex, pixels.Color(bufferA[i], bufferA[i + 1], bufferA[i + 2]));
    i = i + 3;
  }
  pixels.show();
}


void oldProtocol(uint8_t red, uint8_t green, uint8_t blue) {
#ifdef DEBUG
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
#ifdef DEBUG
    if (bytesReceived == 0) {
      Serial.println("New Message");
    }
#endif
    if (bytesReceived == 0 && inChar != '(') {
#ifdef DEBUG
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
      newProtocol();
      bytesReceived = 0;
    } else if (bytesReceived > AMOUNT_CHARS_NEW) {
      Serial.println("Received too much, reset");
      bytesReceived = 0;
    }
  }
}
