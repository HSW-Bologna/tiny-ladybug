#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/wdt.h>
#include <util/delay.h>

#include "peripherals/hardwareprofile.h"
#include "peripherals/timer.h"
#include "app_config.h"


#define LED_ON()     STATUS_LED_PORT |= _BV(STATUS_LED_IO)
#define LED_OFF()    STATUS_LED_PORT &= ~_BV(STATUS_LED_IO)
#define UV_ON()      ENABLE_UV_PORT |= _BV(ENABLE_UV_IO)
#define UV_OFF()     ENABLE_UV_PORT &= ~_BV(ENABLE_UV_IO)
#define DOOR_STATE() ((SENS_PIN & _BV(SENS_IO)) > 0)


typedef enum {
    STATE_ERROR = 0,
    STATE_STANDBY,
    STATE_WAIT,
    STATE_STERILIZATION,
} state_t;


enum {
    PULSE_PAUSED,
    PULSE_RUNNING,
};


typedef enum {
    EVENT_NONE       = 0,
    EVENT_POWER_ON   = 1,
    EVENT_DOOR_OPEN  = 2,
    EVENT_DOOR_CLOSE = 3,
    EVENT_TIMER_OVER = 4,
} event_t;


static void wait_state(void);


static uint16_t pwm_duty_cycle      = 0;
static uint16_t counter             = 0;
static uint8_t  led_pulse           = PULSE_PAUSED;
static state_t  state               = STATE_STANDBY;
static uint8_t  event               = EVENT_POWER_ON;
static uint8_t  door_state          = 0;
static uint8_t  old_door_state      = 0;
static uint8_t  tmp_door_state      = 0;
static uint8_t  door_counter        = 0;
static uint16_t sterilization_timer = 0;


int main(void) {
    wdt_enable(WDTO_60MS);
    wdt_reset();
    IO_DIR = (_BV(ENABLE_UV_IO) | _BV(STATUS_LED_IO)) & (~_BV(SENS_IO));

    timer_init();

    tmp_door_state = DOOR_STATE();
    for (uint8_t i = 0; i < INPUT_DEBOUNCE * 2; i++) {
        wdt_reset();
        if (tmp_door_state == DOOR_STATE()) {
            if (door_counter > INPUT_DEBOUNCE) {
                door_state = tmp_door_state;
                break;
            } else {
                door_counter++;
            }
        }
        _delay_ms(5);
    }

    old_door_state = door_state;
    state          = door_state ? STATE_ERROR : STATE_STANDBY;

    /* Main loop */
    for (;;) {
        /* On event */
        if (event != EVENT_NONE) {
            switch (state) {
                case STATE_ERROR:
                    switch (event) {
                        case EVENT_POWER_ON:
                            UV_OFF();
                            counter        = 0;
                            pwm_duty_cycle = 50;
                            break;

                        case EVENT_DOOR_CLOSE:
                            LED_OFF();
                            UV_OFF();
                            state = STATE_STANDBY;
                            break;

                        default:
                            break;
                    }
                    break;

                case STATE_STANDBY:
                    switch (event) {
                        case EVENT_POWER_ON:
                            LED_OFF();
                            UV_OFF();
                            break;

                        case EVENT_DOOR_OPEN:
                            wait_state();
                            break;

                        default:
                            break;
                    }
                    break;

                case STATE_WAIT:
                    switch (event) {
                        case EVENT_DOOR_CLOSE:
                            sterilization_timer = 0;
                            LED_ON();
                            UV_ON();
                            state = STATE_STERILIZATION;
                            break;

                        default:
                            break;
                    }
                    break;

                case STATE_STERILIZATION:
                    switch (event) {
                        case EVENT_DOOR_OPEN:
                            wait_state();
                            break;

                        case EVENT_TIMER_OVER:
                            LED_OFF();
                            UV_OFF();
                            state = STATE_STANDBY;
                            break;

                        default:
                            break;
                    }
                    break;
            }

            event = EVENT_NONE;
        }

        /* Continuous work */
        if (timer_ticked()) {
            wdt_reset();
            timer_clear_tick();

            tmp_door_state = DOOR_STATE();
            if (tmp_door_state == old_door_state && tmp_door_state != door_state) {
                if (door_counter < INPUT_DEBOUNCE) {
                    door_counter++;
                } else {
                    door_state = tmp_door_state;
                    event      = door_state ? EVENT_DOOR_OPEN : EVENT_DOOR_CLOSE;
                }
            } else {
                door_counter   = 0;
                old_door_state = tmp_door_state;
            }

            counter++;

            switch (state) {
                case STATE_ERROR:
                    if (counter < pwm_duty_cycle)
                        LED_ON();
                    else
                        LED_OFF();
                    if (counter >= 100)
                        counter = 0;
                    break;

                case STATE_WAIT: {
                    switch (led_pulse) {
                        case PULSE_PAUSED:
                            if (counter > (LED_PULSE_PAUSE / 5)) {
                                led_pulse      = PULSE_RUNNING;
                                pwm_duty_cycle = LED_PULSE_STEP;
                                counter        = 0;
                            } else {
                                LED_OFF();
                            }
                            break;

                        case PULSE_RUNNING:
                            if (counter > 0 && counter % 20 == 0)
                                pwm_duty_cycle += LED_PULSE_STEP;

                            if (counter < pwm_duty_cycle)
                                LED_ON();
                            else
                                LED_OFF();

                            if (pwm_duty_cycle >= 100 && counter >= 100)
                                led_pulse = PULSE_PAUSED;

                            if (counter >= 100)
                                counter = 0;
                            break;

                        default:
                            break;
                    }
                    break;
                }

                case STATE_STERILIZATION:
                    if (sterilization_timer > STERILIZATION_DURATION / TICK_DURATION) {
                        event = EVENT_TIMER_OVER;
                    } else {
                        sterilization_timer++;
                    }
                    break;

                default:
                    break;
            }
        }

        _delay_us(100);
    }
}


static void wait_state(void) {
    UV_OFF();
    counter        = 0;
    pwm_duty_cycle = 0;
    led_pulse      = PULSE_PAUSED;
    state          = STATE_WAIT;
}