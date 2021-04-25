#ifndef TIMER_H_INCLUDED
#define TIMER_H_INCLUDED

#include <stdint.h>

#define TICK_DURATION 5

void    timer_init(void);
void    timer_clear_tick(void);
uint8_t timer_ticked(void);

#endif