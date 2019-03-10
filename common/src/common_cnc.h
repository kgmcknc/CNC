/*
 * common_cnc.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#ifndef SRC_COMMON_CNC_H_
#define SRC_COMMON_CNC_H_

#include <stdint.h>

enum INSTRUCTION_OPCODE {
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
};

struct cnc_motor_instruction_struct {
	uint8_t valid_instruction; // flag saying whether instruction is valid or stale
	uint32_t current_position;
#ifdef INTERFACE
	double move_position;
	double feed_rate;
#endif
	uint32_t end_position;
	uint32_t move_count;
	uint32_t step_period;
	uint32_t current_period;
	uint8_t ramp_up_speed;
	uint8_t ramp_down_speed;
	uint8_t find_zero;
	uint8_t find_max;
	uint8_t axis_active;
	uint8_t linear_move;
	uint8_t arc_move;
};

struct cnc_heater_instruction_struct {
	uint8_t active;
	uint8_t enabled;
	uint8_t valid_instruction;
	uint8_t heater_on;
	uint8_t heater_active;
	uint8_t wait_for_temp;
	uint8_t fan_duty;
	uint8_t fan_on;
	uint8_t temp_locked;
	double target_temp;
	double current_temp;
};

enum cnc_opcodes {
	cnc_init,
	reconnect_spi,
	read_version,
	get_cnc_status,
	new_cnc_print,
	flash_firmware,
	start_cnc_program,
	end_cnc_program,
	new_cnc_instruction,
	instant_cnc_instruction,
	disable_route,
	enable_route,
	start_program,
	pause_program,
	resume_program,
	end_program,
	new_instruction,
	get_status,
	error
};

struct cnc_instruction_struct {
	uint8_t instruction_valid;
	double instruction_number;
	uint64_t program_length; // number of instructions for current program
	enum INSTRUCTION_OPCODE opcode_flags; // maps to instruction opcode enum
	char pending_motor_enables[7];
	char pending_motor_disables[7];
	char pending_heater_enables[4];
	char pending_heater_disables[4];
	double speed;
	char set_position_flag;
    char instruction_set;
#ifdef INTERFACE
    char comment_flag;
    char comment[256];
    char message_flag;
    char message[256];
#endif
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

#endif /* SRC_COMMON_CNC_H_ */
