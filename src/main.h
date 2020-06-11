/*
 * main.h
 *
 *  Created on: Aug 21, 2018
 *      Author: Kyle
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include "stdint.h"
#include "../common/src/revision.h"
#include "cnc_functions.h"
#include "cnc_motors.h"
#include "cnc_heaters.h"

void system_init(void);
void variable_init(void);
void init_config(cnc_config_struct* config);
void init_clocks(void);

extern struct cnc_state_struct cnc;
extern struct cnc_heater_list_struct cnc_heaters;
extern struct cnc_motor_list_struct cnc_motors;
extern struct cnc_endstop_list_struct cnc_endstops;

#endif /* SRC_MAIN_H_ */
