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

#define INSTRUCTION_FIFO_DEPTH 500
#define PRINT_DEPTH 16

typedef enum {
	PROGRAM_IDLE,
	PROGRAM_START,
	PROGRAM_PAUSE,
	PROGRAM_RESUME,
	PROGRAM_END,
	PROGRAM_RESET,
	SYSTEM_CHECK,
	INSTRUCTION,
	CHECK_ENDSTOPS,
	ZERO_MOTORS,
	MAX_MOTORS,
	ENABLE_MOTORS,
	DISABLE_MOTORS,
	ENABLE_EXTRUDERS,
	DISABLE_EXTRUDERS,
	ENABLE_HEATERS,
	DISABLE_HEATERS,
	ENABLE_FANS,
	DISABLE_FANS,
	ENABLE_TOOLS,
	DISABLE_TOOLS
} INSTRUCTION_OPCODE;

struct cnc_motor_instruction_struct {
	uint8_t valid_instruction; // flag saying whether instruction is valid or stale
	uint32_t current_position;
	uint32_t end_position;
	uint32_t move_count;
	uint32_t step_period;
	uint32_t current_period;
	uint8_t ramp_up_speed;
	uint8_t ramp_down_speed;
	uint8_t find_zero;
	uint8_t find_max;
};

struct cnc_instruction_struct {
	uint8_t instruction_valid;
	uint64_t instruction_number;
	uint64_t program_length; // number of instructions for current program
	INSTRUCTION_OPCODE opcode_flags; // maps to instruction opcode enum
	char pending_motor_enables[7];
	char pending_motor_disables[7];
	char pending_heater_enables[4];
	char pending_heater_disables[4];
	struct cnc_motor_instruction_struct aux;
	struct cnc_motor_instruction_struct extruder_0;
	struct cnc_motor_instruction_struct extruder_1;
	struct cnc_motor_instruction_struct xl_axis;
	struct cnc_motor_instruction_struct yf_axis;
	struct cnc_motor_instruction_struct zl_axis;
	struct cnc_motor_instruction_struct zr_axis;
	struct cnc_heater_instruction_struct heater_0;
	struct cnc_heater_instruction_struct heater_1;
	struct cnc_heater_instruction_struct heater_2;
	struct cnc_heater_instruction_struct heater_3;
};

extern struct cnc_instruction_struct cnc_instruction, cnc_instruction_next;
extern struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];
extern uint32_t instruction_array_fullness, instruction_array_wp, instruction_array_rp;

void clear_instruction(struct cnc_instruction_struct* instruction);
void copy_instruction(struct cnc_instruction_struct* new_instruction, struct cnc_instruction_struct* current_instruction);
void check_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor);
void check_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater);
void check_instruction(struct cnc_instruction_struct* current_instruction);
void copy_motor_instruction(struct cnc_motor_instruction_struct* new_instruction, struct cnc_motor_instruction_struct* current_instruction);
void copy_heater_instruction(struct cnc_heater_instruction_struct* new_instruction, struct cnc_heater_instruction_struct* current_instruction);

#endif /* CNC_INSTRUCTIONS_H_ */
