/*
 * cnc_pid.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#include "cnc_pid.h"
#include "rtcdriver.h"
#include "cnc_adc.h"

int32_t p_error[NUM_ADCS];
int32_t i_error[NUM_ADCS];
int32_t d_error[NUM_ADCS];

void reset_pid(void){

}

void process_pid(RTCDRV_TimerID_t id, void *user){
	update_pid = 0;
}

