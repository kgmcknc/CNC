/*
 * cnc_pid.h
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_PID_H_
#define SRC_CNC_PID_H_

#include "stdint.h"

extern uint8_t update_pid;

struct cnc_heater_instruction_struct {
	uint8_t enabled;
	uint8_t valid_instruction;
	uint8_t heater_on;
	uint8_t fan_on;
	uint8_t temp_locked;
	double target_temp;
	double current_temp;
};

#endif /* SRC_CNC_PID_H_ */
