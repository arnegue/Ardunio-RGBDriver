#ifndef SETUP_ESP_H
#define SETUP_ESP_H
#ifdef KITCHEN_ESP

#define NUMPIXELS 65
#define RGB_PIN 12
#define R_BTN_PIN 14
#define G_BTN_PIN 27
#define B_BTN_PIN 26

/**
 * Setups Button-Pins, WiFi and the SPIFF file system for serving Index.html
 */
void setup_esp();

/**
   Checks every sensor if their state changed
*/
void esp_loop();

#endif
#endif
