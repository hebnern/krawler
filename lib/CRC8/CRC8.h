#ifndef CRC8_H
#define CRC8_H

#if defined(ARDUINO) && ARDUINO >= 100
#include <Arduino.h>
#else
#include <WProgram.h>
#endif

byte CRC8(byte const *data, byte len);

#endif
