/*
 * cnc_instructions.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

// need to do:
// parse instruction
// check instruction
// process instruction

#include "cnc_functions.h"
#include "cnc_instructions.h"
#include "common_cnc.h"

void init_instructions(struct cnc_state_struct* cnc){
	uint32_t clear_count = 0;

	clear_program(cnc);
	clear_instruction(&cnc->current_instruction);
	clear_instruction(&cnc->new_instruction);
	clear_instruction(&cnc->instant_instruction);

	for(clear_count=0;clear_count<INSTRUCTION_FIFO_DEPTH-1;clear_count++){
		clear_instruction(&cnc->instruction_array[clear_count]);
	}
}

void receive_instruction(struct cnc_state_struct* cnc){
	parse_instruction(cnc);
	if(cnc->new_instruction.instruction_valid){
		if(cnc->new_instruction.instant_instruction){
			if(cnc->new_instruction.opcode == ABORT_INSTRUCTION){
				cnc_printf(cnc, "Received Abort");
				// we just received an abort from an instant instruction... STOP!
				cnc->instant_instruction.opcode = ABORT_INSTRUCTION;
				cnc->current_instruction.opcode = ABORT_INSTRUCTION;
			} else {
				if(cnc->instant_instruction.instruction_valid){
					// instant instruction already set... can't overwrite...
					// but we can check to see if this is an abort...
				} else {
					copy_instruction(&cnc->new_instruction, &cnc->instant_instruction);
				}
			}
		} else {
			cnc->instruction_request_sent = 0;
			if(cnc->program_running){
				cnc_printf(cnc, "Received Program Instruction!");
				// add to running program
				copy_instruction(&cnc->new_instruction, &cnc->instruction_array[cnc->instruction_wp]);
				cnc->instruction_wp = (cnc->instruction_wp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->instruction_wp + 1 : 0;
				cnc->instruction_fullness++;
				cnc->new_instruction.instruction_valid = 0;
				if(cnc->new_instruction.opcode == PROGRAM_END){
					cnc->program_received = 1;
					cnc_printf(cnc, "Received Program End!");
				}
			} else {
				if(cnc->new_instruction.opcode == PROGRAM_START){
					cnc_printf(cnc, "Received Program Start!");
					copy_instruction(&cnc->new_instruction, &cnc->instruction_array[cnc->instruction_wp]);
					cnc->instruction_wp = (cnc->instruction_wp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->instruction_wp + 1 : 0;
					cnc->instruction_fullness++;
					cnc->new_instruction.instruction_valid = 0;
				} else {
					// program isn't running and it's not an immediate instruction  or start of program... so ignore...
				}
			}
		}
		cnc->new_instruction.instruction_valid = 0;
	} else {
		// not valid instruction
	}
}

void clear_program(struct cnc_state_struct* cnc){
	cnc_printf(cnc, "resetting program information");
	cnc->program_running = 0;
	cnc->program_received = 0;
	cnc->program_length = 0;
	cnc->instruction_fullness = 0;
	cnc->instruction_wp = 0;
	cnc->instruction_rp = 0;
	cnc->instruction_array[cnc->instruction_rp].instruction_valid = 0;
	cnc->current_instruction.instruction_valid = 0;
	cnc->instruction_request_sent = 0;
	cnc->request_instruction = 0;
}

void parse_instruction(struct cnc_state_struct* cnc){
	// convert received spi data to cnc instruction
	string_to_instruction(&cnc->new_instruction, &cnc->cnc_read_data[1]);
}

void get_next_instruction(struct cnc_state_struct* cnc){
	if(cnc->instant_instruction.instruction_valid){
		// instant instruction ready
		// copy to current
		copy_instruction(&cnc->instant_instruction, &cnc->current_instruction);
		clear_instruction(&cnc->instant_instruction);
	} else {
		if(cnc->instruction_fullness > 0){
			// we have an instruction ready to copy over
			copy_instruction(&cnc->instruction_array[cnc->instruction_rp], &cnc->current_instruction);
			cnc->instruction_array[cnc->instruction_rp].instruction_valid = 0;
			cnc->instruction_rp = (cnc->instruction_rp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->instruction_rp + 1 : 0;
			cnc->instruction_fullness--;
		} else {
			// shouldn't ever hit this when program is running!
			// means instruction request underflowed!!!
		}
	}
}

void check_instruction_fifo(struct cnc_state_struct* cnc){
	// check to see if instruction threshold is too low and not finished reading full program
	// if so, check if already writing spi
	// if not, write to request instruction
	if(cnc->program_running && !cnc->program_received){
		if(cnc->request_instruction){
			if(cnc->instruction_fullness >= (INSTRUCTION_FIFO_DEPTH-2)){
				cnc->request_instruction = 0;
			} else {
				if(!cnc->instruction_request_sent && (cnc->state == CNC_IDLE)){
					if(cnc->write_in_progress){
						// already writing... wait till it's finished
						if(spi_check_write() > 0){
							cnc->write_in_progress = 0;
							cnc->instruction_request_sent = 1;
						}
					} else {
						// not writing, so we can write
						if(spi_check_write() < 0){
							// can't write, already writing
						} else {
							cnc->cnc_write_data[0] = (char) NEW_CNC_INSTRUCTION;
							cnc->cnc_write_length = 1;
							if(spi_set_write(cnc->cnc_write_data, cnc->cnc_write_length) > 0){
								cnc->write_in_progress = 1;
							} else {
								cnc->write_in_progress = 0;
							}
						}
					}
				}
			}
		} else {
			if(cnc->instruction_fullness < (INSTRUCTION_FIFO_DEPTH/2)){
				cnc->request_instruction = 1;
				cnc_printf(cnc, "setting read request high");
			} else {
				// we have enough instructions, so do nothing
			}
		}
	} else {
		// don't need to do anything if not running a program
		cnc->request_instruction = 0;
	}
}

void process_instruction(struct cnc_state_struct* cnc){
	handle_instruction_opcodes(cnc, &cnc->current_instruction);
	handle_motors(cnc);
	handle_heaters(cnc);
}

void handle_instructions(struct cnc_state_struct* cnc){
	check_instruction_fifo(cnc);
	check_instruction(cnc);
	set_instruction(cnc);
	process_instruction(cnc);
}

void check_errors(struct cnc_state_struct* cnc){
	// check to see if endstops in both directions (min and max) are hit at once... that's def an error
	// others...
}

void set_instruction(struct cnc_state_struct* cnc){
	if(cnc->current_instruction.instruction_valid){
		if(cnc->current_instruction.instruction_set){
			// instruction already been set to functions
		} else {
			set_motor_instruction(&cnc->current_instruction.aux, &cnc->motors->aux);
			set_motor_instruction(&cnc->current_instruction.extruder_0, &cnc->motors->extruder_0);
			set_motor_instruction(&cnc->current_instruction.extruder_1, &cnc->motors->extruder_1);
			set_motor_instruction(&cnc->current_instruction.xl_axis, &cnc->motors->xl_axis);
			set_motor_instruction(&cnc->current_instruction.yf_axis, &cnc->motors->yf_axis);
			set_motor_instruction(&cnc->current_instruction.zl_axis, &cnc->motors->zl_axis);
			set_motor_instruction(&cnc->current_instruction.zr_axis, &cnc->motors->zr_axis);
			set_heater_instruction(&cnc->current_instruction.heater_0, &cnc->heaters->heater_0);
			set_heater_instruction(&cnc->current_instruction.heater_1, &cnc->heaters->heater_1);
			set_heater_instruction(&cnc->current_instruction.heater_2, &cnc->heaters->heater_2);
			set_heater_instruction(&cnc->current_instruction.heater_3, &cnc->heaters->heater_3);
			cnc->current_instruction.instruction_set = 1;
		}
	} else {
		cnc->current_instruction.instruction_set = 0;
	}
}

void set_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor){
	if(current_instruction->instruction_valid){
		motor->move_count = current_instruction->move_count;
		motor->period = current_instruction->current_period;
		if(motor->move_count > 0){
			set_motor_direction(motor, MOTOR_MOVE_INCREASE);
		} else {
			if(motor->move_count < 0){
				set_motor_direction(motor, MOTOR_MOVE_DECREASE);
			} else {
				set_motor_direction(motor, MOTOR_STILL);
			}
		}
	}
}

void set_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater){
	if(current_instruction->instruction_valid){
		heater->reset_heater = 1;
		heater->target_temp = current_instruction->target_temp;
		heater->wait_for_temp = current_instruction->wait_for_temp;
		heater->fan_on = current_instruction->fan_on;
		heater->fan_duty = current_instruction->fan_duty;
	}
}

void check_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor){
	if(current_instruction->instruction_valid){
		if((motor->move_count == 0) && (current_instruction->opcode == EMPTY_OPCODE)){
			current_instruction->instruction_valid = 0;
		}
	}
}

void check_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater){
	if(current_instruction->instruction_valid){
		if(current_instruction->wait_for_temp){
			if(heater->current_temp == heater->target_temp){
				current_instruction->temp_locked = 1;
				current_instruction->instruction_valid = 0;
			}
		} else {
			current_instruction->instruction_valid = 0;
		}
	}
}

void abort_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor){
	if(current_instruction->instruction_valid){
		current_instruction->opcode = EMPTY_OPCODE;
		motor->move_count = 0;
	}
}

void abort_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater){
	if(current_instruction->instruction_valid){
		current_instruction->opcode = EMPTY_OPCODE;
		heater->target_temp = 0;
		heater->wait_for_temp = 0;
	}
}

void check_instruction(struct cnc_state_struct* cnc){
	if(cnc->current_instruction.instruction_valid){
		// if so, check all the various things to see if instruction is done
		// if so, mark instruction as inactive
		// also, if it was last instruction, clear program loaded and program running

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

		uint8_t valid_instruction = 0;
		valid_instruction |= (cnc->current_instruction.opcode != EMPTY_OPCODE);
		valid_instruction |= cnc->current_instruction.aux.instruction_valid;
		valid_instruction |= cnc->current_instruction.extruder_0.instruction_valid;
		valid_instruction |= cnc->current_instruction.extruder_1.instruction_valid;
		valid_instruction |= cnc->current_instruction.xl_axis.instruction_valid;
		valid_instruction |= cnc->current_instruction.yf_axis.instruction_valid;
		valid_instruction |= cnc->current_instruction.zl_axis.instruction_valid;
		valid_instruction |= cnc->current_instruction.zr_axis.instruction_valid;
		valid_instruction |= cnc->current_instruction.heater_0.instruction_valid;
		valid_instruction |= cnc->current_instruction.heater_1.instruction_valid;
		valid_instruction |= cnc->current_instruction.heater_2.instruction_valid;
		valid_instruction |= cnc->current_instruction.heater_3.instruction_valid;

		if(valid_instruction){
			// instruction isn't done
		} else {
			if(cnc->current_instruction.instant_instruction){
				cnc->marker_set = 1;
			}
			clear_instruction(&cnc->current_instruction);
		}
	}

	if(cnc->current_instruction.instruction_valid){
		// instruction didn't finish with this check
	} else {
		// instruction finished, so get new instruction
		get_next_instruction(cnc);
	}
}

void copy_instruction(struct cnc_instruction_struct* new_instruction, struct cnc_instruction_struct* current_instruction){
	//current_instruction->instruction_number = new_instruction->instruction_number;
	current_instruction->instruction_valid = new_instruction->instruction_valid;
	current_instruction->instant_instruction = new_instruction->instant_instruction;
	current_instruction->instruction_set = 0;
	current_instruction->opcode = new_instruction->opcode;
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
	current_instruction->opcode = new_instruction->opcode;
	current_instruction->current_period = new_instruction->current_period;
	current_instruction->move_count = new_instruction->move_count;
	current_instruction->ramp_down_speed = new_instruction->ramp_down_speed;
	current_instruction->ramp_up_speed = new_instruction->ramp_up_speed;
	current_instruction->instruction_valid = new_instruction->instruction_valid;
	current_instruction->find_zero = new_instruction->find_zero;
	current_instruction->find_max = new_instruction->find_max;
	current_instruction->pending_disable = new_instruction->pending_disable;
	current_instruction->pending_enable = new_instruction->pending_enable;
}

void copy_heater_instruction(struct cnc_heater_instruction_struct* new_instruction, struct cnc_heater_instruction_struct* current_instruction){
	current_instruction->opcode = new_instruction->opcode;
	current_instruction->instruction_valid = new_instruction->instruction_valid;
	current_instruction->current_temp = new_instruction->current_temp;
	current_instruction->enabled = new_instruction->enabled;
	current_instruction->fan_on = new_instruction->fan_on;
	current_instruction->fan_duty = new_instruction->fan_duty;
	current_instruction->heater_on = new_instruction->heater_on;
	current_instruction->target_temp = new_instruction->target_temp;
	current_instruction->temp_locked = new_instruction->temp_locked;
}

void handle_instruction_opcodes(struct cnc_state_struct* cnc, struct cnc_instruction_struct* instruction){
	// make opcode handler for motor and heater
	handle_motor_opcode(cnc, &instruction->aux, &cnc->motors->aux);
	handle_motor_opcode(cnc, &instruction->extruder_0, &cnc->motors->extruder_0);
	handle_motor_opcode(cnc, &instruction->extruder_1, &cnc->motors->extruder_1);
	handle_motor_opcode(cnc, &instruction->xl_axis, &cnc->motors->xl_axis);
	handle_motor_opcode(cnc, &instruction->yf_axis, &cnc->motors->yf_axis);
	handle_motor_opcode(cnc, &instruction->zl_axis, &cnc->motors->zl_axis);
	handle_motor_opcode(cnc, &instruction->zr_axis, &cnc->motors->zr_axis);
	handle_heater_opcode(cnc, &instruction->heater_0, &cnc->heaters->heater_0);
	handle_heater_opcode(cnc, &instruction->heater_1, &cnc->heaters->heater_1);
	handle_heater_opcode(cnc, &instruction->heater_2, &cnc->heaters->heater_2);
	handle_heater_opcode(cnc, &instruction->heater_3, &cnc->heaters->heater_3);

	if(instruction->instruction_valid){
		switch(instruction->opcode){
			case EMPTY_OPCODE : {
				// nothing to do
				break;
			}
			case PROGRAM_START : {
				cnc->program_received = 0;
				cnc->program_running = 1;
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
			case PROGRAM_END : {
				cnc->program_received = 0;
				cnc->program_running = 0;
				instruction->opcode = EMPTY_OPCODE;
				clear_program(cnc);
				break;
			}
			case MEASURE_AXIS : {
				// check all motor opcodes and if all are empty, then send status back
				uint8_t instruction_done = 1;
				if(instruction->xl_axis.opcode != EMPTY_OPCODE) instruction_done = 0;
				if(instruction->yf_axis.opcode != EMPTY_OPCODE) instruction_done = 0;
				if(instruction->zl_axis.opcode != EMPTY_OPCODE) instruction_done = 0;
				if(instruction->zr_axis.opcode != EMPTY_OPCODE) instruction_done = 0;
				if(instruction_done){
					// send status back to interfaces
					if(cnc->write_in_progress){
						// can't send status - already writing
					} else {
						instruction->opcode = EMPTY_OPCODE;
					}
				}
				break;
			}
			case RETURN_STATUS : {
				if(cnc->write_in_progress){
					if(spi_check_write() > 0){
						cnc->write_in_progress = 0;
						instruction->opcode = EMPTY_OPCODE;
					}
				} else {
					if(spi_check_write() < 0){
						// can't write, already writing
					} else {
						cnc->cnc_write_data[0] = (char) GET_CNC_STATUS;
						update_status(cnc);
						cnc->cnc_write_length = status_to_string(&cnc->status, &cnc->cnc_write_data[1]);
						cnc->cnc_write_length = cnc->cnc_write_length + 1;
						if(spi_set_write(cnc->cnc_write_data, cnc->cnc_write_length) > 0){
							cnc->write_in_progress = 1;
						} else {
							cnc->write_in_progress = 0;
						}
					}
				}
				break;
			}
			case ABORT_INSTRUCTION : {
				abort_motor_instruction(&cnc->current_instruction.xl_axis, &cnc->motors->xl_axis);
				abort_motor_instruction(&cnc->current_instruction.yf_axis, &cnc->motors->yf_axis);
				abort_motor_instruction(&cnc->current_instruction.zl_axis, &cnc->motors->zl_axis);
				abort_motor_instruction(&cnc->current_instruction.zr_axis, &cnc->motors->zr_axis);
				abort_motor_instruction(&cnc->current_instruction.extruder_0, &cnc->motors->extruder_0);
				abort_motor_instruction(&cnc->current_instruction.extruder_1, &cnc->motors->extruder_1);
				abort_motor_instruction(&cnc->current_instruction.aux, &cnc->motors->aux);
				abort_heater_instruction(&cnc->current_instruction.heater_0, &cnc->heaters->heater_0);
				abort_heater_instruction(&cnc->current_instruction.heater_1, &cnc->heaters->heater_1);
				abort_heater_instruction(&cnc->current_instruction.heater_2, &cnc->heaters->heater_2);
				abort_heater_instruction(&cnc->current_instruction.heater_3, &cnc->heaters->heater_3);
				clear_instruction(&cnc->instant_instruction);
				clear_instruction(&cnc->current_instruction);
				instruction->opcode = EMPTY_OPCODE;
				break;
			}

			case ENABLE_MOTORS : {
				if(instruction->xl_axis.pending_enable){
					enable_motor(&cnc->motors->xl_axis);
				}
				if(instruction->yf_axis.pending_enable){
					enable_motor(&cnc->motors->yf_axis);
				}
				if(instruction->zl_axis.pending_enable){
					enable_motor(&cnc->motors->zl_axis);
				}
				if(instruction->zr_axis.pending_enable){
					enable_motor(&cnc->motors->zr_axis);
				}
				if(instruction->extruder_0.pending_enable){
					enable_motor(&cnc->motors->extruder_0);
				}
				if(instruction->extruder_1.pending_enable){
					enable_motor(&cnc->motors->extruder_1);
				}
				if(instruction->aux.pending_enable){
					enable_motor(&cnc->motors->aux);
				}
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
			case DISABLE_MOTORS : {
				if(instruction->xl_axis.pending_disable){
					disable_motor(&cnc->motors->xl_axis);
				}
				if(instruction->yf_axis.pending_disable){
					disable_motor(&cnc->motors->yf_axis);
				}
				if(instruction->zl_axis.pending_disable){
					disable_motor(&cnc->motors->zl_axis);
				}
				if(instruction->zr_axis.pending_disable){
					disable_motor(&cnc->motors->zr_axis);
				}
				if(instruction->extruder_0.pending_disable){
					disable_motor(&cnc->motors->extruder_0);
				}
				if(instruction->extruder_1.pending_disable){
					disable_motor(&cnc->motors->extruder_1);
				}
				if(instruction->aux.pending_disable){
					disable_motor(&cnc->motors->aux);
				}
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
			default : {
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
		}
	}
}

void handle_motor_opcode(struct cnc_state_struct* cnc, struct cnc_motor_instruction_struct* instruction, struct cnc_motor_struct* motor){
	if(instruction->instruction_valid){
		switch(instruction->opcode){
			case EMPTY_OPCODE : {
				// nothing to do
				break;
			}
			case HOME_AXIS : {
				if(motor->find_max){
					if(motor->max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						motor->find_max = 0;
						motor->move_count = motor->home_position - motor->position;
						set_motor_direction(motor, MOTOR_MOVE_DECREASE);
						instruction->opcode = EMPTY_OPCODE;
					} else {
						motor->move_count = 1;
						set_motor_direction(motor, MOTOR_MOVE_INCREASE);
					}
				} else {
					if(motor->max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						motor->move_count = -1;
						set_motor_direction(motor, MOTOR_MOVE_DECREASE);
					} else {
						motor->find_max = 1;
						motor->move_count = 1;
						set_motor_direction(motor, MOTOR_MOVE_INCREASE);
					}
				}
				break;
			}
			case MEASURE_AXIS : {
				if(motor->find_zero){
					if(motor->min_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						motor->homed = 1;
						motor->position = 0;
						motor->find_zero = 0;
						motor->find_max = 1;
					} else {
						motor->move_count = -1;
						set_motor_direction(motor, MOTOR_MOVE_DECREASE);
					}
				} else {
					if(!motor->find_max){
						if(motor->min_range_flag == ENDSTOP_HIT_OR_NO_POWER){
							motor->move_count = 1;
							set_motor_direction(motor, MOTOR_MOVE_INCREASE);
						} else {
							motor->find_zero = 1;
							motor->move_count = -1;
							set_motor_direction(motor, MOTOR_MOVE_DECREASE);
						}
					}
				}
				if(motor->find_max){
					if(motor->max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						motor->find_max = 0;
						motor->axis_length = motor->position;
						instruction->opcode = EMPTY_OPCODE;
					} else {
						motor->move_count = 1;
						set_motor_direction(motor, MOTOR_MOVE_INCREASE);
					}
				}
				// offset from zero if needed
				// find zero
				// find max
				// send status with final axis length
				break;
			}
			case POSITION_AXIS : {
				motor->position = instruction->move_count;
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
			case ZERO_MOTOR : {
				if(motor->find_zero){
					if(motor->min_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						motor->homed = 1;
						motor->position = 0;
						motor->find_zero = 0;
						instruction->opcode = EMPTY_OPCODE;
					} else {
						motor->move_count = -1;
						set_motor_direction(motor, MOTOR_MOVE_DECREASE);
					}
				} else {
					if(motor->min_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						motor->move_count = 1;
						set_motor_direction(motor, MOTOR_MOVE_INCREASE);
					} else {
						motor->find_zero = 1;
						motor->move_count = -1;
						set_motor_direction(motor, MOTOR_MOVE_DECREASE);
					}
				}
				break;
			}
			case MAX_MOTOR : {
				if(motor->find_max){
					if(motor->max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						motor->find_max = 0;
						instruction->opcode = EMPTY_OPCODE;
					} else {
						motor->move_count = 1;
						set_motor_direction(motor, MOTOR_MOVE_INCREASE);
					}
				} else {
					if(motor->max_range_flag == ENDSTOP_HIT_OR_NO_POWER){
						motor->move_count = -1;
						set_motor_direction(motor, MOTOR_MOVE_DECREASE);
					} else {
						motor->find_max = 1;
						motor->move_count = 1;
						set_motor_direction(motor, MOTOR_MOVE_INCREASE);
					}
				}
				break;
			}
			case ENABLE_MOTORS : {
				cnc_printf(cnc, "Enabling %s Motor!", motor->name);
				if(instruction->pending_enable) enable_motor(motor);
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
			case DISABLE_MOTORS : {
				cnc_printf(cnc, "Disabling %s Motor!", motor->name);
				if(instruction->pending_disable) disable_motor(motor);
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
			default : {
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
		}
	}
}


void handle_heater_opcode(struct cnc_state_struct* cnc, struct cnc_heater_instruction_struct* instruction, struct cnc_heater_struct* heater){
	if(instruction->instruction_valid){
		switch(instruction->opcode){
			case EMPTY_OPCODE : {
				// nothing to do
				break;
			}
			default : {
				instruction->opcode = EMPTY_OPCODE;
				break;
			}
		}
	}
}
