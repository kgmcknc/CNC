/*
 * cnc_heaters.h
 *
 *  Created on: Sep 13, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_HEATERS_H_
#define SRC_CNC_HEATERS_H_

#include "stdint.h"

struct cnc_heater_struct {
	uint8_t enabled;
	uint8_t active;
	uint8_t heater_on;
	uint8_t wait_for_temp; // two different command behaviors... finish instruction immediate or wait until target is met
	uint8_t fan_on;
	uint8_t temp_locked;
	uint8_t reset_heater;
	double target_temp;
	double current_temp;
};

struct cnc_heater_list_struct {
	struct cnc_heater_struct heater_0;
	struct cnc_heater_struct heater_1;
	struct cnc_heater_struct heater_2;
	struct cnc_heater_struct heater_3;
};

void handle_heaters(struct cnc_heater_list_struct* heaters);
void set_heaters(struct cnc_state_struct* cnc);
void set_heater_active(struct cnc_heater_instruction_struct* heater_instruction, struct cnc_heater_struct* heater);
void init_heaters(struct cnc_heater_list_struct* heaters);
void init_heater(struct cnc_heater_struct* heater);
void enable_heater_fans(void);
void disable_heater_fans(void);

#endif /* SRC_CNC_HEATERS_H_ */
