#ifndef HARDWAREPROFILE_H_INCLUDED
#define HARDWAREPROFILE_H_INCLUDED

#include <avr/io.h>


#define IO_DIR  DDRB

#define ENABLE_UV_PORT PORTB
#define ENABLE_UV_IO  PB3

#define STATUS_LED_PORT PORTB
#define STATUS_LED_IO  PB4

#define SENS_PIN  PINB
#define SENS_IO  PB0

#endif