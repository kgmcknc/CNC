/*
 * cnc_instructions.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#include "cnc_functions.h"
#include "cnc_instructions.h"
#include "common_cnc.h"

void init_instructions(struct cnc_state_struct* cnc){
	uint32_t clear_count = 0;

	cnc->instruction_fullness = 0;
	cnc->instruction_wp = 0;
	cnc->instruction_rp = 0;

	for(clear_count=0;clear_count<INSTRUCTION_FIFO_DEPTH-1;clear_count++){
		clear_instruction(&cnc->instruction_array[clear_count]);
	}
}

void clear_instruction(struct cnc_instruction_struct* instruction){
	instruction->instruction_valid = 0;
	//instruction->instruction_number = 0;
	instruction->opcode_flags = PROGRAM_IDLE;
	instruction->program_length = 0;
}

void handle_instructions(struct cnc_state_struct* cnc){
	if(cnc->current_instruction.instruction_valid){
		switch(cnc->current_instruction.opcode_flags){
			case PROGRAM_IDLE : {

				break;
			}
			case PROGRAM_START : {

				break;
			}
			case PROGRAM_PAUSE : {

				break;
			}
			case PROGRAM_RESUME : {

				break;
			}
			case PROGRAM_END : {
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			case PROGRAM_RESET : {

				break;
			}
			case SYSTEM_CHECK : {

				break;
			}
			case INSTRUCTION : {
				check_motor_instruction(&cnc->current_instruction.xl_axis, &cnc->motors->xl_axis);
				check_motor_instruction(&cnc->current_instruction.yf_axis, &cnc->motors->yf_axis);
				check_motor_instruction(&cnc->current_instruction.zl_axis, &cnc->motors->zl_axis);
				check_motor_instruction(&cnc->current_instruction.zr_axis, &cnc->motors->zr_axis);
				check_motor_instruction(&cnc->current_instruction.extruder_0, &cnc->motors->extruder_0);
				check_motor_instruction(&cnc->current_instruction.extruder_1, &cnc->motors->extruder_1);
				check_motor_instruction(&cnc->current_instruction.aux, &cnc->motors->aux);
				check_heater_instruction(&cnc->current_instruction.heater_0, &cnc->heaters->heater_0);
				check_heater_instruction(&cnc->current_instruction.heater_1, &cnc->heaters->heater_1);
				check_heater_instruction(&cnc->current_instruction.heater_2, &cnc->heaters->heater_2);
				check_heater_instruction(&cnc->current_instruction.heater_3, &cnc->heaters->heater_3);
				check_instruction(&cnc->current_instruction);
				break;
			}
			case CHECK_ENDSTOPS : {

				break;
			}
			case ZERO_MOTORS : {
				if(cnc->motors->xl_axis.find_zero){
					if(cnc->motors->xl_axis.min_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						cnc->motors->xl_axis.homed = 1;
						cnc->motors->xl_axis.position = 0;
						cnc->motors->xl_axis.active = 0;
						cnc->motors->xl_axis.find_zero = 0;
						cnc->current_instruction.xl_axis.valid_instruction = 0;
					}
				}
				if(cnc->motors->yf_axis.find_zero){
					if(cnc->motors->yf_axis.min_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						cnc->motors->yf_axis.homed = 1;
						cnc->motors->yf_axis.position = 0;
						cnc->motors->yf_axis.active = 0;
						cnc->motors->yf_axis.find_zero = 0;
						cnc->current_instruction.yf_axis.valid_instruction = 0;
					}
				}
				if(cnc->motors->zl_axis.find_zero){
					if(cnc->motors->zl_axis.min_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						cnc->motors->zl_axis.homed = 1;
						cnc->motors->zl_axis.position = 0;
						cnc->motors->zl_axis.active = 0;
						cnc->motors->zl_axis.find_zero = 0;
						cnc->current_instruction.zl_axis.valid_instruction = 0;
					}
				}
				if(cnc->motors->zr_axis.find_zero){
					if(cnc->motors->zr_axis.min_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						cnc->motors->zr_axis.homed = 1;
						cnc->motors->zr_axis.position = 0;
						cnc->motors->zr_axis.active = 0;
						cnc->motors->zr_axis.find_zero = 0;
						cnc->current_instruction.zr_axis.valid_instruction = 0;
					}
				}
				check_instruction(&cnc->current_instruction);
				break;
			}
			case MAX_MOTORS : {
				if(cnc->motors->xl_axis.find_max){
					if(cnc->motors->xl_axis.max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						cnc->motors->xl_axis.active = 0;
						cnc->motors->xl_axis.find_max = 0;
						cnc->current_instruction.xl_axis.valid_instruction = 0;
					}
				}
				if(cnc->motors->yf_axis.find_max){
					if(cnc->motors->yf_axis.max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						cnc->motors->yf_axis.active = 0;
						cnc->motors->yf_axis.find_max = 0;
						cnc->current_instruction.yf_axis.valid_instruction = 0;
					}
				}
				if(cnc->motors->zl_axis.find_max){
					if(cnc->motors->zl_axis.max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						cnc->motors->zl_axis.active = 0;
						cnc->motors->zl_axis.find_max = 0;
						cnc->current_instruction.zl_axis.valid_instruction = 0;
					}
				}
				if(cnc->motors->zr_axis.find_max){
					if(cnc->motors->zr_axis.max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						cnc->motors->zr_axis.active = 0;
						cnc->motors->zr_axis.find_max = 0;
						cnc->current_instruction.zr_axis.valid_instruction = 0;
					}
				}
				check_instruction(&cnc->current_instruction);
				break;
			}
			case ENABLE_MOTORS : {
				cnc_printf(cnc, "Enabling Motors!");
				if(cnc->current_instruction.pending_motor_enables[0]) enable_motor(&cnc->motors->xl_axis);
				if(cnc->current_instruction.pending_motor_enables[1]) enable_motor(&cnc->motors->yf_axis);
				if(cnc->current_instruction.pending_motor_enables[2]) enable_motor(&cnc->motors->zl_axis);
				if(cnc->current_instruction.pending_motor_enables[3]) enable_motor(&cnc->motors->zr_axis);
				if(cnc->current_instruction.pending_motor_enables[4]) enable_motor(&cnc->motors->extruder_0);
				if(cnc->current_instruction.pending_motor_enables[5]) enable_motor(&cnc->motors->extruder_1);
				if(cnc->current_instruction.pending_motor_enables[6]) enable_motor(&cnc->motors->aux);
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			case DISABLE_MOTORS : {
				cnc_printf(cnc, "Disabling Motors!");
				if(cnc->current_instruction.pending_motor_disables[0]) disable_motor(&cnc->motors->xl_axis);
				if(cnc->current_instruction.pending_motor_disables[1]) disable_motor(&cnc->motors->yf_axis);
				if(cnc->current_instruction.pending_motor_disables[2]) disable_motor(&cnc->motors->zl_axis);
				if(cnc->current_instruction.pending_motor_disables[3]) disable_motor(&cnc->motors->zr_axis);
				if(cnc->current_instruction.pending_motor_disables[4]) disable_motor(&cnc->motors->extruder_0);
				if(cnc->current_instruction.pending_motor_disables[5]) disable_motor(&cnc->motors->extruder_1);
				if(cnc->current_instruction.pending_motor_disables[4]) disable_motor(&cnc->motors->aux);
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			case ENABLE_EXTRUDERS: {
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			case DISABLE_EXTRUDERS: {

				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			case ENABLE_HEATERS : {
				cnc_printf(cnc, "Enabling Heaters!");
				//if(cnc->current_instruction.pending_heater_enables[0])
				//if(cnc->current_instruction.pending_heater_enables[1])
				//if(cnc->current_instruction.pending_heater_enables[2])
				//if(cnc->current_instruction.pending_heater_enables[3])
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			case DISABLE_HEATERS : {
				//if(cnc->current_instruction.pending_heater_disables[0])
				//if(cnc->current_instruction.pending_heater_disables[1])
				//if(cnc->current_instruction.pending_heater_disables[2])
				//if(cnc->current_instruction.pending_heater_disables[3])
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			case ENABLE_FANS : {
				cnc->current_instruction.instruction_valid = 0;
				//enable_heater_fans();
				break;
			}
			case DISABLE_FANS : {
				cnc->current_instruction.instruction_valid = 0;
				//disable_heater_fans();
				break;
			}
			case ENABLE_TOOLS : {
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			case DISABLE_TOOLS : {
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
			default : {
				cnc->current_instruction.instruction_valid = 0;
				break;
			}
		}
	}
}

void handle_program(struct cnc_state_struct* cnc){
	if(cnc->state == CNC_IDLE){
		if(cnc->start_program){
			cnc->start_program = 0;
			cnc->state = CNC_PROGRAM_RUNNING;
			cnc_printf(cnc, "Started CNC Program!");
			cnc->instruction_array[cnc->instruction_rp].instruction_valid = 0;
			cnc->instruction_rp = (cnc->instruction_rp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->instruction_rp + 1 : 0;
		}
	} else {
		if(cnc->request_instruction == 1){
			if((((cnc->instruction_array[cnc->instruction_wp].instruction_valid)) && (cnc->instruction_array[cnc->instruction_wp].opcode_flags == PROGRAM_END)) ||
					(cnc->instruction_fullness >= (INSTRUCTION_FIFO_DEPTH-2))){
				cnc->request_instruction = 0;
			}
		} else {
			if((cnc->instruction_array[cnc->instruction_wp].instruction_valid) &&
					(cnc->instruction_array[cnc->instruction_wp].opcode_flags == PROGRAM_END)){
				cnc->request_instruction = 0;
			} else {
				if(cnc->instruction_fullness < (INSTRUCTION_FIFO_DEPTH/2)){
					cnc->request_instruction = 1;
				}
			}
		}
		if(!cnc->current_instruction.instruction_valid && cnc->instruction_array[cnc->instruction_rp].instruction_valid){
			copy_instruction(&cnc->instruction_array[cnc->instruction_rp], &cnc->current_instruction);
			cnc->instruction_array[cnc->instruction_rp].instruction_valid = 0;
			cnc->instruction_rp = (cnc->instruction_rp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->instruction_rp + 1 : 0;
			cnc->instruction_fullness--;
		}
		if((cnc->abort_program) || ((cnc->current_instruction.opcode_flags == PROGRAM_END) && (cnc->current_instruction.instruction_valid))){
			cnc->abort_program = 0;
			cnc_printf(cnc, "Finished CNC Program!");
			cnc->instruction_fullness = 0;
			cnc->instruction_wp = 0;
			cnc->instruction_rp = 0;
			cnc->instruction_array[cnc->instruction_rp].instruction_valid = 0;
			cnc->current_instruction.instruction_valid = 0;
			cnc->state = CNC_IDLE;
		}
	}
}

void check_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor){
	if(current_instruction->valid_instruction){
		if(current_instruction->end_position == motor->position){
			current_instruction->valid_instruction = 0;
			motor->active = 0;
		}
	} else {
		motor->active = 0;
	}
}

void check_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater){
	if(current_instruction->temp_locked){
		if(current_instruction->target_temp == heater->current_temp){
			current_instruction->temp_locked = 1;
		}
	}
}

void check_instruction(struct cnc_instruction_struct* current_instruction){
	char valid_instruction = 0;
	valid_instruction |= current_instruction->aux.valid_instruction;
	valid_instruction |= current_instruction->extruder_0.valid_instruction;
	valid_instruction |= current_instruction->extruder_1.valid_instruction;
	valid_instruction |= current_instruction->xl_axis.valid_instruction;
	valid_instruction |= current_instruction->yf_axis.valid_instruction;
	valid_instruction |= current_instruction->zl_axis.valid_instruction;
	valid_instruction |= current_instruction->zr_axis.valid_instruction;
	valid_instruction |= current_instruction->heater_0.valid_instruction;
	valid_instruction |= current_instruction->heater_1.valid_instruction;
	valid_instruction |= current_instruction->heater_2.valid_instruction;
	valid_instruction |= current_instruction->heater_3.valid_instruction;
	current_instruction->instruction_valid = valid_instruction;
}

void copy_instruction(struct cnc_instruction_struct* new_instruction, struct cnc_instruction_struct* current_instruction){
	//current_instruction->instruction_number = new_instruction->instruction_number;
	current_instruction->instruction_valid = new_instruction->instruction_valid;
	current_instruction->program_length = new_instruction->program_length;
	current_instruction->opcode_flags = new_instruction->opcode_flags;
	current_instruction->pending_motor_enables[0] = new_instruction->pending_motor_enables[0];
	current_instruction->pending_motor_enables[1] = new_instruction->pending_motor_enables[1];
	current_instruction->pending_motor_disables[0] = new_instruction->pending_motor_disables[0];
	current_instruction->pending_motor_disables[1] = new_instruction->pending_motor_disables[1];
	strcpy(current_instruction->pending_heater_enables, new_instruction->pending_heater_enables);
	strcpy(current_instruction->pending_heater_disables, new_instruction->pending_heater_disables);
	copy_motor_instruction(&new_instruction->aux, &current_instruction->aux);
	copy_motor_instruction(&new_instruction->extruder_0, &current_instruction->extruder_0);
	copy_motor_instruction(&new_instruction->extruder_1, &current_instruction->extruder_1);
	copy_motor_instruction(&new_instruction->xl_axis, &current_instruction->xl_axis);
	copy_motor_instruction(&new_instruction->yf_axis, &current_instruction->yf_axis);
	copy_motor_instruction(&new_instruction->zl_axis, &current_instruction->zl_axis);
	copy_motor_instruction(&new_instruction->zr_axis, &current_instruction->zr_axis);
	copy_heater_instruction(&new_instruction->heater_0, &current_instruction->heater_0);
	copy_heater_instruction(&new_instruction->heater_1, &current_instruction->heater_1);
	copy_heater_instruction(&new_instruction->heater_2, &current_instruction->heater_2);
	copy_heater_instruction(&new_instruction->heater_3, &current_instruction->heater_3);
}

void copy_motor_instruction(struct cnc_motor_instruction_struct* new_instruction, struct cnc_motor_instruction_struct* current_instruction){
	current_instruction->current_period = new_instruction->current_period;
	current_instruction->end_position = new_instruction->end_position;
	current_instruction->ramp_down_speed = new_instruction->ramp_down_speed;
	current_instruction->ramp_up_speed = new_instruction->ramp_up_speed;
	current_instruction->step_period = new_instruction->step_period;
	current_instruction->valid_instruction = new_instruction->valid_instruction;
	current_instruction->find_zero = new_instruction->find_zero;
	current_instruction->find_max = new_instruction->find_max;
}

void copy_heater_instruction(struct cnc_heater_instruction_struct* new_instruction, struct cnc_heater_instruction_struct* current_instruction){
	current_instruction->current_temp = new_instruction->current_temp;
	current_instruction->enabled = new_instruction->enabled;
	current_instruction->fan_on = new_instruction->fan_on;
	current_instruction->heater_on = new_instruction->heater_on;
	current_instruction->target_temp = new_instruction->target_temp;
	current_instruction->temp_locked = new_instruction->temp_locked;
}

