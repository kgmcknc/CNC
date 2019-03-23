/*
 * cnc_motors.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#include "cnc_motors.h"
#include "cnc_timers.h"
#include "cnc_functions.h"

uint8_t update_motors = 0;
uint8_t update_pid = 0;

void init_motors(struct cnc_motor_list_struct* motors){
	motors->aux.ports.dir_port    = AUX_MOTOR_DIR_PORT;
	motors->aux.pins.dir_pin     = AUX_MOTOR_DIR_PIN;
	motors->aux.ports.en_port     = AUX_MOTOR_EN_PORT;
	motors->aux.pins.en_pin      = AUX_MOTOR_EN_PIN;
	motors->aux.ports.ms0_port    = AUX_MOTOR_MS0_PORT;
	motors->aux.pins.ms0_pin     = AUX_MOTOR_MS0_PIN;
	motors->aux.ports.ms1_port    = AUX_MOTOR_MS1_PORT;
	motors->aux.pins.ms1_pin     = AUX_MOTOR_MS1_PIN;
	motors->aux.ports.max_es_port = (GPIO_Port_TypeDef) 0;
	motors->aux.pins.max_es_pin  = 0;
	motors->aux.ports.min_es_port = (GPIO_Port_TypeDef) 0;
	motors->aux.pins.min_es_pin  = 0;
	motors->aux.ports.step_port   = AUX_MOTOR_STEP_PORT;
	motors->aux.pins.step_pin    = AUX_MOTOR_STEP_PIN;

	motors->extruder_0.ports.dir_port    = EXTRUDER_0_MOTOR_DIR_PORT;
	motors->extruder_0.pins.dir_pin     = EXTRUDER_0_MOTOR_DIR_PIN;
	motors->extruder_0.ports.en_port     = EXTRUDER_0_MOTOR_EN_PORT;
	motors->extruder_0.pins.en_pin      = EXTRUDER_0_MOTOR_EN_PIN;
	motors->extruder_0.ports.ms0_port    = EXTRUDER_0_MOTOR_MS0_PORT;
	motors->extruder_0.pins.ms0_pin     = EXTRUDER_0_MOTOR_MS0_PIN;
	motors->extruder_0.ports.ms1_port    = EXTRUDER_0_MOTOR_MS1_PORT;
	motors->extruder_0.pins.ms1_pin     = EXTRUDER_0_MOTOR_MS1_PIN;
	motors->extruder_0.ports.max_es_port = (GPIO_Port_TypeDef) 0;
	motors->extruder_0.pins.max_es_pin  = 0;
	motors->extruder_0.ports.min_es_port = (GPIO_Port_TypeDef) 0;
	motors->extruder_0.pins.min_es_pin  = 0;
	motors->extruder_0.ports.step_port   = EXTRUDER_0_MOTOR_STEP_PORT;
	motors->extruder_0.pins.step_pin    = EXTRUDER_0_MOTOR_STEP_PIN;

	motors->extruder_1.ports.dir_port    = EXTRUDER_1_MOTOR_DIR_PORT;
	motors->extruder_1.pins.dir_pin     = EXTRUDER_1_MOTOR_DIR_PIN;
	motors->extruder_1.ports.en_port     = EXTRUDER_1_MOTOR_EN_PORT;
	motors->extruder_1.pins.en_pin      = EXTRUDER_1_MOTOR_EN_PIN;
	motors->extruder_1.ports.ms0_port    = EXTRUDER_1_MOTOR_MS0_PORT;
	motors->extruder_1.pins.ms0_pin     = EXTRUDER_1_MOTOR_MS0_PIN;
	motors->extruder_1.ports.ms1_port    = EXTRUDER_1_MOTOR_MS1_PORT;
	motors->extruder_1.pins.ms1_pin     = EXTRUDER_1_MOTOR_MS1_PIN;
	motors->extruder_1.ports.max_es_port = (GPIO_Port_TypeDef) 0;
	motors->extruder_1.pins.max_es_pin  = 0;
	motors->extruder_1.ports.min_es_port = (GPIO_Port_TypeDef) 0;
	motors->extruder_1.pins.min_es_pin  = 0;
	motors->extruder_1.ports.step_port   = EXTRUDER_1_MOTOR_STEP_PORT;
	motors->extruder_1.pins.step_pin    = EXTRUDER_1_MOTOR_STEP_PIN;

	motors->xl_axis.ports.dir_port    = XL_AXIS_MOTOR_DIR_PORT;
	motors->xl_axis.pins.dir_pin     = XL_AXIS_MOTOR_DIR_PIN;
	motors->xl_axis.ports.en_port     = XL_AXIS_MOTOR_EN_PORT;
	motors->xl_axis.pins.en_pin      = XL_AXIS_MOTOR_EN_PIN;
	motors->xl_axis.ports.ms0_port    = XL_AXIS_MOTOR_MS0_PORT;
	motors->xl_axis.pins.ms0_pin     = XL_AXIS_MOTOR_MS0_PIN;
	motors->xl_axis.ports.ms1_port    = XL_AXIS_MOTOR_MS1_PORT;
	motors->xl_axis.pins.ms1_pin     = XL_AXIS_MOTOR_MS1_PIN;
	motors->xl_axis.ports.max_es_port = ENDSTOP_R_PORT;
	motors->xl_axis.pins.max_es_pin  = ENDSTOP_R_PIN;
	motors->xl_axis.ports.min_es_port = ENDSTOP_L_PORT;
	motors->xl_axis.pins.min_es_pin  = ENDSTOP_L_PIN;
	motors->xl_axis.ports.step_port   = XL_AXIS_MOTOR_STEP_PORT;
	motors->xl_axis.pins.step_pin    = XL_AXIS_MOTOR_STEP_PIN;

	motors->yf_axis.ports.dir_port    = YF_AXIS_MOTOR_DIR_PORT;
	motors->yf_axis.pins.dir_pin     = YF_AXIS_MOTOR_DIR_PIN;
	motors->yf_axis.ports.en_port     = YF_AXIS_MOTOR_EN_PORT;
	motors->yf_axis.pins.en_pin      = YF_AXIS_MOTOR_EN_PIN;
	motors->yf_axis.ports.ms0_port    = YF_AXIS_MOTOR_MS0_PORT;
	motors->yf_axis.pins.ms0_pin     = YF_AXIS_MOTOR_MS0_PIN;
	motors->yf_axis.ports.ms1_port    = YF_AXIS_MOTOR_MS1_PORT;
	motors->yf_axis.pins.ms1_pin     = YF_AXIS_MOTOR_MS1_PIN;
	motors->yf_axis.ports.max_es_port = ENDSTOP_B_PORT;
	motors->yf_axis.pins.max_es_pin  = ENDSTOP_B_PIN;
	motors->yf_axis.ports.min_es_port = ENDSTOP_F_PORT;
	motors->yf_axis.pins.min_es_pin  = ENDSTOP_F_PIN;
	motors->yf_axis.ports.step_port   = YF_AXIS_MOTOR_STEP_PORT;
	motors->yf_axis.pins.step_pin    = YF_AXIS_MOTOR_STEP_PIN;

	motors->zl_axis.ports.dir_port    = ZL_AXIS_MOTOR_DIR_PORT;
	motors->zl_axis.pins.dir_pin     = ZL_AXIS_MOTOR_DIR_PIN;
	motors->zl_axis.ports.en_port     = ZL_AXIS_MOTOR_EN_PORT;
	motors->zl_axis.pins.en_pin      = ZL_AXIS_MOTOR_EN_PIN;
	motors->zl_axis.ports.ms0_port    = ZL_AXIS_MOTOR_MS0_PORT;
	motors->zl_axis.pins.ms0_pin     = ZL_AXIS_MOTOR_MS0_PIN;
	motors->zl_axis.ports.ms1_port    = ZL_AXIS_MOTOR_MS1_PORT;
	motors->zl_axis.pins.ms1_pin     = ZL_AXIS_MOTOR_MS1_PIN;
	motors->zl_axis.ports.max_es_port = ENDSTOP_TL_PORT;
	motors->zl_axis.pins.max_es_pin  = ENDSTOP_TL_PIN;
	motors->zl_axis.ports.min_es_port = ENDSTOP_BL_PORT;
	motors->zl_axis.pins.min_es_pin  = ENDSTOP_BL_PIN;
	motors->zl_axis.ports.step_port   = ZL_AXIS_MOTOR_STEP_PORT;
	motors->zl_axis.pins.step_pin    = ZL_AXIS_MOTOR_STEP_PIN;

	motors->zr_axis.ports.dir_port    = ZR_AXIS_MOTOR_DIR_PORT;
	motors->zr_axis.pins.dir_pin     = ZR_AXIS_MOTOR_DIR_PIN;
	motors->zr_axis.ports.en_port     = ZR_AXIS_MOTOR_EN_PORT;
	motors->zr_axis.pins.en_pin      = ZR_AXIS_MOTOR_EN_PIN;
	motors->zr_axis.ports.ms0_port    = ZR_AXIS_MOTOR_MS0_PORT;
	motors->zr_axis.pins.ms0_pin     = ZR_AXIS_MOTOR_MS0_PIN;
	motors->zr_axis.ports.ms1_port    = ZR_AXIS_MOTOR_MS1_PORT;
	motors->zr_axis.pins.ms1_pin     = ZR_AXIS_MOTOR_MS1_PIN;
	motors->zr_axis.ports.max_es_port = ENDSTOP_TR_PORT;
	motors->zr_axis.pins.max_es_pin  = ENDSTOP_TR_PIN;
	motors->zr_axis.ports.min_es_port = ENDSTOP_BR_PORT;
	motors->zr_axis.pins.min_es_pin  = ENDSTOP_BR_PIN;
	motors->zr_axis.ports.step_port   = ZR_AXIS_MOTOR_STEP_PORT;
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
	motor->direction = 0;
	motor->position = 10000;
	motor->move_count = 0;
	motor->homed = 0;
	check_endstop(motor);
	motor->period = 10000; // HIGH VALUE AT FIRST AS DEFAULT
	motor->step_timer = 0;
	motor->find_zero = 0;
	motor->find_max = 0;
	strcpy(motor->name, name);
}

void enable_motor(cnc_motor_struct* motor){
	GPIO_PinOutClear(motor->ports.en_port, motor->pins.en_pin);
	motor->enabled = 1;
}

void disable_motor(cnc_motor_struct* motor){
	GPIO_PinOutSet(motor->ports.en_port, motor->pins.en_pin);
	motor->enabled = 0;
}

void set_motor_direction(cnc_motor_struct* motor, int8_t direction){
	switch(direction){
		case MOTOR_STILL : {
			GPIO_PinOutClear(motor->ports.dir_port, motor->pins.dir_pin);
			motor->direction = direction;
			break;
		}
		case MOTOR_MOVE_INCREASE : {
			GPIO_PinOutSet(motor->ports.dir_port, motor->pins.dir_pin);
			motor->direction = direction;
			break;
		}
		case MOTOR_MOVE_DECREASE : {
			GPIO_PinOutClear(motor->ports.dir_port, motor->pins.dir_pin);
			motor->direction = direction;
			break;
		}
		default : {
			GPIO_PinOutClear(motor->ports.dir_port, motor->pins.dir_pin);
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
	if(update_motors){
		update_motors = 0;
		process_motors(cnc->motors);
		check_endstops(cnc->motors);
		check_period(cnc->motors);
		// check period and update if needed
	}
}

void check_endstop(struct cnc_motor_struct* motor){
	//motors->xl_axis.max_range_flag;
	if(GPIO_PinInGet(motor->ports.min_es_port, motor->pins.min_es_pin)){
		// switch not hit
		motor->min_range_flag = ENDSTOP_NOT_HIT;
	} else {
		// switch hit or no power to switch
		motor->min_range_flag = ENDSTOP_HIT_OR_NO_POWER;
	}
	if(GPIO_PinInGet(motor->ports.max_es_port, motor->pins.max_es_pin)){
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
	/*if(motors->zl_axis.min_range_flag == ENDSTOP_HIT_OR_NO_POWER)
		motors->zr_axis.min_range_flag = ENDSTOP_HIT_OR_NO_POWER;
	if(motors->zl_axis.max_range_flag == ENDSTOP_HIT_OR_NO_POWER)
			motors->zr_axis.max_range_flag = ENDSTOP_HIT_OR_NO_POWER;
	if(motors->zr_axis.min_range_flag == ENDSTOP_HIT_OR_NO_POWER)
			motors->zl_axis.min_range_flag = ENDSTOP_HIT_OR_NO_POWER;
	if(motors->zr_axis.max_range_flag == ENDSTOP_HIT_OR_NO_POWER)
			motors->zl_axis.max_range_flag = ENDSTOP_HIT_OR_NO_POWER;*/
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
					motor->move_count = motor->move_count + motor->direction;
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
	GPIO_PinOutSet(motor->ports.step_port, motor->pins.step_pin);
}

void step_motor_low(struct cnc_motor_struct* motor){
	GPIO_PinOutClear(motor->ports.step_port, motor->pins.step_pin);
}

void check_period(struct cnc_motor_list_struct* motors){
	check_direction(&motors->aux);
	check_direction(&motors->extruder_0);
	check_direction(&motors->extruder_1);
	check_direction(&motors->xl_axis);
	check_direction(&motors->yf_axis);
	check_direction(&motors->zl_axis);
	check_direction(&motors->zr_axis);
}

void check_direction(struct cnc_motor_struct* motor){
	if(!motor->set && motor->move_count){
		if(motor->max_range_flag){
			if(motor->direction == MOTOR_MOVE_INCREASE){
				set_motor_direction(motor, MOTOR_MOVE_DECREASE);
				motor->move_count = 0;
				cnc_printf(&cnc,"%s: Max Range Hit", motor->name);
			}
		}
		if(motor->min_range_flag){
			if(motor->direction == MOTOR_MOVE_DECREASE){
				set_motor_direction(motor, MOTOR_MOVE_INCREASE);
				motor->move_count = 0;
				cnc_printf(&cnc,"%s: Min Range Hit", motor->name);
			}
		}
		motor->step_timer = motor->period;
		motor->set = 1;
	}
}
