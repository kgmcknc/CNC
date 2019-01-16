/*
 * cnc_heaters.cpp
 *
 *  Created on: Sep 13, 2018
 *      Author: Kyle
 */

#include "stdint.h"
#include "cnc_heaters.h"
#include "cnc_functions.h"
#include "cnc_pid.h"
#include "cnc_adc.h"
#include "cnc_gpio.h"
#include "math.h"

#define IN_VOLT 3.3
#define BETA_VALUE 3950.0
#define BASE_RESISTANCE 100000.0
#define KELVIN_CONV 273.15
#define BASE_TEMP 25.0 // degrees celsius
#define ADC_MAX ((1<<12)-1)
#define FAN_OFF 0
#define FAN_ON 1
#define UPDATE_INTERVAL 200
#define AVERAGE_COUNT 64

uint16_t adc_av[NUM_ADCS][AVERAGE_COUNT] = {0};
uint16_t av_pnt[NUM_ADCS] = {0};
uint32_t print_count = 0;
uint32_t update_count = 0;

void handle_heaters(cnc_heater_list_struct* heaters){
	uint8_t heater_count = 0;
	uint32_t current_average[NUM_ADCS];
	double current_temp[NUM_ADCS];
	double current_voltage[NUM_ADCS];
	double current_resistance[NUM_ADCS];
	double log_val;
	uint16_t av_cnt;

	if(update_pid){
		update_pid = 0;
		for(heater_count=0;heater_count<NUM_ADCS;heater_count++){
			adc_av[heater_count][av_pnt[heater_count]] = adc_channel_data[heater_count];
			if(av_pnt[heater_count]<(AVERAGE_COUNT-1)){
				av_pnt[heater_count] += 1;
			} else {
				av_pnt[heater_count] = 0;
			}
		}
		if(update_count<UPDATE_INTERVAL){
			update_count++;
		} else {
			update_count = 0;
			for(heater_count=0;heater_count<NUM_ADCS;heater_count++){
				current_average[heater_count] = 0;
				for(av_cnt=0;av_cnt<AVERAGE_COUNT;av_cnt++){
					current_average[heater_count] = current_average[heater_count] + adc_av[heater_count][av_cnt];
				}
				current_average[heater_count] = current_average[heater_count] / AVERAGE_COUNT;
				adc_channel_valid[heater_count] = 0;
				current_voltage[heater_count] = ((double) IN_VOLT*adc_channel_data[heater_count])/ADC_MAX;
				current_resistance[heater_count] = (((double) IN_VOLT*BASE_RESISTANCE)/current_voltage[heater_count])-BASE_RESISTANCE;
				if(current_resistance[heater_count] > 0){
					log_val = (double) log(BASE_RESISTANCE/current_resistance[heater_count]);
					current_temp[heater_count] = ((double) (log_val*(KELVIN_CONV*(BASE_TEMP+KELVIN_CONV)))+(BASE_TEMP*BETA_VALUE))/(BETA_VALUE-(log_val*(BASE_TEMP+KELVIN_CONV)));
					if(!print_count && !heater_count){
						//cnc_printf(&cnc,"volt: %f, res: %f, log: %f", current_voltage[heater_count], current_resistance[heater_count], log_val);
						//cnc_printf(&cnc,"ADC: %ld, Temp: %f C, %f F", adc_channel_data[heater_count], current_temp[heater_count], (((current_temp[heater_count]*9)/5)+32));
					}
				}
			}
			if(print_count < 50){
					print_count++;
			} else {
				print_count = 0;
			}
		}
	}
	if(heaters->heater_0.enabled){
		if(heaters->heater_0.fan_on == FAN_OFF){
			// turn on fan
		}
	} else {
		if(heaters->heater_0.fan_on == FAN_ON){
			// turn off fan
		}
	}
}

void set_heaters(struct cnc_state_struct* cnc){
	set_heater_active(&cnc->current_instruction.heater_0, &cnc->heaters->heater_0);
	set_heater_active(&cnc->current_instruction.heater_1, &cnc->heaters->heater_1);
	set_heater_active(&cnc->current_instruction.heater_2, &cnc->heaters->heater_2);
	set_heater_active(&cnc->current_instruction.heater_3, &cnc->heaters->heater_3);
}

void set_heater_active(struct cnc_heater_instruction_struct* heater_instruction, struct cnc_heater_struct* heater){
	if(heater_instruction->valid_instruction && !heater->active){
		if(heater_instruction->target_temp == heater->current_temp){
			// moving to same place... just happened to be there already...
			heater_instruction->valid_instruction = 0;
			heater->active = 1;
		} else {
			heater->active = 1;
			heater->target_temp = heater_instruction->target_temp;
			heater->reset_heater = 1;
		}
	}
}

void init_heaters(cnc_heater_list_struct* heaters){

}

void init_heater(cnc_heater_struct* heater){

}

void enable_heater_fans(void){
	GPIO_PinOutSet(FANS_PORT, FANS_PIN);
}

void disable_heater_fans(void){
	GPIO_PinOutClear(FANS_PORT, FANS_PIN);
}
