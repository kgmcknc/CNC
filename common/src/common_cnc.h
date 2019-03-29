/*
 * common_cnc.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#ifndef SRC_COMMON_CNC_H_
#define SRC_COMMON_CNC_H_

#include <stdint.h>

#define MOTOR_TIMER_CLOCK 48000000
#define MOTOR_TOP_COUNT 1000
#define MOTOR_STEPS_PER_REV 200
#define MOTOR_STEP_FACTOR (1/8)
#define STEPS_PER_REV (MOTOR_STEPS_PER_REF/MOTOR_STEP_FACTOR)

enum CNC_OPCODES {
	GET_CNC_VERSION,
	GET_CNC_STATUS,
	NEW_CNC_PRINT,
	FLASH_FIRMWARE,
	NEW_CNC_INSTRUCTION,
	INSTANT_CNC_INSTRUCTION,
	DISABLE_ROUTE,
	ENABLE_ROUTE,
	PAUSE_PROGRAM,
	RESUME_PROGRAM,
	ERROR
};

enum INSTRUCTION_OPCODE {
	// change these some
	EMPTY_OPCODE,
	PROGRAM_IDLE,
	PROGRAM_START,
	PROGRAM_PAUSE,
	PROGRAM_RESUME,
	PROGRAM_END,
	PROGRAM_RESET,
	SYSTEM_CHECK,
	INSTRUCTION,
	CHECK_ENDSTOPS,
	ZERO_MOTOR,
	MAX_MOTOR,
	ENABLE_MOTORS,
	DISABLE_MOTORS,
	ENABLE_EXTRUDERS,
	DISABLE_EXTRUDERS,
	ENABLE_HEATERS,
	DISABLE_HEATERS,
	ENABLE_FANS,
	DISABLE_FANS,
	ENABLE_TOOLS,
	DISABLE_TOOLS,
	ABORT_INSTRUCTION
};

struct cnc_motor_instruction_struct {
	uint8_t instruction_valid; // flag saying whether instruction is valid or stale
	enum INSTRUCTION_OPCODE opcode;
	uint8_t pending_enable;
	uint8_t pending_disable;
	int64_t current_position;
#ifdef INTERFACE
	double move_position;
	double feed_rate;
#endif
	int32_t move_count;
	uint32_t current_period;
	uint8_t ramp_up_speed;
	uint8_t ramp_down_speed;
	uint8_t find_zero;
	uint8_t find_max;
	uint8_t linear_move;
	uint8_t arc_move;
};

struct cnc_heater_instruction_struct {
	uint8_t instruction_valid;
	enum INSTRUCTION_OPCODE opcode;
	uint8_t pending_enable;
	uint8_t pending_disable;
	uint8_t enabled;
	uint8_t heater_on;
	uint8_t heater_active;
	uint8_t wait_for_temp;
	uint8_t fan_duty;
	uint8_t fan_on;
	uint8_t temp_locked;
	double target_temp;
	double current_temp;
};

struct cnc_instruction_struct {
	uint8_t instruction_valid;
	uint8_t instant_instruction;
	enum INSTRUCTION_OPCODE opcode; // maps to instruction opcode enum
	uint8_t set_position_flag;
	uint8_t instruction_set;
#ifdef INTERFACE
    double instruction_number;
    double speed;
    uint8_t comment_flag;
    char comment[256];
    uint8_t message_flag;
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

uint16_t instruction_to_string(struct cnc_instruction_struct* instruction, char* string);
void string_to_instruction(struct cnc_instruction_struct* instruction, char* string);

void motor_instruction_to_string(struct cnc_motor_instruction_struct* instruction, char* string, uint16_t* offset);
void heater_instruction_to_string(struct cnc_heater_instruction_struct* instruction, char* string, uint16_t* offset);

void string_to_motor_instruction(struct cnc_motor_instruction_struct* instruction, char* string, uint16_t* offset);
void string_to_heater_instruction(struct cnc_heater_instruction_struct* instruction, char* string, uint16_t* offset);

void clear_instruction(struct cnc_instruction_struct* instruction);
void clear_motor_instruction(struct cnc_motor_instruction_struct* instruction);
void clear_heater_instruction(struct cnc_heater_instruction_struct* instruction);

#endif /* SRC_COMMON_CNC_H_ */
