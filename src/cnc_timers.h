/*
 * cnc_timers.h
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_TIMERS_H_
#define SRC_CNC_TIMERS_H_

#include "stdint.h"
#include "cnc_heaters.h"
#include "cnc_motors.h"
#include "cnc_pid.h"

/*#define MOTOR_TIMER_PERIOD_US  1 // 1 microsecond
#define MOTOR_TIMER_CLOCK      cmuClock_TIMER0
#define MOTOR_TIMER            TIMER0
#define MOTOR_TIMER_TOP        500
#define MOTOR_TIMER_IRQ_NUMBER TIMER0_IRQn*/

#define MOTOR_TIMER_PERIOD_US  1 // 1 microsecond
#define MOTOR_TIMER_CLOCK      cmuClock_TIMER1
#define MOTOR_TIMER            TIMER1
#define MOTOR_TIMER_TOP        1000
#define MOTOR_TIMER_IRQ_NUMBER TIMER1_IRQn

/*#define PID_TIMER_PERIOD_US  10  // 10 milliseconds
#define PID_TIMER_CLOCK      cmuClock_TIMER1
#define PID_TIMER            TIMER1
#define PID_TIMER_TOP        1000
#define PID_TIMER_IRQ_NUMBER TIMER1_IRQn*/

void init_timers(void);
void init_motor_timer(void);
void init_heater_timer(void);

#endif /* SRC_CNC_TIMERS_H_ */
