/*
 * cnc_functions.h
 *
 *  Created on: Aug 21, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_FUNCTIONS_H_
#define SRC_CNC_FUNCTIONS_H_

#include "cnc_instructions.h"
#include "common_cnc.h"
#include "cnc_motors.h"

enum cnc_state {
	CNC_IDLE,
	PROCESS_SPI,
	GET_STATUS,
	GET_VERSION,
	SEND_CNC_PRINT
};

struct cnc_state_struct {
	cnc_state state = CNC_IDLE;
	uint8_t program_running;
	uint8_t program_received; // finished loading the whole program
	uint64_t program_length; // number of instructions for current program
	uint8_t request_instruction;
	// cnc_spi variables
	char cnc_read_data[MAX_SPI_TRANSFER];
	int32_t cnc_read_length;
	uint8_t read_in_progress;
	uint8_t read_complete;
	char cnc_write_data[MAX_SPI_TRANSFER];
	int32_t cnc_write_length;
	uint8_t write_in_progress;
	uint8_t write_complete;
	// cnc_instruction variabless
	struct cnc_motor_list_struct* motors;
	struct cnc_heater_list_struct* heaters;
	struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];
	struct cnc_instruction_struct current_instruction;
	struct cnc_instruction_struct instant_instruction;
	struct cnc_instruction_struct new_instruction;
	uint8_t instruction_set;
	uint8_t instruction_finished;
	uint16_t instruction_rp;
	uint16_t instruction_wp;
	uint16_t instruction_fullness;
	// cnc_printf variables
	char print_buffer[PRINT_DEPTH][MAX_PRINT_LENGTH];
	uint8_t print_rp;
	uint8_t print_wp;
	uint8_t print_fullness;
};

extern struct cnc_state_struct cnc;

void process_spi_request(struct cnc_state_struct* cnc);
void init_cnc(struct cnc_state_struct* cnc);
void handle_state(struct cnc_state_struct* cnc);
void cnc_printf(struct cnc_state_struct* cnc, const char* print_string, ...);
void parse_status(struct cnc_state_struct* cnc);
void parse_print(struct cnc_state_struct* cnc);
void parse_version(struct cnc_state_struct* cnc);

#endif /* SRC_CNC_FUNCTIONS_H_ */
