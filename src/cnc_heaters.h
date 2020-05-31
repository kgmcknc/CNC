/*
 * cnc_heaters.h
 *
 *  Created on: Sep 13, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_HEATERS_H_
#define SRC_CNC_HEATERS_H_

#include "stdint.h"
#include "cnc_gpio.h"
#include "common_cnc.h"

#define UPDATE_INTERVAL 200
#define AVERAGE_COUNT 32
#define WINDUP_COUNT 32

struct cnc_heater_struct {
	uint8_t heater_active;
   uint8_t fan_active;
	uint8_t heater_on;
	uint8_t wait_for_temp; // two different command behaviors... finish instruction immediate or wait until target is met
	uint8_t fan_on;
	uint8_t fan_duty;
	uint8_t temp_locked;
	uint8_t reset_heater;
   GPIO_Port_TypeDef heater_port;
	uint8_t heater_pin;
   GPIO_Port_TypeDef fan_port;
	uint8_t fan_pin;
   GPIO_Port_TypeDef adc_pinport;
   uint8_t adc_pinbit;
	uint8_t adc_pin;
   int16_t adc_value;
   int16_t target_adc;
   uint8_t cooldown_active;
   uint8_t pid_reset;
   int16_t pid_average[AVERAGE_COUNT];
   uint8_t average_pointer;
   int16_t last_p_error;
   int16_t last_adj;
   int16_t i_error;
   uint8_t anti_windup;
   uint8_t windup_rounds;
};

struct cnc_heater_list_struct {
   uint8_t heater_irq;
   uint16_t update_counter;
	struct cnc_heater_struct heater[NUM_HEATERS];
};

void handle_heaters(struct cnc_state_struct* cnc);
void init_heaters(struct cnc_heater_list_struct* heaters);
void init_heater(struct cnc_heater_struct* heater);
void read_heater_adc(struct cnc_heater_struct* heater);
void reset_heater_pid(struct cnc_heater_struct* heater);
void update_heater_pid(struct cnc_heater_struct* heater);

#endif /* SRC_CNC_HEATERS_H_ */
