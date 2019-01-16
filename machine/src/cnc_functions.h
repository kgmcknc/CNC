/*
 * cnc_functions.h
 *
 *  Created on: Aug 21, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_FUNCTIONS_H_
#define SRC_CNC_FUNCTIONS_H_

#include "cnc_instructions.h"
#include "cnc_motors.h"

typedef enum {
	cnc_idle,
	cnc_program_running
} cnc_state;

struct cnc_state_struct {
	cnc_state state = cnc_idle;
	uint8_t start_program;
	uint8_t end_of_program_read;
	uint8_t abort_program;
	uint8_t request_instruction;
	uint8_t request_print;
	struct cnc_instruction_struct current_instruction;
	struct cnc_motor_list_struct* motors;
	struct cnc_heater_list_struct* heaters;
	struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];
	uint16_t instruction_rp;
	uint16_t instruction_wp;
	uint16_t instruction_fullness;
	char print_buffer[PRINT_DEPTH][PRINT_LENGTH];
	uint8_t print_rp;
	uint8_t print_wp;
	uint8_t print_fullness;
};

typedef enum {
	start_program,
	pause_program,
	resume_program,
	end_program,
	new_instruction,
	get_status,
	error
} cnc_opcodes;

extern struct cnc_state_struct cnc;

void init_instructions(struct cnc_state_struct* cnc);
void handle_instructions(struct cnc_state_struct* cnc);
void handle_program(struct cnc_state_struct* cnc);
void init_cnc(struct cnc_state_struct* cnc);
void handle_state(struct cnc_state_struct* cnc);
void cnc_printf(struct cnc_state_struct* cnc, const char* print_string, ...);
//void start_program();
//void end_program();

#endif /* SRC_CNC_FUNCTIONS_H_ */
