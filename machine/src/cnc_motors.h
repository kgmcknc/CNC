/*
 * cnc_motors.h
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_MOTORS_H_
#define SRC_CNC_MOTORS_H_

#include "stdint.h"
#include "rtcdriver.h"
#include "cnc_gpio.h"
#include "cnc_functions.h"
#include "string.h"
#include "stdarg.h"

#define MAX_MOTOR_NAME_LENGTH 8

typedef enum {
	AUX_MOTOR,          // extra...
	EXTRUDER_0_MOTOR,   // 3d printer 0
	EXTRUDER_1_MOTOR,   // 3d printer 1
	X_AXIS_L_MOTOR,     // left right direction
	Y_AXIS_F_MOTOR,     // front back direction
	Z_AXIS_L_MOTOR,     // up down
	Z_AXIS_R_MOTOR      // up down
} CNC_MOTOR_SELECTIONS;

struct cnc_motor_port_struct {
	GPIO_Port_TypeDef en_port;
	GPIO_Port_TypeDef step_port;
	GPIO_Port_TypeDef dir_port;
	GPIO_Port_TypeDef ms0_port;
	GPIO_Port_TypeDef ms1_port;
	GPIO_Port_TypeDef min_es_port;
	GPIO_Port_TypeDef max_es_port;
};

struct cnc_motor_pin_struct {
	uint8_t en_pin;
	uint8_t step_pin;
	uint8_t dir_pin;
	uint8_t ms0_pin;
	uint8_t ms1_pin;
	uint8_t min_es_pin;
	uint8_t max_es_pin;
};

struct cnc_motor_struct {
	uint8_t enabled;
	uint8_t homed;
	uint8_t find_zero;
	uint8_t find_max;
	uint8_t set;
	uint8_t step_high;
	uint8_t min_range_flag;
	uint8_t max_range_flag;
	int8_t direction; // -1 , 0, 1
	int64_t position;
	int64_t axis_length;
	int64_t safe_position;
	int64_t home_position;
	int32_t ramp_count;
	int32_t move_count;
	uint32_t period;
	uint32_t current_period;
	uint32_t step_timer;
	char name[MAX_MOTOR_NAME_LENGTH] = {};
	struct cnc_motor_port_struct ports;
	struct cnc_motor_pin_struct pins;
};

struct cnc_motor_list_struct {
	struct cnc_motor_struct aux;
	struct cnc_motor_struct extruder_0;
	struct cnc_motor_struct extruder_1;
	struct cnc_motor_struct xl_axis;
	struct cnc_motor_struct yf_axis;
	struct cnc_motor_struct zl_axis;
	struct cnc_motor_struct zr_axis;
};

#define XL_AXIS_MOTOR_EN_PORT       MOTOR_0_ENABLE_PORT
#define XL_AXIS_MOTOR_STEP_PORT     MOTOR_0_STEP_PORT
#define XL_AXIS_MOTOR_DIR_PORT      MOTOR_0_DIR_PORT
#define XL_AXIS_MOTOR_MS0_PORT      MOTOR_0_MS0_PORT
#define XL_AXIS_MOTOR_MS1_PORT      MOTOR_0_MS1_PORT
#define XL_AXIS_MOTOR_EN_PIN        MOTOR_0_ENABLE_PIN
#define XL_AXIS_MOTOR_STEP_PIN      MOTOR_0_STEP_PIN
#define XL_AXIS_MOTOR_DIR_PIN       MOTOR_0_DIR_PIN
#define XL_AXIS_MOTOR_MS0_PIN       MOTOR_0_MS0_PIN
#define XL_AXIS_MOTOR_MS1_PIN       MOTOR_0_MS1_PIN

#define YF_AXIS_MOTOR_EN_PORT       MOTOR_1_ENABLE_PORT
#define YF_AXIS_MOTOR_STEP_PORT     MOTOR_1_STEP_PORT
#define YF_AXIS_MOTOR_DIR_PORT      MOTOR_1_DIR_PORT
#define YF_AXIS_MOTOR_MS0_PORT      MOTOR_1_MS0_PORT
#define YF_AXIS_MOTOR_MS1_PORT      MOTOR_1_MS1_PORT
#define YF_AXIS_MOTOR_EN_PIN        MOTOR_1_ENABLE_PIN
#define YF_AXIS_MOTOR_STEP_PIN      MOTOR_1_STEP_PIN
#define YF_AXIS_MOTOR_DIR_PIN       MOTOR_1_DIR_PIN
#define YF_AXIS_MOTOR_MS0_PIN       MOTOR_1_MS0_PIN
#define YF_AXIS_MOTOR_MS1_PIN       MOTOR_1_MS1_PIN

#define ZL_AXIS_MOTOR_EN_PORT       MOTOR_2_ENABLE_PORT
#define ZL_AXIS_MOTOR_STEP_PORT     MOTOR_2_STEP_PORT
#define ZL_AXIS_MOTOR_DIR_PORT      MOTOR_2_DIR_PORT
#define ZL_AXIS_MOTOR_MS0_PORT      MOTOR_2_MS0_PORT
#define ZL_AXIS_MOTOR_MS1_PORT      MOTOR_2_MS1_PORT
#define ZL_AXIS_MOTOR_EN_PIN        MOTOR_2_ENABLE_PIN
#define ZL_AXIS_MOTOR_STEP_PIN      MOTOR_2_STEP_PIN
#define ZL_AXIS_MOTOR_DIR_PIN       MOTOR_2_DIR_PIN
#define ZL_AXIS_MOTOR_MS0_PIN       MOTOR_2_MS0_PIN
#define ZL_AXIS_MOTOR_MS1_PIN       MOTOR_2_MS1_PIN

#define ZR_AXIS_MOTOR_EN_PORT       MOTOR_3_ENABLE_PORT
#define ZR_AXIS_MOTOR_STEP_PORT     MOTOR_3_STEP_PORT
#define ZR_AXIS_MOTOR_DIR_PORT      MOTOR_3_DIR_PORT
#define ZR_AXIS_MOTOR_MS0_PORT      MOTOR_3_MS0_PORT
#define ZR_AXIS_MOTOR_MS1_PORT      MOTOR_3_MS1_PORT
#define ZR_AXIS_MOTOR_EN_PIN        MOTOR_3_ENABLE_PIN
#define ZR_AXIS_MOTOR_STEP_PIN      MOTOR_3_STEP_PIN
#define ZR_AXIS_MOTOR_DIR_PIN       MOTOR_3_DIR_PIN
#define ZR_AXIS_MOTOR_MS0_PIN       MOTOR_3_MS0_PIN
#define ZR_AXIS_MOTOR_MS1_PIN       MOTOR_3_MS1_PIN

#define EXTRUDER_0_MOTOR_EN_PORT    MOTOR_4_ENABLE_PORT
#define EXTRUDER_0_MOTOR_STEP_PORT  MOTOR_4_STEP_PORT
#define EXTRUDER_0_MOTOR_DIR_PORT   MOTOR_4_DIR_PORT
#define EXTRUDER_0_MOTOR_MS0_PORT   MOTOR_4_MS0_PORT
#define EXTRUDER_0_MOTOR_MS1_PORT   MOTOR_4_MS1_PORT
#define EXTRUDER_0_MOTOR_EN_PIN     MOTOR_4_ENABLE_PIN
#define EXTRUDER_0_MOTOR_STEP_PIN   MOTOR_4_STEP_PIN
#define EXTRUDER_0_MOTOR_DIR_PIN    MOTOR_4_DIR_PIN
#define EXTRUDER_0_MOTOR_MS0_PIN    MOTOR_4_MS0_PIN
#define EXTRUDER_0_MOTOR_MS1_PIN    MOTOR_4_MS1_PIN

#define EXTRUDER_1_MOTOR_EN_PORT    MOTOR_5_ENABLE_PORT
#define EXTRUDER_1_MOTOR_STEP_PORT  MOTOR_5_STEP_PORT
#define EXTRUDER_1_MOTOR_DIR_PORT   MOTOR_5_DIR_PORT
#define EXTRUDER_1_MOTOR_MS0_PORT   MOTOR_5_MS0_PORT
#define EXTRUDER_1_MOTOR_MS1_PORT   MOTOR_5_MS1_PORT
#define EXTRUDER_1_MOTOR_EN_PIN     MOTOR_5_ENABLE_PIN
#define EXTRUDER_1_MOTOR_STEP_PIN   MOTOR_5_STEP_PIN
#define EXTRUDER_1_MOTOR_DIR_PIN    MOTOR_5_DIR_PIN
#define EXTRUDER_1_MOTOR_MS0_PIN    MOTOR_5_MS0_PIN
#define EXTRUDER_1_MOTOR_MS1_PIN    MOTOR_5_MS1_PIN

#define AUX_MOTOR_EN_PORT           MOTOR_6_ENABLE_PORT
#define AUX_MOTOR_STEP_PORT         MOTOR_6_STEP_PORT
#define AUX_MOTOR_DIR_PORT          MOTOR_6_DIR_PORT
#define AUX_MOTOR_MS0_PORT          MOTOR_6_MS0_PORT
#define AUX_MOTOR_MS1_PORT          MOTOR_6_MS1_PORT
#define AUX_MOTOR_EN_PIN            MOTOR_6_ENABLE_PIN
#define AUX_MOTOR_STEP_PIN          MOTOR_6_STEP_PIN
#define AUX_MOTOR_DIR_PIN           MOTOR_6_DIR_PIN
#define AUX_MOTOR_MS0_PIN           MOTOR_6_MS0_PIN
#define AUX_MOTOR_MS1_PIN           MOTOR_6_MS1_PIN

#define MOTOR_STILL          0
#define MOTOR_MOVE_INCREASE  1
#define MOTOR_MOVE_DECREASE -1
#define MOTOR_MOVE_RIGHT     MOTOR_MOVE_INCREASE
#define MOTOR_MOVE_LEFT      MOTOR_MOVE_DECREASE
#define MOTOR_MOVE_FORWARD   MOTOR_MOVE_INCREASE
#define MOTOR_MOVE_BACKWARD  MOTOR_MOVE_DECREASE
#define MOTOR_MOVE_UP        MOTOR_MOVE_INCREASE
#define MOTOR_MOVE_DOWN      MOTOR_MOVE_DECREASE

#define ENDSTOP_HIT_OR_NO_POWER 1
#define ENDSTOP_NOT_HIT 0

extern uint8_t motors_updated;
extern uint8_t update_motors;
extern uint8_t update_pid;

void process_motors(struct cnc_motor_list_struct* motors);
void enable_motor(struct cnc_motor_struct* motor);
void disable_motor(struct cnc_motor_struct* motor);
void set_motor_direction(struct cnc_motor_struct* motor, int8_t direction);
void enable_axis_motors(struct cnc_motor_list_struct* motors);
void disable_axis_motors(struct cnc_motor_list_struct* motors);
void check_endstops(struct cnc_motor_list_struct* motors);
void check_endstop(struct cnc_motor_struct* motor);
void init_motors(struct cnc_motor_list_struct* motors);
void init_motor(struct cnc_motor_struct* motor, const char* name);
void handle_motors(struct cnc_state_struct* cnc);
void handle_step(struct cnc_motor_struct* motor);
void check_periods(struct cnc_state_struct* cnc, struct cnc_motor_list_struct* motors);
void check_period(struct cnc_state_struct* cnc, struct cnc_motor_struct* motors);
void step_motor_high(struct cnc_motor_struct* motor);
void step_motor_low(struct cnc_motor_struct* motor);
void check_directions(struct cnc_state_struct* cnc, struct cnc_motor_list_struct* motor);
void check_direction(struct cnc_state_struct* cnc, struct cnc_motor_struct* motor);

#endif /* SRC_CNC_MOTORS_H_ */
