/*
 * cnc_heaters.h
 *
 *  Created on: Sep 13, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_HEATERS_H_
#define SRC_CNC_HEATERS_H_

#include "stdint.h"
#include "common_cnc.h"

struct cnc_heater_struct {
	uint8_t enabled;
	uint8_t heater_on;
	uint8_t wait_for_temp; // two different command behaviors... finish instruction immediate or wait until target is met
	uint8_t fan_on;
	uint8_t fan_duty;
	uint8_t temp_locked;
	uint8_t reset_heater;
	cnc_double target_temp;
	cnc_double current_temp;
};

struct cnc_heater_list_struct {
   uint8_t heater_irq;
	struct cnc_heater_struct heater[NUM_HEATERS];
};

void handle_heaters(struct cnc_state_struct* cnc);
void init_heaters(struct cnc_heater_list_struct* heaters);
void init_heater(struct cnc_heater_struct* heater);
void enable_heater_fans(void);
void disable_heater_fans(void);

#endif /* SRC_CNC_HEATERS_H_ */
