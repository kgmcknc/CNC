/*
 * cnc_timers.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifdef SILABS
   #include "em_timer.h"
   #include "em_cmu.h"
   RTCDRV_TimerID_t motor_update_timer;
   RTCDRV_TimerID_t pid_update_timer;
#else
   #include "Arduino.h"
#endif

#include "main.h"
#include "cnc_timers.h"
#include "cnc_motors.h"
#include <stddef.h>

/***************************************************************************//**
 * Local prototypes
 ******************************************************************************/

void init_timers(void){
   #ifdef SILABS
   #else
   noInterrupts(); // disable all interrupts
   #endif

	init_heater_timer();
	init_motor_timer();

   #ifdef SILABS
   #else
   interrupts(); // enable all interrupts
   #endif
}

void init_heater_timer(void){
	#ifdef SILABS
   CMU_ClockEnable(PID_TIMER_CLOCK, true);
	TIMER_TopSet(PID_TIMER, PID_TIMER_TOP);
	TIMER_Init_TypeDef pid_timer_init = TIMER_INIT_DEFAULT;
	TIMER_IntEnable(PID_TIMER, TIMER_IF_OF);
	NVIC_EnableIRQ(PID_TIMER_IRQ_NUMBER);
	TIMER_Init(PID_TIMER, &pid_timer_init);
   #else
   // initialize Timer2 for heater pid
   TCCR1A = 0;
   TCCR1B = 0;
   TCNT1 = 0;

   TCCR1B |= (1 << WGM12); // CTC mode
   TCCR1B |= (1 << CS12); // 256 prescaler
   TIMSK1 |= (1 << OCIE1A); // enable timer compare interrupt
   
   OCR1A = DEFAULT_HEATER_TIMER_COUNT - 1; // compare match register 16MHz/256/2Hz
   #endif
}

void init_motor_timer(void){
   #ifdef SILABS
   CMU_ClockEnable(MOTOR_TIMER_CLOCK, true);
	TIMER_TopSet(MOTOR_TIMER, MOTOR_TIMER_TOP);
	TIMER_Init_TypeDef motor_timer_init = TIMER_INIT_DEFAULT;
	TIMER_IntEnable(MOTOR_TIMER, TIMER_IF_OF);
	NVIC_EnableIRQ(MOTOR_TIMER_IRQ_NUMBER);
	TIMER_Init(MOTOR_TIMER, &motor_timer_init);
   #else
   // initialize Timer1 for motors
   TCCR3A = 0;
   TCCR3B = 0;
   TCNT3 = 0;

   TCCR3B |= (1 << WGM12); // CTC mode
   TCCR3B |= (1 << CS10); // 1 prescaler
   TIMSK3 |= (1 << OCIE3A); // enable timer compare interrupt
   
   OCR3A = DEFAULT_TIMER_COUNT - 1; // compare match register 16MHz/256/2Hz
   #endif
}

ISR(TIMER1_COMPA_vect){ // timer compare interrupt service routine
   // set next overflow count value
   OCR1A = DEFAULT_HEATER_TIMER_COUNT - 1; // compare match register 16MHz/256/2Hz
   // clear current count to reset
   TCNT1 = 0;
   cnc_heaters.heater_irq = 1;
}
