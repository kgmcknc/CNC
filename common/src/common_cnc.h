/*
 * common_cnc.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#ifndef SRC_COMMON_CNC_H_
#define SRC_COMMON_CNC_H_

#include <stdint.h>

#define MOTOR_TIMER_CLOCK (double) 48000000
#define MOTOR_TOP_COUNT (double) 1000
#define STEPS_PER_SECOND (((double) MOTOR_TIMER_CLOCK)/MOTOR_TOP_COUNT)
#define STEPS_PER_MINUTE (((double) STEPS_PER_SECOND)*60)
#define MOTOR_STEPS_PER_REV (double) 200
#define MOTOR_STEP_FACTOR (((double) 1)/8)
#define STEPS_PER_REV (((double) MOTOR_STEPS_PER_REV)/MOTOR_STEP_FACTOR)
#define REVS_PER_MM (((double) 1)/8)
#define REVS_PER_IN (((double) 25.4)/8)
#define STEPS_PER_MM (((double) STEPS_PER_REV)*REVS_PER_MM)
#define STEPS_PER_IN (((double) STEPS_PER_REV)*REVS_PER_IN)

#define MOVE_PERIOD 4

enum CNC_OPCODES {
	GET_CNC_VERSION,
	GET_CNC_STATUS,
	SET_CNC_CONFIG,
	NEW_CNC_PRINT,
	FLASH_FIRMWARE,
	NEW_CNC_INSTRUCTION,
	INSTANT_CNC_INSTRUCTION,
	DISABLE_ROUTE,
	ENABLE_ROUTE,
	PAUSE_PROGRAM,
	RESUME_PROGRAM,
	MARKER,
	ERROR
};

enum INSTRUCTION_OPCODE {
	// change these some
	EMPTY_OPCODE,
	IDLE_OPCODE,
	PROGRAM_IDLE,
	PROGRAM_START,
	PROGRAM_PAUSE,
	PROGRAM_RESUME,
	PROGRAM_END,
	PROGRAM_RESET,
	SYSTEM_CHECK,
	INSTRUCTION,
	CHECK_ENDSTOPS,
	HOME_AXIS,
	MEASURE_AXIS,
	POSITION_AXIS,
	RETURN_STATUS,
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

#define CONFIG_DEFAULT  {\
    0, /* uint8_t config_loaded; */ \
	0, /* uint8_t valid_config; */ \
	0, /* double max_speed; */ \
	0, /* double min_speed; */ \
	0, /* uint64_t xl_min_safe_pos; */ \
	0, /* uint64_t xr_max_safe_pos; */ \
	0, /* uint64_t yf_min_safe_pos; */ \
	0, /* uint64_t yb_max_safe_pos; */ \
	0, /* uint64_t zl_min_safe_pos; */ \
	0, /* uint64_t zl_max_safe_pos; */ \
	0, /* uint64_t zr_min_safe_pos; */ \
	0, /* uint64_t rr_max_safe_pos; */ \
	0, /* uint64_t xl_min_home_pos; */ \
	0, /* uint64_t xr_max_home_pos; */ \
	0, /* uint64_t yf_min_home_pos; */ \
	0, /* uint64_t yb_max_home_pos; */ \
	0, /* uint64_t zl_min_home_pos; */ \
	0, /* uint64_t zl_max_home_pos; */ \
	0, /* uint64_t zr_min_home_pos; */ \
	0, /* uint64_t zr_max_home_pos; */ \
	0, /* uint64_t x_axis_size; */ \
	0, /* uint64_t y_axis_size; */ \
	0  /* uint64_t z_axis_size; */ \
};

struct cnc_config_struct {
	uint8_t config_loaded;
	uint8_t valid_config;
	double max_speed;
	double min_speed;
	uint64_t xl_min_safe_pos;
	uint64_t xr_max_safe_pos;
	uint64_t yf_min_safe_pos;
	uint64_t yb_max_safe_pos;
	uint64_t zl_min_safe_pos;
	uint64_t zl_max_safe_pos;
	uint64_t zr_min_safe_pos;
	uint64_t zr_max_safe_pos;
	uint64_t xl_min_home_pos;
	uint64_t xr_max_home_pos;
	uint64_t yf_min_home_pos;
	uint64_t yb_max_home_pos;
	uint64_t zl_min_home_pos;
	uint64_t zl_max_home_pos;
	uint64_t zr_min_home_pos;
	uint64_t zr_max_home_pos;
	uint64_t x_axis_size;
	uint64_t y_axis_size;
	uint64_t zl_axis_size;
	uint64_t zr_axis_size;
};

struct cnc_status_struct {
	uint8_t xl_min_flag;
	uint8_t xl_max_flag;
	uint8_t yf_min_flag;
	uint8_t yf_max_flag;
	uint8_t zl_min_flag;
	uint8_t zl_max_flag;
	uint8_t zr_min_flag;
	uint8_t zr_max_flag;
	int64_t ex0_position;
	int64_t ex1_position;
	int64_t aux_position;
	int64_t xl_position;
	int64_t yf_position;
	int64_t zl_position;
	int64_t zr_position;
	double heater_0_temp;
	double heater_1_temp;
	double heater_2_temp;
	double heater_3_temp;
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

uint16_t config_to_string(struct cnc_config_struct* config, char* string);
void string_to_config(struct cnc_config_struct* config, char* string);

uint16_t status_to_string(struct cnc_status_struct* status, char* string);
void string_to_status(struct cnc_status_struct* status, char* string);

void clear_instruction(struct cnc_instruction_struct* instruction);
void clear_motor_instruction(struct cnc_motor_instruction_struct* instruction);
void clear_heater_instruction(struct cnc_heater_instruction_struct* instruction);

#endif /* SRC_COMMON_CNC_H_ */
