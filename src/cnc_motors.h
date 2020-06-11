/*
 * cnc_motors.h
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_MOTORS_H_
#define SRC_CNC_MOTORS_H_

#include "stdint.h"
#include "cnc_gpio.h"
#include "cnc_functions.h"
#include "string.h"
#include "stdarg.h"
#include "common_cnc.h"

#define MAX_MOTOR_NAME_LENGTH 8

struct cnc_motor_port_struct {
	GPIO_Port_TypeDef en_port;
	GPIO_Port_TypeDef step_port;
	GPIO_Port_TypeDef dir_port;
	GPIO_Port_TypeDef ms0_port;
	GPIO_Port_TypeDef ms1_port;
};

struct cnc_motor_pin_struct {
	uint8_t en_pin;
	uint8_t step_pin;
	uint8_t dir_pin;
	uint8_t ms0_pin;
	uint8_t ms1_pin;
};

struct cnc_motor_struct {
	uint8_t enabled;
   uint8_t active;
	uint8_t known_position;
	uint8_t find_zero;
	uint8_t find_max;
	uint8_t step_set;
   uint8_t* min_range_flag;
   uint8_t* max_range_flag;
	cnc_double target;
	cnc_double distance;
   cnc_double position;
	uint8_t direction;
   cnc_double speed;
   cnc_double steps_per_mm;
   cnc_double mm_per_step;
   uint8_t step_count_set;
	uint32_t next_step_count;
   uint16_t last_timer_error;
   uint32_t total_timer_count;
	char name[MAX_MOTOR_NAME_LENGTH] = {};
	struct cnc_motor_pin_struct pins;
	struct cnc_motor_port_struct ports;
};

struct cnc_motor_list_struct {
   uint8_t motor_irq;
   uint8_t next_valid_irq;
   uint8_t valid_irq;
   uint8_t next_timer_value_loaded;
   uint16_t next_timer_value;
   uint8_t motors_active;
   cnc_double motor_speed;
   cnc_double max_distance;
   struct cnc_motor_struct motor[NUM_MOTORS];
};

struct cnc_endstop_struct {
	uint8_t enabled;
   uint8_t status;
   uint8_t previous_status;
   GPIO_Port_TypeDef port;
	uint8_t pin;
};

struct cnc_endstop_list_struct {
   uint8_t new_event;
	struct cnc_endstop_struct endstop[NUM_ENDSTOPS];
   uint8_t empty_endstop;
};

#define XL_AXIS_MOTOR_EN_PORT       MOTOR_0_ENABLE_PINPORT
#define XL_AXIS_MOTOR_STEP_PORT     MOTOR_0_STEP_PINPORT
#define XL_AXIS_MOTOR_DIR_PORT      MOTOR_0_DIR_PINPORT
#define XL_AXIS_MOTOR_MS0_PORT      MOTOR_0_MS0_PINPORT
#define XL_AXIS_MOTOR_MS1_PORT      MOTOR_0_MS1_PINPORT
#define XL_AXIS_MOTOR_EN_PIN        MOTOR_0_ENABLE_PINBIT
#define XL_AXIS_MOTOR_STEP_PIN      MOTOR_0_STEP_PINBIT
#define XL_AXIS_MOTOR_DIR_PIN       MOTOR_0_DIR_PINBIT
#define XL_AXIS_MOTOR_MS0_PIN       MOTOR_0_MS0_PINBIT
#define XL_AXIS_MOTOR_MS1_PIN       MOTOR_0_MS1_PINBIT

#define YF_AXIS_MOTOR_EN_PORT       MOTOR_1_ENABLE_PINPORT
#define YF_AXIS_MOTOR_STEP_PORT     MOTOR_1_STEP_PINPORT
#define YF_AXIS_MOTOR_DIR_PORT      MOTOR_1_DIR_PINPORT
#define YF_AXIS_MOTOR_MS0_PORT      MOTOR_1_MS0_PINPORT
#define YF_AXIS_MOTOR_MS1_PORT      MOTOR_1_MS1_PINPORT
#define YF_AXIS_MOTOR_EN_PIN        MOTOR_1_ENABLE_PINBIT
#define YF_AXIS_MOTOR_STEP_PIN      MOTOR_1_STEP_PINBIT
#define YF_AXIS_MOTOR_DIR_PIN       MOTOR_1_DIR_PINBIT
#define YF_AXIS_MOTOR_MS0_PIN       MOTOR_1_MS0_PINBIT
#define YF_AXIS_MOTOR_MS1_PIN       MOTOR_1_MS1_PINBIT

#define ZL_AXIS_MOTOR_EN_PORT       MOTOR_2_ENABLE_PINPORT
#define ZL_AXIS_MOTOR_STEP_PORT     MOTOR_2_STEP_PINPORT
#define ZL_AXIS_MOTOR_DIR_PORT      MOTOR_2_DIR_PINPORT
#define ZL_AXIS_MOTOR_MS0_PORT      MOTOR_2_MS0_PINPORT
#define ZL_AXIS_MOTOR_MS1_PORT      MOTOR_2_MS1_PINPORT
#define ZL_AXIS_MOTOR_EN_PIN        MOTOR_2_ENABLE_PINBIT
#define ZL_AXIS_MOTOR_STEP_PIN      MOTOR_2_STEP_PINBIT
#define ZL_AXIS_MOTOR_DIR_PIN       MOTOR_2_DIR_PINBIT
#define ZL_AXIS_MOTOR_MS0_PIN       MOTOR_2_MS0_PINBIT
#define ZL_AXIS_MOTOR_MS1_PIN       MOTOR_2_MS1_PINBIT

#define ZR_AXIS_MOTOR_EN_PORT       MOTOR_3_ENABLE_PINPORT
#define ZR_AXIS_MOTOR_STEP_PORT     MOTOR_3_STEP_PINPORT
#define ZR_AXIS_MOTOR_DIR_PORT      MOTOR_3_DIR_PINPORT
#define ZR_AXIS_MOTOR_MS0_PORT      MOTOR_3_MS0_PINPORT
#define ZR_AXIS_MOTOR_MS1_PORT      MOTOR_3_MS1_PINPORT
#define ZR_AXIS_MOTOR_EN_PIN        MOTOR_3_ENABLE_PINBIT
#define ZR_AXIS_MOTOR_STEP_PIN      MOTOR_3_STEP_PINBIT
#define ZR_AXIS_MOTOR_DIR_PIN       MOTOR_3_DIR_PINBIT
#define ZR_AXIS_MOTOR_MS0_PIN       MOTOR_3_MS0_PINBIT
#define ZR_AXIS_MOTOR_MS1_PIN       MOTOR_3_MS1_PINBIT

#define EXTRUDER_0_MOTOR_EN_PORT    MOTOR_4_ENABLE_PINPORT
#define EXTRUDER_0_MOTOR_STEP_PORT  MOTOR_4_STEP_PINPORT
#define EXTRUDER_0_MOTOR_DIR_PORT   MOTOR_4_DIR_PINPORT
#define EXTRUDER_0_MOTOR_MS0_PORT   MOTOR_4_MS0_PINPORT
#define EXTRUDER_0_MOTOR_MS1_PORT   MOTOR_4_MS1_PINPORT
#define EXTRUDER_0_MOTOR_EN_PIN     MOTOR_4_ENABLE_PINBIT
#define EXTRUDER_0_MOTOR_STEP_PIN   MOTOR_4_STEP_PINBIT
#define EXTRUDER_0_MOTOR_DIR_PIN    MOTOR_4_DIR_PINBIT
#define EXTRUDER_0_MOTOR_MS0_PIN    MOTOR_4_MS0_PINBIT
#define EXTRUDER_0_MOTOR_MS1_PIN    MOTOR_4_MS1_PINBIT

#define EXTRUDER_1_MOTOR_EN_PORT    MOTOR_5_ENABLE_PINPORT
#define EXTRUDER_1_MOTOR_STEP_PORT  MOTOR_5_STEP_PINPORT
#define EXTRUDER_1_MOTOR_DIR_PORT   MOTOR_5_DIR_PINPORT
#define EXTRUDER_1_MOTOR_MS0_PORT   MOTOR_5_MS0_PINPORT
#define EXTRUDER_1_MOTOR_MS1_PORT   MOTOR_5_MS1_PINPORT
#define EXTRUDER_1_MOTOR_EN_PIN     MOTOR_5_ENABLE_PINBIT
#define EXTRUDER_1_MOTOR_STEP_PIN   MOTOR_5_STEP_PINBIT
#define EXTRUDER_1_MOTOR_DIR_PIN    MOTOR_5_DIR_PINBIT
#define EXTRUDER_1_MOTOR_MS0_PIN    MOTOR_5_MS0_PINBIT
#define EXTRUDER_1_MOTOR_MS1_PIN    MOTOR_5_MS1_PINBIT

#define AUX_MOTOR_EN_PORT           MOTOR_6_ENABLE_PINPORT
#define AUX_MOTOR_STEP_PORT         MOTOR_6_STEP_PINPORT
#define AUX_MOTOR_DIR_PORT          MOTOR_6_DIR_PINPORT
#define AUX_MOTOR_MS0_PORT          MOTOR_6_MS0_PINPORT
#define AUX_MOTOR_MS1_PORT          MOTOR_6_MS1_PINPORT
#define AUX_MOTOR_EN_PIN            MOTOR_6_ENABLE_PINBIT
#define AUX_MOTOR_STEP_PIN          MOTOR_6_STEP_PINBIT
#define AUX_MOTOR_DIR_PIN           MOTOR_6_DIR_PINBIT
#define AUX_MOTOR_MS0_PIN           MOTOR_6_MS0_PINBIT
#define AUX_MOTOR_MS1_PIN           MOTOR_6_MS1_PINBIT

#define MOTOR_ENABLED        0
#define MOTOR_DISABLED       1
#define MOTOR_MOVE_DECREASE  0
#define MOTOR_MOVE_INCREASE  1
#define MOTOR_MOVE_RIGHT     MOTOR_MOVE_INCREASE
#define MOTOR_MOVE_LEFT      MOTOR_MOVE_DECREASE
#define MOTOR_MOVE_FORWARD   MOTOR_MOVE_INCREASE
#define MOTOR_MOVE_BACKWARD  MOTOR_MOVE_DECREASE
#define MOTOR_MOVE_UP        MOTOR_MOVE_INCREASE
#define MOTOR_MOVE_DOWN      MOTOR_MOVE_DECREASE

#define ENDSTOP_HIT_OR_NO_POWER 0
#define ENDSTOP_NOT_HIT 1

extern cnc_double next_period;

void process_motors(void);
void get_next_timer_value(void);
void enable_motor(struct cnc_motor_struct* motor);
void disable_motor(struct cnc_motor_struct* motor);
void set_motor_direction(struct cnc_motor_struct* motor, int8_t direction);
void check_endstops(void);
void check_endstop(struct cnc_endstop_struct* endstop);
void init_motors(void);
void init_motor(struct cnc_motor_struct* motor, const char* name);
void handle_motors(void);
void init_endstops(void);
void init_endstop(struct cnc_endstop_struct* endstop);
void handle_step(struct cnc_motor_struct* motor);
void set_step(void);
//void check_periods(struct cnc_state_struct* cnc, struct cnc_motor_list_struct* motors);
//void check_period(struct cnc_state_struct* cnc, struct cnc_motor_struct* motors);
void step_motor_set_step(struct cnc_motor_struct* motor);
void step_motor_clear_step(struct cnc_motor_struct* motor);
//void check_directions(struct cnc_state_struct* cnc, struct cnc_motor_list_struct* motor);
//void check_direction(struct cnc_state_struct* cnc, struct cnc_motor_struct* motor);

#endif /* SRC_CNC_MOTORS_H_ */
