
#include <avr/io.h>
#include <avr/interrupt.h>
#include "hardwareprofile.h"


static uint8_t tick = 0;


void timer_init(void) {
    TCCR0A |= _BV(WGM01);                // set timer counter mode to CTC
    TCCR0B |= _BV(CS00) | _BV(CS01);     // 64 prescaler (CLK=1200000Hz/64=1.875KHz, increments every 0.053333ms)
    OCR0A = 93;                          // set Timer's counter max value (93.750585941 makes 5ms)
    TIMSK0 |= _BV(OCIE0A);               // Enable Timer CTC interrupt
    sei();                               // enable global interrupts
}


void timer_clear_tick(void) {
    tick = 0;
}


uint8_t timer_ticked(void) {
    return tick;
}


ISR(TIM0_COMPA_vect, ISR_NAKED) {
    tick = 1;
    reti();
}