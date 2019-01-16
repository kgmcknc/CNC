/*
 * cnc_timers.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */


#include "cnc_timers.h"
#include "cnc_motors.h"
#include "cnc_pid.h"
#include "em_timer.h"
#include "em_cmu.h"
#include "cnc_motors.h"
#include <stddef.h>

RTCDRV_TimerID_t motor_update_timer;
RTCDRV_TimerID_t pid_update_timer;

/***************************************************************************//**
 * Local prototypes
 ******************************************************************************/

void init_timers(cnc_motor_list_struct* cnc_motors){
	init_motor_timer();
	//init_pid_timer();
}

void init_motor_timer(void){
	CMU_ClockEnable(MOTOR_TIMER_CLOCK, true);
	TIMER_TopSet(MOTOR_TIMER, MOTOR_TIMER_TOP);
	TIMER_Init_TypeDef motor_timer_init = TIMER_INIT_DEFAULT;
	TIMER_IntEnable(MOTOR_TIMER, TIMER_IF_OF);
	NVIC_EnableIRQ(MOTOR_TIMER_IRQ_NUMBER);
	TIMER_Init(MOTOR_TIMER, &motor_timer_init);
}

/*void init_pid_timer(void){
	CMU_ClockEnable(PID_TIMER_CLOCK, true);
	TIMER_TopSet(PID_TIMER, PID_TIMER_TOP);
	TIMER_Init_TypeDef pid_timer_init = TIMER_INIT_DEFAULT;
	TIMER_IntEnable(PID_TIMER, TIMER_IF_OF);
	NVIC_EnableIRQ(PID_TIMER_IRQ_NUMBER);
	TIMER_Init(PID_TIMER, &pid_timer_init);
}*/


/*void TIMER0_IRQHandler(void){
	TIMER_IntClear(MOTOR_TIMER, TIMER_IF_OF);
	update_motors = 1;
	// code
}*/

void TIMER1_IRQHandler(void){
	TIMER_IntClear(MOTOR_TIMER, TIMER_IF_OF);
	update_motors = 1;
	// code
}

/*void TIMER1_IRQHandler(void){
	TIMER_IntClear(PID_TIMER, TIMER_IF_OF);
	//update_pid = 1;
}*/
