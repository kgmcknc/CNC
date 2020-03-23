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

#include "Arduino.h"
#include "cnc_functions.h"
#include "cnc_serial.h"
#include "cnc_instructions.h"
#include "common_cnc.h"

void init_instructions(struct cnc_state_struct* cnc){
	clear_program(cnc);
	clear_instruction(cnc->program.current_instruction);
	clear_instruction(&cnc->program.new_instruction);
	clear_instruction(&cnc->program.instant_instruction);
   clear_instruction(&cnc->program.empty_instruction);
}

void receive_instruction(struct cnc_state_struct* cnc){
	parse_instruction(cnc);
	if(cnc->program.new_instruction.instruction_valid){
		if(cnc->program.new_instruction.instant_instruction){
         if(cnc->program.instant_instruction.instruction_valid){
            // instant instruction already set... can't overwrite...
            // but we can check to see if this is an abort...
            if(cnc->program.new_instruction.instruction_type == AUX_INSTRUCTION){
               if(cnc->program.new_instruction.instruction.aux.opcode == ABORT_INSTRUCTION){
                  cnc_printf(cnc, "Received Abort");
                  // we just received an abort from an instant instruction... STOP!
                  cnc->program.instant_instruction.instruction_type = AUX_INSTRUCTION;
                  cnc->program.instant_instruction.instruction.aux.opcode = ABORT_INSTRUCTION;
               }
            }
         } else {
            copy_instruction(&cnc->program.new_instruction, &cnc->program.instant_instruction);
         }
		} else {
			cnc->program.instruction_request_sent = 0;
			if(cnc->program.program_running){
				cnc_printf(cnc, "Received Program Instruction!");
				// add to running program
				copy_instruction(&cnc->program.new_instruction, &cnc->program.instruction_array[cnc->program.instruction_wp]);
				cnc->program.instruction_wp = (cnc->program.instruction_wp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->program.instruction_wp + 1 : 0;
				cnc->program.instruction_fullness++;
				cnc->program.new_instruction.instruction_valid = 0;
            if(cnc->program.new_instruction.instruction_type == AUX_INSTRUCTION){
               if(cnc->program.new_instruction.instruction.aux.opcode == PROGRAM_END){
                  cnc->program.program_received = 1;
                  cnc_printf(cnc, "Received Program End!");
               }
            }
			} else {
            if(cnc->program.new_instruction.instruction_type == AUX_INSTRUCTION){
               if(cnc->program.new_instruction.instruction.aux.opcode == PROGRAM_START){
                  cnc_printf(cnc, "Received Program Start!");
                  copy_instruction(&cnc->program.new_instruction, &cnc->program.instruction_array[cnc->program.instruction_wp]);
                  cnc->program.instruction_wp = (cnc->program.instruction_wp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->program.instruction_wp + 1 : 0;
                  cnc->program.instruction_fullness++;
                  cnc->program.new_instruction.instruction_valid = 0;
               } else {
                  // program isn't running and it's not an immediate instruction  or start of program... so ignore...
               }
            }
			}
		}
		cnc->program.new_instruction.instruction_valid = 0;
	} else {
		// not valid instruction
	}
}

void clear_program(struct cnc_state_struct* cnc){
	cnc_printf(cnc, "resetting program information");
	cnc->program.program_running = 0;
	cnc->program.program_received = 0;
	cnc->program.program_length = 0;
	cnc->program.instruction_fullness = 0;
	cnc->program.instruction_wp = 0;
	cnc->program.instruction_rp = 0;
   for(int i=0;i<INSTRUCTION_FIFO_DEPTH;i++){
      clear_instruction(&cnc->program.instruction_array[i]);
   }
   cnc->program.current_instruction = &cnc->program.empty_instruction;
	cnc->program.instruction_request_sent = 0;
	cnc->program.request_instruction = 0;
}

void parse_instruction(struct cnc_state_struct* cnc){
	// convert received spi data to cnc instruction
	string_to_instruction(&cnc->program.new_instruction, &cnc->cnc_read_data[1]);
}

void get_next_instruction(struct cnc_state_struct* cnc){
	if(cnc->program.instant_instruction.instruction_valid){
		// instant instruction ready
		// copy to current
      cnc->program.current_instruction = &cnc->program.instant_instruction;
		//copy_instruction(&cnc->instant_instruction, &);
		//clear_instruction(&cnc->instant_instruction);
	} else {
		if(cnc->program.instruction_fullness > 0){
			// we have an instruction ready to copy over
         cnc->program.current_instruction = &cnc->program.instruction_array[cnc->program.instruction_rp];
			//copy_instruction(&cnc->instruction_array[cnc->instruction_rp], &cnc->current_instruction);
			//cnc->instruction_array[cnc->instruction_rp].instruction_valid = 0;
			cnc->program.instruction_rp = (cnc->program.instruction_rp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->program.instruction_rp + 1 : 0;
			cnc->program.instruction_fullness--;
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
	if(cnc->program.program_running && !cnc->program.program_received){
		if(cnc->program.request_instruction){
			if(cnc->program.instruction_fullness >= (INSTRUCTION_FIFO_DEPTH-2)){
				cnc->program.request_instruction = 0;
			} else {
				if(!cnc->program.instruction_request_sent && (cnc->state == CNC_IDLE)){
               cnc->cnc_write_data[0] = (char) NEW_CNC_INSTRUCTION;
               cnc->cnc_write_length = 1;
               if(cnc_serial.send(cnc->cnc_write_length, cnc->cnc_write_data) > 0){
                  cnc->program.instruction_request_sent = 1;
               }
				}
			}
		} else {
			if(cnc->program.instruction_fullness < (INSTRUCTION_FIFO_DEPTH/2)){
				cnc->program.request_instruction = 1;
				cnc_printf(cnc, "setting instruction read request high");
			} else {
				// we have enough instructions, so do nothing
			}
		}
	} else {
		// don't need to do anything if not running a program
		cnc->program.request_instruction = 0;
	}
}

void process_instruction(struct cnc_state_struct* cnc){
	//handle_instruction_opcodes(cnc, cnc->program.current_instruction);
	//handle_motors(cnc);
	//handle_heaters(cnc);
}

void handle_instructions(struct cnc_state_struct* cnc){
	// check instruction type
   // if heater instruction, see if we are waiting for temp
   // if waiting for temp, see if temp reached for all active heaters... if so, then instruction is done
   // if motor instruction, for each active motor:
   // if finding min or max see if endstop has been hit
   // if not finding min or max see if end position has been reached
   check_instruction(cnc); // check various instructions
	check_instruction_fifo(cnc);
	//process_instruction(cnc);
}

void check_errors(struct cnc_state_struct* cnc){
	// check to see if endstops in both directions (min and max) are hit at once... that's def an error
	// others...
}

void set_instruction(struct cnc_state_struct* cnc){
	if(cnc->program.current_instruction->instruction_valid){
      if(cnc->program.current_instruction->instruction_type == MOTOR_INSTRUCTION){
         for(int i=0;i<NUM_MOTORS;i++){
            set_motor_instruction(cnc, &cnc->program.current_instruction->instruction.motors.motor[i], &cnc->motors->motor[i]);
         }
         cnc->program.current_instruction->instruction.motors.ramp_speed;
         cnc->program.current_instruction->instruction.motors.rotations;
         cnc->program.current_instruction->instruction.motors.speed;
      }
      if(cnc->program.current_instruction->instruction_type == HEATER_INSTRUCTION){
         for(int i=0;i<NUM_HEATERS;i++){
            set_heater_instruction(&cnc->program.current_instruction->instruction.heaters.heater[i], &cnc->heaters->heater[i]);
         }
      }
      if(cnc->program.current_instruction->instruction_type == AUX_INSTRUCTION){
         set_aux_instruction(cnc, &cnc->program.current_instruction->instruction.aux);
      }
	}
}

void set_motor_instruction(struct cnc_state_struct* cnc, struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor){
	if(current_instruction->instruction_valid){
		if(current_instruction->relative_move){
         motor->target = motor->position + current_instruction->end_position;
      } else {
         motor->target = current_instruction->end_position;
      }
	}
}

void set_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater){
	if(current_instruction->instruction_valid){
		heater->reset_heater = 1;
      current_instruction->disable_fan;
      current_instruction->disable_heater;
      current_instruction->enable_fan;
      current_instruction->enable_heater;
		heater->target_temp = current_instruction->target_temp;
		heater->wait_for_temp = current_instruction->wait_for_temp;
		heater->fan_duty = current_instruction->fan_duty;
	}
}

void set_aux_instruction(struct cnc_state_struct* cnc, struct cnc_aux_instruction_struct* current_instruction){
   switch(current_instruction->opcode){
      case EMPTY_OPCODE : {
         // nothing to do
         break;
      }
      case PROGRAM_START : {
         cnc->program.program_received = 0;
         cnc->program.program_running = 1;
         break;
      }
      case PROGRAM_END : {
         cnc->program.program_received = 0;
         cnc->program.program_running = 0;
         clear_program(cnc);
         break;
      }
      case ENABLE_MOTORS : {
         for(int i=0;i<NUM_MOTORS;i++){
            if(cnc->program.current_instruction->instruction.aux.enable_motor[i]){
               enable_motor(&cnc->motors->motor[i]);
            }
         }
         break;
      }
      case DISABLE_MOTORS : {
         for(int i=0;i<NUM_MOTORS;i++){
            if(cnc->program.current_instruction->instruction.aux.disable_motor[i]){
               disable_motor(&cnc->motors->motor[i]);
            }
         }
         break;
      }
      case MOVE_TO_ENDSTOP : {
         for(int i=0;i<NUM_MOTORS;i++){
            if(cnc->program.current_instruction->instruction.aux.min_motor[i]){
               cnc->motors->motor[i].speed = cnc->program.current_instruction->instruction.aux.motor_speed;
               cnc->motors->motor[i].find_zero = 1;
            } else {
               if(cnc->program.current_instruction->instruction.aux.max_motor[i]){
                  cnc->motors->motor[i].speed = cnc->program.current_instruction->instruction.aux.motor_speed;
                  cnc->motors->motor[i].find_max = 1;
               }
            }
         }
         cnc->program.current_instruction->instruction_type = MOTOR_INSTRUCTION;
         for(int i=0;i<NUM_MOTORS;i++){
            cnc->program.current_instruction->instruction.motors.motor[i].instruction_valid = (cnc->motors->motor[i].find_zero || cnc->motors->motor[i].find_max);
         }
         break;
      }
      case SET_POSITION : {
         for(int i=0;i<NUM_MOTORS;i++){
            if(cnc->program.current_instruction->instruction.aux.set_position[i]){
               cnc->motors->motor[i].position = cnc->program.current_instruction->instruction.aux.motor_position[i];
            }
         }
         break;
      }
      default : {
         break;
      }
   }
}

uint8_t check_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor){
	if(current_instruction->instruction_valid){
      if(motor->find_max || motor->find_zero){
         if(motor->find_max && *motor->max_range_flag){
            motor->find_max = 0;
            current_instruction->instruction_valid = 0;
         }
         if(motor->find_zero && *motor->min_range_flag){
            motor->find_zero = 0;
            current_instruction->instruction_valid = 0;
         }
      } else {
         if(abs(motor->position - motor->target) <= MOTOR_PRECISION){
            current_instruction->instruction_valid = 0;
         }
      }
	}
   return current_instruction->instruction_valid;
}

uint8_t check_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater){
	if(current_instruction->instruction_valid){
		if(current_instruction->wait_for_temp){
			if(heater->current_temp == heater->target_temp){
				heater->temp_locked = 1;
				current_instruction->instruction_valid = 0;
			}
		} else {
			current_instruction->instruction_valid = 0;
		}
	}
   return current_instruction->instruction_valid;
}

uint8_t check_aux_instruction(struct cnc_aux_instruction_struct* current_instruction){
   /*switch(current_instruction->opcode){
      case EMPTY_OPCODE : {
         // nothing to do
         break;
      }
      case RETURN_STATUS : {
         cnc->cnc_write_data[0] = (char) GET_CNC_STATUS;
         update_status(cnc);
         cnc->cnc_write_length = status_to_string(&cnc->status, &cnc->cnc_write_data[1]);
         cnc->cnc_write_length = cnc->cnc_write_length + 1;
         if(cnc_serial.send(cnc->cnc_write_length, cnc->cnc_write_data) > 0){
            current_instruction.opcode = EMPTY_OPCODE;
         }
         break;
      }
      case ABORT_INSTRUCTION : {
         if(cnc->program.current_instruction->instruction_type == MOTOR_INSTRUCTION){
            for(int i=0;i<NUM_MOTORS;i++){
               abort_motor_instruction(&cnc->program.current_instruction->instruction.motors.motor[i], &cnc->motors->motor[i]);
            }
         }
         if(cnc->program.current_instruction->instruction_type == HEATER_INSTRUCTION){
            for(int i=0;i<NUM_HEATERS;i++){
               abort_heater_instruction(&cnc->program.current_instruction->instruction.heaters.heater[i], &cnc->heaters->heater[i]);
            }
         }
         if(cnc->program.current_instruction->instruction_type == AUX_INSTRUCTION){
            clear_instruction(current_instruction);
         }
         break;
      }
      default : {
         current_instruction->opcode = EMPTY_OPCODE;
         break;
      }
   }*/
   return 0;
}

void abort_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor){
	if(current_instruction->instruction_valid){
		motor->target = motor->position;
      motor->find_max = 0;
      motor->find_zero = 0;
	}
}

void abort_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater){
	if(current_instruction->instruction_valid){
		heater->target_temp = 0;
		heater->wait_for_temp = 0;
	}
}

void check_instruction(struct cnc_state_struct* cnc){
	if(cnc->program.current_instruction->instruction_valid){
		// if so, check all the various things to see if instruction is done
		// if so, mark instruction as inactive
		// also, if it was last instruction, clear program loaded and program running
      
      uint8_t valid_instruction = 0;
      uint8_t valid;
      if(cnc->program.current_instruction->instruction_type == MOTOR_INSTRUCTION){
         for(int i=0;i<NUM_MOTORS;i++){
             valid = check_motor_instruction(&cnc->program.current_instruction->instruction.motors.motor[i], &cnc->motors->motor[i]);
             valid_instruction = valid_instruction | valid;
         }
      }
      if(cnc->program.current_instruction->instruction_type == HEATER_INSTRUCTION){
         for(int i=0;i<NUM_HEATERS;i++){
            valid = check_heater_instruction(&cnc->program.current_instruction->instruction.heaters.heater[i], &cnc->heaters->heater[i]);
            valid_instruction = valid_instruction | valid;
         }
      }
      if(cnc->program.current_instruction->instruction_type == AUX_INSTRUCTION){
         valid = check_aux_instruction(&cnc->program.current_instruction->instruction.aux);
         valid_instruction = valid_instruction | valid;
      }
		
		/*valid_instruction |= (cnc->program.current_instruction.opcode != EMPTY_OPCODE);
		valid_instruction |= cnc->program.current_instruction.aux.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.extruder_0.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.extruder_1.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.xl_axis.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.yf_axis.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.zl_axis.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.zr_axis.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.heater_0.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.heater_1.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.heater_2.instruction_valid;
		valid_instruction |= cnc->program.current_instruction.heater_3.instruction_valid;
*/
		if(valid_instruction){
			// instruction isn't done
		} else {
         cnc_printf(cnc, "instruction done: %d", cnc->program.current_instruction->instruction_type);
         cnc->program.current_instruction->instruction_valid = 0;
			// if(cnc->program.current_instruction.instant_instruction){
			// 	cnc->marker_set = 1;
			// }
			clear_instruction(cnc->program.current_instruction);
         cnc->program.current_instruction = &cnc->program.empty_instruction;
		}
	} else {
      // instruction finished, so get new instruction
		get_next_instruction(cnc);
      set_instruction(cnc);
   }
}

void copy_instruction(struct cnc_instruction_struct* new_instruction, struct cnc_instruction_struct* current_instruction){
   current_instruction->instant_instruction = new_instruction->instant_instruction;
   current_instruction->instruction_type = new_instruction->instruction_type;
   current_instruction->instruction_valid = new_instruction->instruction_valid;
   if(current_instruction->instruction_type == MOTOR_INSTRUCTION){
      current_instruction->instruction.motors.ramp_speed = new_instruction->instruction.motors.ramp_speed;
      current_instruction->instruction.motors.rotations = new_instruction->instruction.motors.rotations;
      current_instruction->instruction.motors.speed = new_instruction->instruction.motors.speed;
      for(int i=0;i<NUM_MOTORS;i++){
         current_instruction->instruction.motors.motor[i].arc_move = new_instruction->instruction.motors.motor[i].arc_move;
         current_instruction->instruction.motors.motor[i].relative_move = new_instruction->instruction.motors.motor[i].relative_move;
         current_instruction->instruction.motors.motor[i].end_position = new_instruction->instruction.motors.motor[i].end_position;
         current_instruction->instruction.motors.motor[i].instruction_valid = new_instruction->instruction.motors.motor[i].instruction_valid;
         current_instruction->instruction.motors.motor[i].ramp_down = new_instruction->instruction.motors.motor[i].ramp_down;
         current_instruction->instruction.motors.motor[i].ramp_up = new_instruction->instruction.motors.motor[i].ramp_up;
      }
   }
   if(current_instruction->instruction_type == HEATER_INSTRUCTION){
      for(int i=0;i<NUM_HEATERS;i++){
         current_instruction->instruction.heaters.heater[i].disable_fan = new_instruction->instruction.heaters.heater[i].disable_fan;
         current_instruction->instruction.heaters.heater[i].disable_heater = new_instruction->instruction.heaters.heater[i].disable_heater;
         current_instruction->instruction.heaters.heater[i].enable_fan = new_instruction->instruction.heaters.heater[i].enable_fan;
         current_instruction->instruction.heaters.heater[i].enable_heater = new_instruction->instruction.heaters.heater[i].enable_heater;
         current_instruction->instruction.heaters.heater[i].fan_duty = new_instruction->instruction.heaters.heater[i].fan_duty;
         current_instruction->instruction.heaters.heater[i].instruction_valid = new_instruction->instruction.heaters.heater[i].instruction_valid;
         current_instruction->instruction.heaters.heater[i].target_temp = new_instruction->instruction.heaters.heater[i].target_temp;
         current_instruction->instruction.heaters.heater[i].wait_for_temp = new_instruction->instruction.heaters.heater[i].wait_for_temp;
      }
   }
   if(current_instruction->instruction_type == AUX_INSTRUCTION){
      current_instruction->instruction.aux.opcode = new_instruction->instruction.aux.opcode;
      for(int i=0;i<NUM_MOTORS;i++){
         current_instruction->instruction.aux.disable_motor[i] = new_instruction->instruction.aux.disable_motor[i];
         current_instruction->instruction.aux.enable_motor[i] = new_instruction->instruction.aux.enable_motor[i];
         current_instruction->instruction.aux.max_motor[i] = new_instruction->instruction.aux.max_motor[i];
         current_instruction->instruction.aux.min_motor[i] = new_instruction->instruction.aux.min_motor[i];
         current_instruction->instruction.aux.set_position[i] = new_instruction->instruction.aux.set_position[i];
         current_instruction->instruction.aux.motor_position[i] = new_instruction->instruction.aux.motor_position[i];
      }
   }
}

