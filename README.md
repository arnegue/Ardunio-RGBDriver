This project is the main part of all RGB-LED-Strip-Projects: It finally displays the LEDs (WS2812) using [neopixel](https://learn.adafruit.com/adafruit-neopixel-uberguide)-library 

# How does this work

Commands are sent via serial:

* Baudrate: 115200
* StopBits: 1
* Parity: None
* DataBits: 8

## Old protocol 
### Description
Considering you have the old protocol (only shows one color to all LEDs).
The protocol is quiet simple: 7 byte each message
`(R,G,B)`
Every character above is one byte.

### Example
You want to show Yellow (Red 255, Green 255, Blue 0)

|               | Byte0   | Byte1 | Byte2 | Byte3 | Byte4 | Byte5 | Byte6 | 
| :---:         | :---:   | :---: | :---: | :---: | :---: | :---: | :---: |
| Hex           | 0x28    |  0xFF |  0x2C |  0xFF |  0x2C | 0x00  |  0x29 |
| Description   | Start ( |   R   |  ,    |  G    |  ,    | B     | ) End |

## New protocol  
### Description

This protocol support the LED-Strip where each LED is individually controllable.

The message length depends on the amount of LEDs on one strip. The amount has to be programmed manually into the Arduino/ESP in `serialRGB.h` under `#define NUMPIXELS`. 

The protocol is similar to the old protocol, but the commas are missing, as they are not really needed. The protocol-start and -end is like the old protocol. A `(` at the start and a `)` at the end
The amount of bytes sent is: AmountRGB * 3 + 2.

### Example
For this example let's consider that there are three LEDs on the RGB-Strip. The amount of bytes sent are: 3 * 3 + 2 = 11

`(RGBRGBRGB)`
If you want to show it 0LightGreen, 1Orange, 2WeirdBlue:


|             | Byte0   | Byte1 | Byte2 | Byte3 | Byte4 | Byte5 | Byte6 | Byte7 | Byte8 | Byte9 | Byte10 |
| :---:       | :---:   | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---: | :---:  |
| Hex         | 0x28    |  0x80 |  0xFF |  0x80 |  0xFF |  0x80 |  0x00 | 0x20  | 0x50  |  0xD5 |  0x29  |
| Description | Start ( |  R0   |  G0   |  B0   |  R1   |  G1   |  B1   |  R2   |  G3   |  B4   | ) End  |

