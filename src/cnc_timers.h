/*
 * cnc_timers.h
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_TIMERS_H_
#define SRC_CNC_TIMERS_H_

#include "stdint.h"

#define TIMER_BASE_HZ ((cnc_double) 16000000.0)
#define TIMER_PRESCALE_DIVIDE ((cnc_double) 1.0)
#define TIMER_HZ (TIMER_BASE_HZ/TIMER_PRESCALE_DIVIDE)
#define TIMER_PERIOD_US ((cnc_double) (((cnc_double) 1000000.0)/TIMER_HZ))
#define IRQ_TIME_US ((cnc_double) 16.0)
#define DEFAULT_TIMER_COUNT ((uint32_t) 1000)
#define DEFAULT_HEATER_TIMER_COUNT ((uint32_t) 62500)
#define MIN_TIMER_COUNT ((uint32_t) 100)
#define MAX_TIMER_COUNT ((uint32_t) 65535)

#define MOTOR_TIMER_PERIOD_US ((cnc_double) TIMER_PERIOD_US)

void init_timers(void);
void init_motor_timer(void);
void init_heater_timer(void);

#endif /* SRC_CNC_TIMERS_H_ */
