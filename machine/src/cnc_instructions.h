/*
 * cnc_instructions.h
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifndef CNC_INSTRUCTIONS_H_
#define CNC_INSTRUCTIONS_H_

#include "stdint.h"
#include "common_spi.h"
#include "cnc_functions.h"
#include "cnc_heaters.h"
#include "cnc_pid.h"
#include "common_cnc.h"

#define INSTRUCTION_FIFO_DEPTH 500
#define PRINT_DEPTH 16
#define MAX_PRINT_LENGTH 2048

extern struct cnc_instruction_struct cnc_instruction, cnc_instruction_next;
extern struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];
extern uint32_t instruction_array_fullness, instruction_array_wp, instruction_array_rp;

void receive_instruction(struct cnc_state_struct* cnc);
void parse_instruction(struct cnc_state_struct* cnc);
void get_next_instruction(struct cnc_state_struct* cnc);
void check_instruction_fifo(struct cnc_state_struct* cnc);
void handle_instructions(struct cnc_state_struct* cnc);
void set_instruction(struct cnc_state_struct* cnc);
void set_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor);
void set_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater);
void clear_program(struct cnc_state_struct* cnc);
void handle_program(struct cnc_state_struct* cnc);
void process_instruction(struct cnc_state_struct* cnc);
void init_instructions(struct cnc_state_struct* cnc);
void copy_instruction(struct cnc_instruction_struct* new_instruction, struct cnc_instruction_struct* current_instruction);
void check_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor);
void check_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater);
void abort_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor);
void abort_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater);
void check_instruction(struct cnc_state_struct* cnc);
void check_errors(struct cnc_state_struct* cnc);
void copy_motor_instruction(struct cnc_motor_instruction_struct* new_instruction, struct cnc_motor_instruction_struct* current_instruction);
void copy_heater_instruction(struct cnc_heater_instruction_struct* new_instruction, struct cnc_heater_instruction_struct* current_instruction);
void handle_instruction_opcodes(struct cnc_state_struct* cnc, struct cnc_instruction_struct* instruction);
void handle_motor_opcode(struct cnc_state_struct* cnc, struct cnc_motor_instruction_struct* instruction, struct cnc_motor_struct* motor);
void handle_heater_opcode(struct cnc_state_struct* cnc, struct cnc_heater_instruction_struct* instruction, struct cnc_heater_struct* heater);

#endif /* CNC_INSTRUCTIONS_H_ */
