#ifndef APP_CONFIG_H_INCLUDED
#define APP_CONFIG_H_INCLUDED

#define LED_PULSE_STEP         2
#define LED_PULSE_PAUSE        500
#define STERILIZATION_DURATION 5000
#define INPUT_DEBOUNCE 4

#if LED_PULSE_STEP > 100
#error "The increasing step cannot be over 100!"
#endif

#if INPUT_DEBOUNCE > 127
#error "Debounce readings (times 2) can't overflow a byte!"
#endif

#if STERILIZATION_DURATION > 0xFFFF
#error "Sterilization time can't overflow a double byte!"
#endif

#endif