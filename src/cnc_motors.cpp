/*
 * cnc_motors.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#include "Arduino.h"
#include "cnc_motors.h"
#include "cnc_timers.h"
#include "cnc_functions.h"
#include "math.h"

void init_motors(struct cnc_motor_list_struct* motors){
   #ifdef SILABS
	motors->aux.ports.dir_port    = AUX_MOTOR_DIR_PORT;
	motors->aux.ports.en_port     = AUX_MOTOR_EN_PORT;
	motors->aux.ports.ms0_port    = AUX_MOTOR_MS0_PORT;
	motors->aux.ports.ms1_port    = AUX_MOTOR_MS1_PORT;
	motors->aux.ports.max_es_port = (GPIO_Port_TypeDef) 0;
	motors->aux.ports.min_es_port = (GPIO_Port_TypeDef) 0;
	motors->aux.ports.step_port   = AUX_MOTOR_STEP_PORT;
   #endif
	motors->aux.pins.dir_pin     = AUX_MOTOR_DIR_PIN;
	motors->aux.pins.en_pin      = AUX_MOTOR_EN_PIN;
	motors->aux.pins.ms0_pin     = AUX_MOTOR_MS0_PIN;
	motors->aux.pins.ms1_pin     = AUX_MOTOR_MS1_PIN;
	motors->aux.pins.max_es_pin  = 0;
	motors->aux.pins.min_es_pin  = 0;
	motors->aux.pins.step_pin    = AUX_MOTOR_STEP_PIN;

   #ifdef SILABS
	motors->extruder_0.ports.dir_port    = EXTRUDER_0_MOTOR_DIR_PORT;
	motors->extruder_0.ports.en_port     = EXTRUDER_0_MOTOR_EN_PORT;
	motors->extruder_0.ports.ms0_port    = EXTRUDER_0_MOTOR_MS0_PORT;
	motors->extruder_0.ports.ms1_port    = EXTRUDER_0_MOTOR_MS1_PORT;
	motors->extruder_0.ports.max_es_port = (GPIO_Port_TypeDef) 0;
	motors->extruder_0.ports.min_es_port = (GPIO_Port_TypeDef) 0;
	motors->extruder_0.ports.step_port   = EXTRUDER_0_MOTOR_STEP_PORT;
   #endif
	motors->extruder_0.pins.dir_pin     = EXTRUDER_0_MOTOR_DIR_PIN;
	motors->extruder_0.pins.en_pin      = EXTRUDER_0_MOTOR_EN_PIN;
	motors->extruder_0.pins.ms0_pin     = EXTRUDER_0_MOTOR_MS0_PIN;
	motors->extruder_0.pins.ms1_pin     = EXTRUDER_0_MOTOR_MS1_PIN;
	motors->extruder_0.pins.max_es_pin  = 0;
	motors->extruder_0.pins.min_es_pin  = 0;
	motors->extruder_0.pins.step_pin    = EXTRUDER_0_MOTOR_STEP_PIN;

   #ifdef SILABS
	motors->extruder_1.ports.dir_port    = EXTRUDER_1_MOTOR_DIR_PORT;
	motors->extruder_1.ports.en_port     = EXTRUDER_1_MOTOR_EN_PORT;
	motors->extruder_1.ports.ms0_port    = EXTRUDER_1_MOTOR_MS0_PORT;
	motors->extruder_1.ports.ms1_port    = EXTRUDER_1_MOTOR_MS1_PORT;
	motors->extruder_1.ports.max_es_port = (GPIO_Port_TypeDef) 0;
	motors->extruder_1.ports.min_es_port = (GPIO_Port_TypeDef) 0;
	motors->extruder_1.ports.step_port   = EXTRUDER_1_MOTOR_STEP_PORT;
   #endif
	motors->extruder_1.pins.dir_pin     = EXTRUDER_1_MOTOR_DIR_PIN;
	motors->extruder_1.pins.en_pin      = EXTRUDER_1_MOTOR_EN_PIN;
	motors->extruder_1.pins.ms0_pin     = EXTRUDER_1_MOTOR_MS0_PIN;
	motors->extruder_1.pins.ms1_pin     = EXTRUDER_1_MOTOR_MS1_PIN;
	motors->extruder_1.pins.max_es_pin  = 0;
	motors->extruder_1.pins.min_es_pin  = 0;
	motors->extruder_1.pins.step_pin    = EXTRUDER_1_MOTOR_STEP_PIN;

   #ifdef SILABS
	motors->xl_axis.ports.dir_port    = XL_AXIS_MOTOR_DIR_PORT;
	motors->xl_axis.ports.en_port     = XL_AXIS_MOTOR_EN_PORT;
	motors->xl_axis.ports.ms0_port    = XL_AXIS_MOTOR_MS0_PORT;
	motors->xl_axis.ports.ms1_port    = XL_AXIS_MOTOR_MS1_PORT;
	motors->xl_axis.ports.max_es_port = ENDSTOP_R_PORT;
	motors->xl_axis.ports.min_es_port = ENDSTOP_L_PORT;
	motors->xl_axis.ports.step_port   = XL_AXIS_MOTOR_STEP_PORT;
   #endif
	motors->xl_axis.pins.dir_pin     = XL_AXIS_MOTOR_DIR_PIN;
	motors->xl_axis.pins.en_pin      = XL_AXIS_MOTOR_EN_PIN;
	motors->xl_axis.pins.ms0_pin     = XL_AXIS_MOTOR_MS0_PIN;
	motors->xl_axis.pins.ms1_pin     = XL_AXIS_MOTOR_MS1_PIN;
	motors->xl_axis.pins.max_es_pin  = ENDSTOP_R_PIN;
	motors->xl_axis.pins.min_es_pin  = ENDSTOP_L_PIN;
	motors->xl_axis.pins.step_pin    = XL_AXIS_MOTOR_STEP_PIN;

   #ifdef SILABS
	motors->yf_axis.ports.dir_port    = YF_AXIS_MOTOR_DIR_PORT;
	motors->yf_axis.ports.en_port     = YF_AXIS_MOTOR_EN_PORT;
	motors->yf_axis.ports.ms0_port    = YF_AXIS_MOTOR_MS0_PORT;
	motors->yf_axis.ports.ms1_port    = YF_AXIS_MOTOR_MS1_PORT;
	motors->yf_axis.ports.max_es_port = ENDSTOP_B_PORT;
	motors->yf_axis.ports.min_es_port = ENDSTOP_F_PORT;
	motors->yf_axis.ports.step_port   = YF_AXIS_MOTOR_STEP_PORT;
   #endif
	motors->yf_axis.pins.dir_pin     = YF_AXIS_MOTOR_DIR_PIN;
	motors->yf_axis.pins.en_pin      = YF_AXIS_MOTOR_EN_PIN;
	motors->yf_axis.pins.ms0_pin     = YF_AXIS_MOTOR_MS0_PIN;
	motors->yf_axis.pins.ms1_pin     = YF_AXIS_MOTOR_MS1_PIN;
	motors->yf_axis.pins.max_es_pin  = ENDSTOP_B_PIN;
	motors->yf_axis.pins.min_es_pin  = ENDSTOP_F_PIN;
	motors->yf_axis.pins.step_pin    = YF_AXIS_MOTOR_STEP_PIN;

   #ifdef SILABS
	motors->zl_axis.ports.dir_port    = ZL_AXIS_MOTOR_DIR_PORT;
	motors->zl_axis.ports.en_port     = ZL_AXIS_MOTOR_EN_PORT;
	motors->zl_axis.ports.ms0_port    = ZL_AXIS_MOTOR_MS0_PORT;
	motors->zl_axis.ports.ms1_port    = ZL_AXIS_MOTOR_MS1_PORT;
	motors->zl_axis.ports.max_es_port = ENDSTOP_TL_PORT;
	motors->zl_axis.ports.min_es_port = ENDSTOP_BL_PORT;
	motors->zl_axis.ports.step_port   = ZL_AXIS_MOTOR_STEP_PORT;
   #endif
	motors->zl_axis.pins.dir_pin     = ZL_AXIS_MOTOR_DIR_PIN;
	motors->zl_axis.pins.en_pin      = ZL_AXIS_MOTOR_EN_PIN;
	motors->zl_axis.pins.ms0_pin     = ZL_AXIS_MOTOR_MS0_PIN;
	motors->zl_axis.pins.ms1_pin     = ZL_AXIS_MOTOR_MS1_PIN;
	motors->zl_axis.pins.max_es_pin  = ENDSTOP_TL_PIN;
	motors->zl_axis.pins.min_es_pin  = ENDSTOP_BL_PIN;
	motors->zl_axis.pins.step_pin    = ZL_AXIS_MOTOR_STEP_PIN;

   #ifdef SILABS
	motors->zr_axis.ports.dir_port    = ZR_AXIS_MOTOR_DIR_PORT;
	motors->zr_axis.ports.en_port     = ZR_AXIS_MOTOR_EN_PORT;
	motors->zr_axis.ports.ms0_port    = ZR_AXIS_MOTOR_MS0_PORT;
	motors->zr_axis.ports.ms1_port    = ZR_AXIS_MOTOR_MS1_PORT;
	motors->zr_axis.ports.max_es_port = ENDSTOP_TR_PORT;
	motors->zr_axis.ports.min_es_port = ENDSTOP_BR_PORT;
	motors->zr_axis.ports.step_port   = ZR_AXIS_MOTOR_STEP_PORT;
   #endif
	motors->zr_axis.pins.dir_pin     = ZR_AXIS_MOTOR_DIR_PIN;
	motors->zr_axis.pins.en_pin      = ZR_AXIS_MOTOR_EN_PIN;
	motors->zr_axis.pins.ms0_pin     = ZR_AXIS_MOTOR_MS0_PIN;
	motors->zr_axis.pins.ms1_pin     = ZR_AXIS_MOTOR_MS1_PIN;
	motors->zr_axis.pins.max_es_pin  = ENDSTOP_TR_PIN;
	motors->zr_axis.pins.min_es_pin  = ENDSTOP_BR_PIN;
	motors->zr_axis.pins.step_pin    = ZR_AXIS_MOTOR_STEP_PIN;

	init_motor(&motors->aux, "Aux");
	init_motor(&motors->extruder_0, "Extd-0");
	init_motor(&motors->extruder_1, "Extd-1");
	init_motor(&motors->xl_axis, "X-LR");
	init_motor(&motors->yf_axis, "Y-FB");
	init_motor(&motors->zl_axis, "ZL-UD");
	init_motor(&motors->zr_axis, "ZR-UD");
}

void init_motor(cnc_motor_struct* motor, const char* name){
	disable_motor(motor);
	motor->direction = MOTOR_STILL;
	motor->axis_length = 0;
	motor->max_range_flag = 0;
	motor->min_range_flag = 0;
	motor->step_high = 0;
	motor->position = 10000;
	motor->home_position = 0;
	motor->safe_position = 0;
	motor->move_count = 0;
	motor->ramp_count = 0;
	motor->set = 0;
	motor->homed = 0;
	check_endstop(motor);
	motor->period = 10000; // HIGH VALUE AT FIRST AS DEFAULT
	motor->current_period = 10000;
	motor->step_timer = 0;
	motor->find_zero = 0;
	motor->find_max = 0;
	strcpy(motor->name, name);
}

void enable_motor(cnc_motor_struct* motor){
   cnc_gpio_write(motor->pins.en_pin, motor->ports.en_port, MOTOR_ENABLED);
	motor->enabled = 1;
}

void disable_motor(cnc_motor_struct* motor){
	cnc_gpio_write(motor->pins.en_pin, motor->ports.en_port, MOTOR_DISABLED);
	motor->enabled = 0;
}

void set_motor_direction(cnc_motor_struct* motor, int8_t direction){
	switch(direction){
		case MOTOR_STILL : {
         cnc_gpio_write(motor->pins.dir_pin, motor->ports.dir_port, 0);
			motor->direction = direction;
			break;
		}
		case MOTOR_MOVE_INCREASE : {
         cnc_gpio_write(motor->pins.dir_pin, motor->ports.dir_port, 1);
			motor->direction = direction;
			break;
		}
		case MOTOR_MOVE_DECREASE : {
         cnc_gpio_write(motor->pins.dir_pin, motor->ports.dir_port, 0);
			motor->direction = direction;
			break;
		}
		default : {
			cnc_gpio_write(motor->pins.dir_pin, motor->ports.dir_port, MOTOR_STILL);
			motor->direction = MOTOR_STILL;
		}
	}
}

void enable_axis_motors(struct cnc_motor_list_struct* motors){
	enable_motor(&motors->xl_axis);
	enable_motor(&motors->yf_axis);
	enable_motor(&motors->zl_axis);
	enable_motor(&motors->zr_axis);
}

void disable_axis_motors(struct cnc_motor_list_struct* motors){
	disable_motor(&motors->xl_axis);
	disable_motor(&motors->yf_axis);
	disable_motor(&motors->zl_axis);
	disable_motor(&motors->zr_axis);
}

void handle_motors(struct cnc_state_struct* cnc){
	if(motor_irq){
		motor_irq = 0;
		process_motors(cnc->motors);
		check_endstops(cnc->motors);
		check_directions(cnc, cnc->motors);
		check_periods(cnc, cnc->motors);
	}
}

void check_endstop(struct cnc_motor_struct* motor){
	//motors->xl_axis.max_range_flag;
   if(cnc_gpio_read(motor->pins.min_es_pin, motor->ports.min_es_port)){
		// switch not hit
		motor->min_range_flag = ENDSTOP_NOT_HIT;
	} else {
		// switch hit or no power to switch
		motor->min_range_flag = ENDSTOP_HIT_OR_NO_POWER;
	}
	if(cnc_gpio_read(motor->pins.max_es_pin, motor->ports.max_es_port)){
		// switch not hit
		motor->max_range_flag = ENDSTOP_NOT_HIT;
	} else {
		// switch hit or no power to switch
		motor->max_range_flag = ENDSTOP_HIT_OR_NO_POWER;
	}
}

void check_endstops(struct cnc_motor_list_struct* motors){
	check_endstop(&motors->xl_axis);
	check_endstop(&motors->yf_axis);
	check_endstop(&motors->zl_axis);
	check_endstop(&motors->zr_axis);
	motors->extruder_0.min_range_flag = ENDSTOP_NOT_HIT;
	motors->extruder_0.max_range_flag = ENDSTOP_NOT_HIT;
	motors->extruder_1.min_range_flag = ENDSTOP_NOT_HIT;
	motors->extruder_1.max_range_flag = ENDSTOP_NOT_HIT;
	motors->aux.min_range_flag = ENDSTOP_NOT_HIT;
	motors->aux.max_range_flag = ENDSTOP_NOT_HIT;
}

void handle_step(struct cnc_motor_struct* motor){
	if(motor->step_high){
		// clear step pulse to get rising edge later
		step_motor_low(motor);
		motor->step_high = 0;
	}
	if(motor->enabled && motor->move_count){
		if(motor->step_timer > 0){
			// decrease step timer until 0
			// moved to interrupt
			motor->step_timer--;
		} else {
			// if timer has reached 0 and motor set
			if(motor->set){
				// if you haven't reached the endstop in direction you're moving
				if(((motor->direction == MOTOR_MOVE_INCREASE) && (!motor->max_range_flag)) ||
						((motor->direction == MOTOR_MOVE_DECREASE) && (!motor->min_range_flag))){
					// step motor, adjust position, set flag to clear rising edge next loop
					step_motor_high(motor);
					motor->position = motor->position + motor->direction;
					motor->move_count = motor->move_count - motor->direction;
					motor->step_high = 1;
				}
				// clear set flag to reload period
				motor->set = 0;
			}
		}
	}
}

void process_motors(struct cnc_motor_list_struct* motors){
	handle_step(&motors->aux);
	handle_step(&motors->extruder_0);
	handle_step(&motors->extruder_1);
	handle_step(&motors->xl_axis);
	handle_step(&motors->yf_axis);
	handle_step(&motors->zl_axis);
	handle_step(&motors->zr_axis);
}

void step_motor_high(struct cnc_motor_struct* motor){
   cnc_gpio_write(motor->pins.step_pin, motor->ports.step_port, 1);
}

void step_motor_low(struct cnc_motor_struct* motor){
   cnc_gpio_write(motor->pins.step_pin, motor->ports.step_port, 0);
}

void check_directions(struct cnc_state_struct* cnc, struct cnc_motor_list_struct* motors){
	check_direction(cnc, &motors->aux);
	check_direction(cnc, &motors->extruder_0);
	check_direction(cnc, &motors->extruder_1);
	check_direction(cnc, &motors->xl_axis);
	check_direction(cnc, &motors->yf_axis);
	check_direction(cnc, &motors->zl_axis);
	check_direction(cnc, &motors->zr_axis);
}

void check_periods(struct cnc_state_struct* cnc, struct cnc_motor_list_struct* motors){
	check_period(cnc, &motors->aux);
	check_period(cnc, &motors->extruder_0);
	check_period(cnc, &motors->extruder_1);
	check_period(cnc, &motors->xl_axis);
	check_period(cnc, &motors->yf_axis);
	check_period(cnc, &motors->zl_axis);
	check_period(cnc, &motors->zr_axis);
}

void check_direction(struct cnc_state_struct* cnc, struct cnc_motor_struct* motor){
	if(!motor->set && motor->move_count){
		if(motor->max_range_flag){
			if(motor->direction == MOTOR_MOVE_INCREASE){
				set_motor_direction(motor, MOTOR_STILL);
				motor->move_count = 0;
				step_motor_low(motor);
				motor->step_high = 0;
				cnc_printf(cnc,"%s: Max Range Hit", motor->name);
			}
		}
		if(motor->min_range_flag){
			if(motor->direction == MOTOR_MOVE_DECREASE){
				set_motor_direction(motor, MOTOR_STILL);
				motor->move_count = 0;
				step_motor_low(motor);
				motor->step_high = 0;
				cnc_printf(cnc,"%s: Min Range Hit", motor->name);
			}
		}
	}
}

void check_period(struct cnc_state_struct* cnc, struct cnc_motor_struct* motor){
	// here is where I would check ramping up and down, arc moves and anything else...
	if(!motor->set && motor->move_count){
		// adjust speed for ramping
		motor->current_period = motor->period + motor->ramp_count;
		if(abs(motor->move_count) > (cnc->config.ramp_period - motor->ramp_count)){
			// ramping up to faster speed
			if(motor->ramp_count > 0){
				motor->ramp_count = motor->ramp_count - 1;
			}
		} else {
			// ramping down to slower speed
			if(motor->ramp_count < cnc->config.ramp_period){
				motor->ramp_count = motor->ramp_count + 1;
			}
		}

		// adjust speed for arc moves
		// bias for arc here somehow...

		motor->step_timer = motor->current_period;
		//motor->step_timer = motor->period;
		motor->set = 1;
	}
}

#ifdef SILABS

#else
   
#endif
