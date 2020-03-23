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


void system_init(struct cnc_state_struct* cnc);
void variable_init(struct cnc_state_struct* cnc);
void init_config(cnc_config_struct* config);
void init_clocks(void);

#endif /* SRC_MAIN_H_ */
