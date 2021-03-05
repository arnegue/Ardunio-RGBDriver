#ifndef SERIAL_RGB_H
#define SERIAL_RGB_H
#include <stdint.h>

//#define DEBUG_SERIAL
#define ERROR_SERIAL

#define TABLE_NANO
//#define COMPUTER_NANO
//#define KITCHEN_ESP
//#define JENKINS_UNO

#ifdef TABLE_NANO
#define NUMPIXELS 42
#define RGB_PIN A1
#endif

#ifdef COMPUTER_NANO
#define NUMPIXELS 38
#define RGB_PIN A5
#define REVERTED
#endif

#ifdef JENKINS_UNO
#define NUMPIXELS 4
#define RGB_PIN A0
#endif

#ifdef KITCHEN_ESP
#include "setup_esp.h" // define this after general RGB-definitions
#endif

#ifndef NUMPIXELS
#error Cannot continue compilation. NUMPIXELS was not set
#endif

#define AMOUNT_CHARS_NEW ((NUMPIXELS *3) +2) //  42*LEDs a 3 RGB-Values + 2 braces
#define AMOUNT_CHARS_OLD 7 // (X,X,X)
#define AMOUNT_CHARS_OLD_DEC 13// (123,123,123) // Used by esp

struct last_rgb_state { // TODO whyn ot use strip.Color instead
  uint8_t r;
  uint8_t g;
  uint8_t b;
};

static last_rgb_state l_rgb;


void oldProtocol(uint8_t red, uint8_t green, uint8_t blue);
void newProtocol(char* buffer_pntr);

#endif
