#include <stdint.h>

//#define DEBUG

//#define TABLE_NANO
//#define COMPUTER_NANO
#define KITCHEN_ESP

#ifdef TABLE_NANO
#define NUMPIXELS 42
#define RGB_PIN A0
#endif

#ifdef COMPUTER_NANO
#define NUMPIXELS 38
#define RGB_PIN A5
#endif

#ifdef KITCHEN_ESP
#define NUMPIXELS 65
#define RGB_PIN 12
#define R_BTN_PIN 2
#define G_BTN_PIN 3
#define B_BTN_PIN 4
#endif

#ifndef NUMPIXELS
#error Cannot continue compilation. NUMPIXELS was not set
#endif

#define AMOUNT_CHARS_NEW ((NUMPIXELS *3) +2) //  42*LEDs a 3 RGB-Values + 2 braces
#define AMOUNT_CHARS_OLD 7 // (X,X,X)

#ifdef KITCHEN_ESP
#include "setup_esp.h" // define this after general RGB-definitions
#endif

struct last_rgb_state {
  uint8_t r;
  uint8_t g;
  uint8_t b;
};


void oldProtocol(uint8_t red, uint8_t green, uint8_t blue);
void newProtocol();
