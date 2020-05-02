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

#define IN_VOLT ((cnc_double) 3.3)
#define BETA_VALUE ((cnc_double) 3950.0)
#define BASE_RESISTANCE ((cnc_double) 22000.0)
#define THERMISTOR_RESISTANCE ((cnc_double) 100000.0)
#define KELVIN_CONV ((cnc_double) 273.15)
#define BASE_TEMP ((cnc_double) 25.0) // degrees celsius
#define BASE_TEMP_KELVIN (BASE_TEMP + KELVIN_CONV)
#define ADC_MAX ((cnc_double) ((1<<10)-1))

#define SHUTOFF_TEMP_C ((cnc_double) 38)
#define SHUTOFF_BETA ((BETA_VALUE/(SHUTOFF_TEMP_C+KELVIN_CONV))-(BETA_VALUE/BASE_TEMP_KELVIN))
#define SHUTOFF_RESISTANCE (exp(SHUTOFF_BETA)*THERMISTOR_RESISTANCE)
#define SHUTOFF_ADC (uint32_t) ((ADC_MAX*BASE_RESISTANCE)/(BASE_RESISTANCE+SHUTOFF_RESISTANCE))

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
   GPIO_Port_TypeDef adc_port;
	uint8_t adc_pin;
   uint32_t adc_value;
   uint32_t target_adc;
   uint8_t cooldown_active;
   uint8_t pid_reset;
   int16_t pid_average[AVERAGE_COUNT];
   uint8_t average_pointer;
   int16_t last_p_error;
   cnc_double last_adj;
   int64_t i_error;
   int8_t d_sign;
   uint8_t anti_windup;
   uint16_t windup_rounds;
   int16_t adj_count;
	cnc_double target_temp;
	cnc_double current_temp;
};

struct cnc_heater_list_struct {
   uint8_t heater_irq;
   uint16_t update_counter;
	struct cnc_heater_struct heater[NUM_HEATERS];
};

void handle_heaters(struct cnc_state_struct* cnc);
void init_heaters(struct cnc_heater_list_struct* heaters);
void init_heater(struct cnc_heater_struct* heater);
void enable_heater_fans(void);
void disable_heater_fans(void);
void read_heater_adc(struct cnc_heater_struct* heater);
void reset_heater_pid(struct cnc_heater_struct* heater);
void update_heater_pid(struct cnc_heater_struct* heater);

#endif /* SRC_CNC_HEATERS_H_ */
