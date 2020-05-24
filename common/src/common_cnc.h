/*
 * common_cnc.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#ifndef SRC_COMMON_CNC_H_
#define SRC_COMMON_CNC_H_

#include <stdint.h>

#define PRINT_DEPTH 16
#define MAX_PRINT_LENGTH 64
#define INSTRUCTION_BYTE_LENGTH sizeof(cnc_instruction_struct)
#define MAX_COMM_TRANSFER ((INSTRUCTION_BYTE_LENGTH > MAX_PRINT_LENGTH) ? INSTRUCTION_BYTE_LENGTH + 1 : MAX_PRINT_LENGTH + 1)

#define RAMP_PERIOD 0 // start/end period of ramp... should be slow
#define cnc_double float

#define MINUTE_TO_SECOND ((cnc_double) 60.0)
#define STEPS_PER_SECOND (((cnc_double) MOTOR_TIMER_FREQ)/MOTOR_TOP_COUNT)
#define STEPS_PER_MINUTE (((cnc_double) STEPS_PER_SECOND)*60)

#define MOTOR_STEPS_PER_REV (cnc_double) 200
#define MOTOR_STEP_FACTOR (((cnc_double) 1)/8)
#define STEPS_PER_REV (((cnc_double) MOTOR_STEPS_PER_REV)/MOTOR_STEP_FACTOR)
#define LEADSCREW_MM_PER_REV ((cnc_double) 8)
#define REVS_PER_MM (((cnc_double) 1)/LEADSCREW_MM_PER_REV)
#define STEPS_PER_MM (((cnc_double) STEPS_PER_REV)*REVS_PER_MM)

#define IN_TO_MM ((cnc_double) 25.4)

#define STEP_MM (LEADSCREW_MM_PER_REV / STEPS_PER_REV)
#define PRECISION STEP_MM

#define USEC_PER_SEC ((cnc_double) (1000000))

#define RAPID_MOVE_SPEED 600.0

#define NUM_MOTORS 7
enum MOTOR_NUMBERS {
   MOTOR_AUX,
   MOTOR_EXTRUDER_0,
   MOTOR_EXTRUDER_1,
   MOTOR_AXIS_XL,
   MOTOR_AXIS_YF,
   MOTOR_AXIS_ZL,
   MOTOR_AXIS_ZR
};

#define NUM_HEATERS 4
enum HEATER_NUMBERS {
   HEATER_EXTRUTER_0,
   HEATER_EXTRUTER_1,
   HEATER_BED,
   HEATER_OTHER
};

#define NUM_ENDSTOPS 12
enum ENDSTOP_NUMBERS {
   X_L_MIN,
   X_R_MAX,
   Y_F_MIN,
   Y_B_MAX,
   Z_L_MIN,
   Z_L_MAX,
   Z_R_MIN,
   Z_R_MAX,
   EXTRA_0,
   EXTRA_1,
   EXTRA_2,
   EXTRA_3
};

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
   INSTANT_DONE,
	ERROR
};

enum INSTRUCTION_TYPES {
   MOTOR_INSTRUCTION,
   HEATER_INSTRUCTION,
   AUX_INSTRUCTION
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
	SET_POSITION,
	RETURN_STATUS,
	MOVE_TO_ENDSTOP,
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
	0, /* cnc_double max_speed; */ \
	0, /* cnc_double min_speed; */ \
	0, /* uint32_t ramp_period; */ \
	0, /* int64_t xl_min_safe_pos; */ \
	0, /* int64_t xr_max_safe_pos; */ \
	0, /* int64_t yf_min_safe_pos; */ \
	0, /* int64_t yb_max_safe_pos; */ \
	0, /* int64_t zl_min_safe_pos; */ \
	0, /* int64_t zl_max_safe_pos; */ \
	0, /* int64_t zr_min_safe_pos; */ \
	0, /* int64_t rr_max_safe_pos; */ \
	0, /* int64_t xl_min_home_pos; */ \
	0, /* int64_t xr_max_home_pos; */ \
	0, /* int64_t yf_min_home_pos; */ \
	0, /* int64_t yb_max_home_pos; */ \
	0, /* int64_t zl_min_home_pos; */ \
	0, /* int64_t zl_max_home_pos; */ \
	0, /* int64_t zr_min_home_pos; */ \
	0, /* int64_t zr_max_home_pos; */ \
	0, /* uint64_t x_axis_size; */ \
	0, /* uint64_t y_axis_size; */ \
	0, /* uint64_t zl_axis_size; */ \
	0  /* uint64_t zr_axis_size; */ \
};

struct cnc_config_struct {
	uint8_t config_loaded;
	uint8_t valid_config;
	cnc_double max_speed;
	cnc_double min_speed;
	cnc_double ramp_speed;
   cnc_double safe_position[NUM_MOTORS];
   cnc_double home_position[NUM_MOTORS];
   cnc_double axis_size[NUM_MOTORS];
};

struct cnc_status_struct {
   uint8_t endstop_status[NUM_ENDSTOPS];
   cnc_double position[NUM_MOTORS];
   cnc_double temp[NUM_HEATERS];
   uint8_t temp_locked[NUM_HEATERS];
};

struct cnc_motor_instruction_struct {
	uint8_t instruction_valid; // flag saying whether instruction is valid or stale
   uint8_t relative_move;
   uint8_t arc_move;
   uint8_t ramp_up;
   uint8_t ramp_down;
	cnc_double end_position;
#ifdef INTERFACE
	cnc_double move_position;
	cnc_double feed_rate;
#endif
};

struct cnc_motor_instructions {
   uint32_t rotations;
   cnc_double speed;
   cnc_double ramp_speed;
   struct cnc_motor_instruction_struct motor[NUM_MOTORS];
};

struct cnc_heater_instruction_struct {
	uint8_t instruction_valid;
   uint8_t wait_for_temp;
   uint8_t fan_duty;
   uint8_t enable_heater;
   uint8_t disable_heater;
   uint8_t enable_fan;
   uint8_t disable_fan;
   cnc_double target_temp;
};

struct cnc_heater_instructions {
   struct cnc_heater_instruction_struct heater[NUM_HEATERS];
};

struct cnc_aux_instruction_struct {
   enum INSTRUCTION_OPCODE opcode; // maps to instruction opcode enum
   uint8_t enable_motor[NUM_MOTORS];
   uint8_t disable_motor[NUM_MOTORS];
   uint8_t set_position[NUM_MOTORS];
   uint8_t min_motor[NUM_MOTORS];
   uint8_t max_motor[NUM_MOTORS];
   cnc_double motor_position[NUM_MOTORS];
   cnc_double motor_speed;
};

union cnc_instruction_union {
   struct cnc_motor_instructions motors;
   struct cnc_heater_instructions heaters;
   struct cnc_aux_instruction_struct aux;
};

struct cnc_instruction_struct {
   enum INSTRUCTION_TYPES instruction_type;
	uint8_t instruction_valid;
	uint8_t instant_instruction;
   union cnc_instruction_union instruction;
#ifdef INTERFACE
   cnc_double instruction_number;
   cnc_double speed;
   uint8_t comment_flag;
   char comment[256];
   uint8_t message_flag;
   char message[256];
#endif
};

uint16_t instruction_to_string(struct cnc_instruction_struct* instruction, uint8_t* string);
void string_to_instruction(struct cnc_instruction_struct* instruction, uint8_t* string);

void motor_instruction_to_string(struct cnc_motor_instruction_struct* instruction, uint8_t* string, uint16_t* offset);
void heater_instruction_to_string(struct cnc_heater_instruction_struct* instruction, uint8_t* string, uint16_t* offset);

void string_to_motor_instruction(struct cnc_motor_instruction_struct* instruction, uint8_t* string, uint16_t* offset);
void string_to_heater_instruction(struct cnc_heater_instruction_struct* instruction, uint8_t* string, uint16_t* offset);

uint16_t config_to_string(struct cnc_config_struct* config, uint8_t* string);
void string_to_config(struct cnc_config_struct* config, uint8_t* string);

uint16_t status_to_string(struct cnc_status_struct* status, uint8_t* string);
void string_to_status(struct cnc_status_struct* status, uint8_t* string);

void clear_instruction(struct cnc_instruction_struct* instruction);
void clear_motor_instruction(struct cnc_motor_instruction_struct* instruction);
void clear_heater_instruction(struct cnc_heater_instruction_struct* instruction);

#endif /* SRC_COMMON_CNC_H_ */
