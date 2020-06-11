/*
 * cnc_motors.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#include "Arduino.h"
#include "main.h"
#include "cnc_motors.h"
#include "cnc_timers.h"
#include "cnc_functions.h"
#include "common_cnc.h"
#include "math.h"

uint16_t endstop_status;

void init_motors(void){
	cnc_motors.motor[MOTOR_AUX].ports.dir_port   = AUX_MOTOR_DIR_PORT;
	cnc_motors.motor[MOTOR_AUX].ports.en_port    = AUX_MOTOR_EN_PORT;
	cnc_motors.motor[MOTOR_AUX].ports.ms0_port   = AUX_MOTOR_MS0_PORT;
	cnc_motors.motor[MOTOR_AUX].ports.ms1_port   = AUX_MOTOR_MS1_PORT;
	cnc_motors.motor[MOTOR_AUX].ports.step_port  = AUX_MOTOR_STEP_PORT;
	cnc_motors.motor[MOTOR_AUX].pins.dir_pin     = AUX_MOTOR_DIR_PIN;
	cnc_motors.motor[MOTOR_AUX].pins.en_pin      = AUX_MOTOR_EN_PIN;
	cnc_motors.motor[MOTOR_AUX].pins.ms0_pin     = AUX_MOTOR_MS0_PIN;
	cnc_motors.motor[MOTOR_AUX].pins.ms1_pin     = AUX_MOTOR_MS1_PIN;
	cnc_motors.motor[MOTOR_AUX].pins.step_pin    = AUX_MOTOR_STEP_PIN;
   cnc_motors.motor[MOTOR_AUX].steps_per_mm     = AUX_STEPS_PER_MM;
   cnc_motors.motor[MOTOR_AUX].mm_per_step      = AUX_MM_PER_STEP;
   cnc_motors.motor[MOTOR_AUX].min_range_flag   = &cnc_endstops.empty_endstop;
   cnc_motors.motor[MOTOR_AUX].max_range_flag   = &cnc_endstops.empty_endstop;

	cnc_motors.motor[MOTOR_EXTRUDER_0].ports.dir_port   = EXTRUDER_0_MOTOR_DIR_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_0].ports.en_port    = EXTRUDER_0_MOTOR_EN_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_0].ports.ms0_port   = EXTRUDER_0_MOTOR_MS0_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_0].ports.ms1_port   = EXTRUDER_0_MOTOR_MS1_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_0].ports.step_port  = EXTRUDER_0_MOTOR_STEP_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_0].pins.dir_pin     = EXTRUDER_0_MOTOR_DIR_PIN;
	cnc_motors.motor[MOTOR_EXTRUDER_0].pins.en_pin      = EXTRUDER_0_MOTOR_EN_PIN;
	cnc_motors.motor[MOTOR_EXTRUDER_0].pins.ms0_pin     = EXTRUDER_0_MOTOR_MS0_PIN;
	cnc_motors.motor[MOTOR_EXTRUDER_0].pins.ms1_pin     = EXTRUDER_0_MOTOR_MS1_PIN;
	cnc_motors.motor[MOTOR_EXTRUDER_0].pins.step_pin    = EXTRUDER_0_MOTOR_STEP_PIN;
   cnc_motors.motor[MOTOR_EXTRUDER_0].steps_per_mm     = EXTRUDER_STEPS_PER_MM;
   cnc_motors.motor[MOTOR_EXTRUDER_0].mm_per_step      = EXTRUDER_MM_PER_STEP;
   cnc_motors.motor[MOTOR_EXTRUDER_0].min_range_flag   = &cnc_endstops.empty_endstop;
   cnc_motors.motor[MOTOR_EXTRUDER_0].max_range_flag   = &cnc_endstops.empty_endstop;

	cnc_motors.motor[MOTOR_EXTRUDER_1].ports.dir_port   = EXTRUDER_1_MOTOR_DIR_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_1].ports.en_port    = EXTRUDER_1_MOTOR_EN_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_1].ports.ms0_port   = EXTRUDER_1_MOTOR_MS0_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_1].ports.ms1_port   = EXTRUDER_1_MOTOR_MS1_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_1].ports.step_port  = EXTRUDER_1_MOTOR_STEP_PORT;
	cnc_motors.motor[MOTOR_EXTRUDER_1].pins.dir_pin     = EXTRUDER_1_MOTOR_DIR_PIN;
	cnc_motors.motor[MOTOR_EXTRUDER_1].pins.en_pin      = EXTRUDER_1_MOTOR_EN_PIN;
	cnc_motors.motor[MOTOR_EXTRUDER_1].pins.ms0_pin     = EXTRUDER_1_MOTOR_MS0_PIN;
	cnc_motors.motor[MOTOR_EXTRUDER_1].pins.ms1_pin     = EXTRUDER_1_MOTOR_MS1_PIN;
	cnc_motors.motor[MOTOR_EXTRUDER_1].pins.step_pin    = EXTRUDER_1_MOTOR_STEP_PIN;
   cnc_motors.motor[MOTOR_EXTRUDER_1].steps_per_mm     = EXTRUDER_STEPS_PER_MM;
   cnc_motors.motor[MOTOR_EXTRUDER_1].mm_per_step      = EXTRUDER_MM_PER_STEP;
   cnc_motors.motor[MOTOR_EXTRUDER_1].min_range_flag   = &cnc_endstops.empty_endstop;
   cnc_motors.motor[MOTOR_EXTRUDER_1].max_range_flag   = &cnc_endstops.empty_endstop;

	cnc_motors.motor[MOTOR_AXIS_XL].ports.dir_port   = XL_AXIS_MOTOR_DIR_PORT;
	cnc_motors.motor[MOTOR_AXIS_XL].ports.en_port    = XL_AXIS_MOTOR_EN_PORT;
	cnc_motors.motor[MOTOR_AXIS_XL].ports.ms0_port   = XL_AXIS_MOTOR_MS0_PORT;
	cnc_motors.motor[MOTOR_AXIS_XL].ports.ms1_port   = XL_AXIS_MOTOR_MS1_PORT;
	cnc_motors.motor[MOTOR_AXIS_XL].ports.step_port  = XL_AXIS_MOTOR_STEP_PORT;
	cnc_motors.motor[MOTOR_AXIS_XL].pins.dir_pin     = XL_AXIS_MOTOR_DIR_PIN;
	cnc_motors.motor[MOTOR_AXIS_XL].pins.en_pin      = XL_AXIS_MOTOR_EN_PIN;
	cnc_motors.motor[MOTOR_AXIS_XL].pins.ms0_pin     = XL_AXIS_MOTOR_MS0_PIN;
	cnc_motors.motor[MOTOR_AXIS_XL].pins.ms1_pin     = XL_AXIS_MOTOR_MS1_PIN;
	cnc_motors.motor[MOTOR_AXIS_XL].pins.step_pin    = XL_AXIS_MOTOR_STEP_PIN;
   cnc_motors.motor[MOTOR_AXIS_XL].steps_per_mm     = AXIS_STEPS_PER_MM;
   cnc_motors.motor[MOTOR_AXIS_XL].mm_per_step      = AXIS_MM_PER_STEP;
   cnc_motors.motor[MOTOR_AXIS_XL].min_range_flag   = &cnc_endstops.endstop[X_L_MIN].status;
   cnc_motors.motor[MOTOR_AXIS_XL].max_range_flag   = &cnc_endstops.endstop[X_R_MAX].status;

	cnc_motors.motor[MOTOR_AXIS_YF].ports.dir_port   = YF_AXIS_MOTOR_DIR_PORT;
	cnc_motors.motor[MOTOR_AXIS_YF].ports.en_port    = YF_AXIS_MOTOR_EN_PORT;
	cnc_motors.motor[MOTOR_AXIS_YF].ports.ms0_port   = YF_AXIS_MOTOR_MS0_PORT;
	cnc_motors.motor[MOTOR_AXIS_YF].ports.ms1_port   = YF_AXIS_MOTOR_MS1_PORT;
	cnc_motors.motor[MOTOR_AXIS_YF].ports.step_port  = YF_AXIS_MOTOR_STEP_PORT;
	cnc_motors.motor[MOTOR_AXIS_YF].pins.dir_pin     = YF_AXIS_MOTOR_DIR_PIN;
	cnc_motors.motor[MOTOR_AXIS_YF].pins.en_pin      = YF_AXIS_MOTOR_EN_PIN;
	cnc_motors.motor[MOTOR_AXIS_YF].pins.ms0_pin     = YF_AXIS_MOTOR_MS0_PIN;
	cnc_motors.motor[MOTOR_AXIS_YF].pins.ms1_pin     = YF_AXIS_MOTOR_MS1_PIN;
	cnc_motors.motor[MOTOR_AXIS_YF].pins.step_pin    = YF_AXIS_MOTOR_STEP_PIN;
   cnc_motors.motor[MOTOR_AXIS_YF].steps_per_mm     = AXIS_STEPS_PER_MM;
   cnc_motors.motor[MOTOR_AXIS_YF].mm_per_step      = AXIS_MM_PER_STEP;
   cnc_motors.motor[MOTOR_AXIS_YF].min_range_flag   = &cnc_endstops.endstop[Y_F_MIN].status;
   cnc_motors.motor[MOTOR_AXIS_YF].max_range_flag   = &cnc_endstops.endstop[Y_B_MAX].status;

	cnc_motors.motor[MOTOR_AXIS_ZL].ports.dir_port   = ZL_AXIS_MOTOR_DIR_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZL].ports.en_port    = ZL_AXIS_MOTOR_EN_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZL].ports.ms0_port   = ZL_AXIS_MOTOR_MS0_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZL].ports.ms1_port   = ZL_AXIS_MOTOR_MS1_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZL].ports.step_port  = ZL_AXIS_MOTOR_STEP_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZL].pins.dir_pin     = ZL_AXIS_MOTOR_DIR_PIN;
	cnc_motors.motor[MOTOR_AXIS_ZL].pins.en_pin      = ZL_AXIS_MOTOR_EN_PIN;
	cnc_motors.motor[MOTOR_AXIS_ZL].pins.ms0_pin     = ZL_AXIS_MOTOR_MS0_PIN;
	cnc_motors.motor[MOTOR_AXIS_ZL].pins.ms1_pin     = ZL_AXIS_MOTOR_MS1_PIN;
	cnc_motors.motor[MOTOR_AXIS_ZL].pins.step_pin    = ZL_AXIS_MOTOR_STEP_PIN;
   cnc_motors.motor[MOTOR_AXIS_ZL].steps_per_mm     = AXIS_STEPS_PER_MM;
   cnc_motors.motor[MOTOR_AXIS_ZL].mm_per_step      = AXIS_MM_PER_STEP;
   cnc_motors.motor[MOTOR_AXIS_ZL].min_range_flag   = &cnc_endstops.endstop[Z_L_MIN].status;
   cnc_motors.motor[MOTOR_AXIS_ZL].max_range_flag   = &cnc_endstops.endstop[Z_L_MAX].status;

	cnc_motors.motor[MOTOR_AXIS_ZR].ports.dir_port   = ZR_AXIS_MOTOR_DIR_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZR].ports.en_port    = ZR_AXIS_MOTOR_EN_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZR].ports.ms0_port   = ZR_AXIS_MOTOR_MS0_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZR].ports.ms1_port   = ZR_AXIS_MOTOR_MS1_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZR].ports.step_port  = ZR_AXIS_MOTOR_STEP_PORT;
	cnc_motors.motor[MOTOR_AXIS_ZR].pins.dir_pin     = ZR_AXIS_MOTOR_DIR_PIN;
	cnc_motors.motor[MOTOR_AXIS_ZR].pins.en_pin      = ZR_AXIS_MOTOR_EN_PIN;
	cnc_motors.motor[MOTOR_AXIS_ZR].pins.ms0_pin     = ZR_AXIS_MOTOR_MS0_PIN;
	cnc_motors.motor[MOTOR_AXIS_ZR].pins.ms1_pin     = ZR_AXIS_MOTOR_MS1_PIN;
	cnc_motors.motor[MOTOR_AXIS_ZR].pins.step_pin    = ZR_AXIS_MOTOR_STEP_PIN;
   cnc_motors.motor[MOTOR_AXIS_ZR].steps_per_mm     = AXIS_STEPS_PER_MM;
   cnc_motors.motor[MOTOR_AXIS_ZR].mm_per_step      = AXIS_MM_PER_STEP;
   cnc_motors.motor[MOTOR_AXIS_ZR].min_range_flag   = &cnc_endstops.endstop[Z_R_MIN].status;
   cnc_motors.motor[MOTOR_AXIS_ZR].max_range_flag   = &cnc_endstops.endstop[Z_R_MAX].status;

   cnc_motors.motor_irq = 0;
   cnc_motors.next_timer_value = DEFAULT_TIMER_COUNT;
   cnc_motors.next_timer_value_loaded = 0;
   cnc_motors.valid_irq = 0;
   cnc_motors.next_valid_irq = 0;
   cnc_motors.motors_active = 0;

	init_motor(&cnc_motors.motor[MOTOR_AUX], "Aux");
	init_motor(&cnc_motors.motor[MOTOR_EXTRUDER_0], "Extd-0");
	init_motor(&cnc_motors.motor[MOTOR_EXTRUDER_1], "Extd-1");
	init_motor(&cnc_motors.motor[MOTOR_AXIS_XL], "X-LR");
	init_motor(&cnc_motors.motor[MOTOR_AXIS_YF], "Y-FB");
	init_motor(&cnc_motors.motor[MOTOR_AXIS_ZL], "ZL-UD");
	init_motor(&cnc_motors.motor[MOTOR_AXIS_ZR], "ZR-UD");
}

void init_motor(struct cnc_motor_struct* motor, const char* name){
   motor->enabled = 0;
   motor->active = 0;
	motor->direction = MOTOR_MOVE_DECREASE;
	motor->step_set = 0;
	motor->position = 0.0;
   motor->target = 0.0;
	motor->known_position = 0;
   disable_motor(motor);
   step_motor_clear_step(motor);
	motor->speed = 0;
   motor->next_step_count = 0;
   motor->last_timer_error = 0;
   motor->total_timer_count = 0;
   motor->step_count_set = 0;
	motor->find_zero = 0;
	motor->find_max = 0;
	strcpy(motor->name, name);
}

void init_endstops(void){
   cnc_endstops.new_event = 0;
   cnc_endstops.endstop[0].enabled = 1;
   cnc_endstops.endstop[0].pin = ENDSTOP_0_PINBIT;
   cnc_endstops.endstop[0].port = ENDSTOP_0_PINPORT;

   cnc_endstops.endstop[1].enabled = 1;
   cnc_endstops.endstop[1].pin = ENDSTOP_1_PINBIT;
   cnc_endstops.endstop[1].port = ENDSTOP_1_PINPORT;

   cnc_endstops.endstop[2].enabled = 1;
   cnc_endstops.endstop[2].pin = ENDSTOP_2_PINBIT;
   cnc_endstops.endstop[2].port = ENDSTOP_2_PINPORT;

   cnc_endstops.endstop[3].enabled = 1;
   cnc_endstops.endstop[3].pin = ENDSTOP_3_PINBIT;
   cnc_endstops.endstop[3].port = ENDSTOP_3_PINPORT;

   cnc_endstops.endstop[4].enabled = 1;
   cnc_endstops.endstop[4].pin = ENDSTOP_4_PINBIT;
   cnc_endstops.endstop[4].port = ENDSTOP_4_PINPORT;

   cnc_endstops.endstop[5].enabled = 1;
   cnc_endstops.endstop[5].pin = ENDSTOP_5_PINBIT;
   cnc_endstops.endstop[5].port = ENDSTOP_5_PINPORT;

   cnc_endstops.endstop[6].enabled = 1;
   cnc_endstops.endstop[6].pin = ENDSTOP_6_PINBIT;
   cnc_endstops.endstop[6].port = ENDSTOP_6_PINPORT;

   cnc_endstops.endstop[7].enabled = 1;
   cnc_endstops.endstop[7].pin = ENDSTOP_7_PINBIT;
   cnc_endstops.endstop[7].port = ENDSTOP_7_PINPORT;

   cnc_endstops.endstop[8].enabled = 1;
   cnc_endstops.endstop[8].pin = ENDSTOP_8_PINBIT;
   cnc_endstops.endstop[8].port = ENDSTOP_8_PINPORT;

   cnc_endstops.endstop[9].enabled = 1;
   cnc_endstops.endstop[9].pin = ENDSTOP_9_PINBIT;
   cnc_endstops.endstop[9].port = ENDSTOP_9_PINPORT;

   cnc_endstops.endstop[10].enabled = 1;
   cnc_endstops.endstop[10].pin = ENDSTOP_10_PINBIT;
   cnc_endstops.endstop[10].port = ENDSTOP_10_PINPORT;

   cnc_endstops.endstop[11].enabled = 1;
   cnc_endstops.endstop[11].pin = ENDSTOP_11_PINBIT;
   cnc_endstops.endstop[11].port = ENDSTOP_11_PINPORT;

   cnc_endstops.empty_endstop = 0;

   for(int i=0;i<NUM_ENDSTOPS;i++){
      check_endstop(&cnc_endstops.endstop[i]);
   }
}

void enable_motor(struct cnc_motor_struct* motor){
   cnc_gpio_write(motor->pins.en_pin, motor->ports.en_port, MOTOR_ENABLED);
	motor->enabled = 1;
}

void disable_motor(struct cnc_motor_struct* motor){
	cnc_gpio_write(motor->pins.en_pin, motor->ports.en_port, MOTOR_DISABLED);
	motor->enabled = 0;
}

void set_motor_direction(struct cnc_motor_struct* motor, int8_t direction){
   if(direction == MOTOR_MOVE_DECREASE){
      CLR(*motor->ports.dir_port, motor->pins.dir_pin);
      //cnc_gpio_write(motor->pins.dir_pin, motor->ports.dir_port, 0);
      motor->direction = direction;
   }
   if(direction == MOTOR_MOVE_INCREASE){
      SET(*motor->ports.dir_port, motor->pins.dir_pin);
      //cnc_gpio_write(motor->pins.dir_pin, motor->ports.dir_port, 1);
      motor->direction = direction;
   }
}


uint8_t count_set = 0;
uint32_t smallest_count = MAX_TIMER_COUNT;
uint8_t target_positive;
cnc_double position_delta;
cnc_motor_struct *this_motor;
void handle_motors(void){
   if(cnc_motors.motor_irq){
      cnc_motors.motor_irq = 0;
      if(cnc_motors.motors_active){
         for(int i=0;i<NUM_MOTORS;i++){
            if(cnc_motors.motor[i].active){
               if((cnc_motors.motor[i].target - cnc_motors.motor[i].position) > 0.0){
                  target_positive = 1;
               } else {
                  target_positive = 0;
               }
               
               if(((!target_positive) || cnc_motors.motor[i].find_zero) && !cnc_motors.motor[i].find_max){
                  cnc_motors.motor[i].direction = MOTOR_MOVE_DECREASE;
               }
               if(((target_positive) || cnc_motors.motor[i].find_max) && !cnc_motors.motor[i].find_zero){
                  cnc_motors.motor[i].direction = MOTOR_MOVE_INCREASE;
               }
               if(cnc_motors.motor[i].step_count_set){
                  if(cnc_motors.motor[i].next_step_count < cnc_motors.next_timer_value){
                     //cnc_printf("going to be neg!!! %ld, %ld", cnc_motors.motor[i].next_step_count, cnc_motors.next_timer_value);
                  }
                  cnc_motors.motor[i].next_step_count = cnc_motors.motor[i].next_step_count - cnc_motors.next_timer_value;
                  if(cnc_motors.motor[i].next_step_count <= MIN_TIMER_COUNT){
                     cnc_motors.motor[i].step_set = 1;
                     cnc_motors.motor[i].last_timer_error = cnc_motors.motor[i].next_step_count;
                     cnc_motors.motor[i].step_count_set = 0;
                     cnc_motors.motor[i].next_step_count = 0;
                  } else {
                     cnc_motors.motor[i].step_set = 0;
                     cnc_motors.motor[i].last_timer_error = 0;
                  }
               }
            } else {
               cnc_motors.motor[i].last_timer_error = 0;
               cnc_motors.motor[i].step_set = 0;
               cnc_motors.motor[i].step_count_set = 0;
            }
         }
         
         for(int i=0;i<NUM_MOTORS;i++){
            if(cnc_motors.motor[i].active && cnc_motors.motor[i].step_set){
               cnc_motors.motor[i].step_set = 0;
               step_motor_clear_step(&cnc_motors.motor[i]);
               if(cnc_motors.motor[i].direction == MOTOR_MOVE_INCREASE){
                  if(*cnc_motors.motor[i].max_range_flag){
                     //cnc_printf("Hit Max %s", motor->name);
                     //motor->find_max = 0;
                     //motor->active = 0;
                  } else {
                     // step motor, adjust position, set flag to clear rising edge next loop
                     set_motor_direction(&cnc_motors.motor[i], MOTOR_MOVE_INCREASE);
                     cnc_motors.motor[i].position = cnc_motors.motor[i].position + cnc_motors.motor[i].mm_per_step;
                     step_motor_set_step(&cnc_motors.motor[i]);
                  }
               } else {
                  if(*cnc_motors.motor[i].min_range_flag){
                     // min endstop hit
                     //motor->find_zero = 0;
                     //motor->active = 0;
                     //cnc_printf("Hit Min %s", motor->name);
                  } else {
                     // step motor, adjust position, set flag to clear rising edge next loop
                     set_motor_direction(&cnc_motors.motor[i], MOTOR_MOVE_DECREASE);
                     cnc_motors.motor[i].position = cnc_motors.motor[i].position - cnc_motors.motor[i].mm_per_step;
                     step_motor_set_step(&cnc_motors.motor[i]);
                  }
               }
            }
         }

         endstop_status = ENDSTOP_U_PORT;
         endstop_status = ((endstop_status << 8) | (ENDSTOP_L_PORT)) >> NUM_HEATERS;
         for(int i=0;i<NUM_ENDSTOPS;i++){
            if(cnc_endstops.endstop[i].enabled){
               cnc_endstops.endstop[i].status = (endstop_status >> i) & 0x01;
            } else {
               cnc_endstops.endstop[i].status = ENDSTOP_NOT_HIT;
            }
            if(cnc_endstops.endstop[i].status == ENDSTOP_NOT_HIT){
               // switch not hit
               cnc_endstops.endstop[i].status = 0;
            } else {
               // switch hit or no power to switch
               cnc_endstops.endstop[i].status = 1;
            }
            cnc_endstops.new_event |= (cnc_endstops.endstop[i].status && !cnc_endstops.endstop[i].previous_status);
            cnc_endstops.endstop[i].previous_status = cnc_endstops.endstop[i].status;
         }

         cnc_motors.motors_active = 0;
         for(int i=0;i<NUM_MOTORS;i++){
            if(cnc_motors.motor[i].find_max || cnc_motors.motor[i].find_zero){
               if(cnc_motors.motor[i].find_max && *cnc_motors.motor[i].max_range_flag){
                  cnc_motors.motor[i].find_max = 0;
                  cnc_motors.motor[i].active = 0;
                  cnc_motors.motor[i].target = cnc_motors.motor[i].position;
               }
               if(cnc_motors.motor[i].find_zero && *cnc_motors.motor[i].min_range_flag){
                  cnc_motors.motor[i].find_zero = 0;
                  cnc_motors.motor[i].active = 0;
                  cnc_motors.motor[i].target = cnc_motors.motor[i].position;
               }
            } else {
               if(fabs(cnc_motors.motor[i].target - cnc_motors.motor[i].position) < cnc_motors.motor[i].mm_per_step){
                  cnc_motors.motor[i].position = cnc_motors.motor[i].target;
                  cnc_motors.motor[i].active = 0;
               } else {
                  if((cnc_motors.motor[i].target > cnc_motors.motor[i].position) && (*cnc_motors.motor[i].max_range_flag)){
                     cnc_motors.motor[i].target = cnc_motors.motor[i].position;
                     cnc_motors.motor[i].active = 0;
                  }
                  if((cnc_motors.motor[i].target < cnc_motors.motor[i].position) && (*cnc_motors.motor[i].min_range_flag)){
                     cnc_motors.motor[i].target = cnc_motors.motor[i].position;
                     cnc_motors.motor[i].active = 0;
                  }
               }
            }
            cnc_motors.motors_active = cnc_motors.motors_active | cnc_motors.motor[i].active;
         }
         
         smallest_count = MAX_TIMER_COUNT;
         count_set = 0;
         
         for(int i=0;i<NUM_MOTORS;i++){
            if(cnc_motors.motor[i].active && !cnc_motors.motor[i].step_count_set){
               cnc_motors.motor[i].next_step_count = cnc_motors.motor[i].total_timer_count;
               cnc_motors.motor[i].step_count_set = 1;
               if(cnc_motors.motor[i].last_timer_error > 0){
                  cnc_motors.motor[i].next_step_count = cnc_motors.motor[i].next_step_count + cnc_motors.motor[i].last_timer_error;
                  cnc_motors.motor[i].last_timer_error = 0;
               }
            }
            if(cnc_motors.motor[i].active && cnc_motors.motor[i].step_count_set){
               if(cnc_motors.motor[i].next_step_count <= smallest_count){
                  smallest_count = cnc_motors.motor[i].next_step_count;
                  count_set = 1;
               }
            }
         }

         if(smallest_count > MAX_TIMER_COUNT){
            smallest_count = MAX_TIMER_COUNT;
         }

         cnc_motors.next_timer_value = smallest_count;
         TCNT3 = 0;
         OCR3A = cnc_motors.next_timer_value - 1;
      } else {
         //cnc_motors.next_timer_value = MAX_TIMER_COUNT;
      }
   }
}

void get_motor_speed(void){
   /*for(int i=0;i<NUM_MOTORS;i++){
      if(cnc_motors.motor[i].active){
         if(cnc_motors.motor[i].find_max || cnc_motors.motor[i].find_zero){
            cnc_motors.motor[i].speed = cnc_motors.motor_speed;
         } else {
            cnc_motors.motor[i].speed = cnc_motors.motor_speed * cnc_motors.motor[i].distance;
            cnc_motors.motor[i].speed = cnc_motors.motor[i].speed / cnc_motors.max_distance;
         }
         cnc_double steps_per_sec;
         cnc_double temp;
         steps_per_sec = cnc_motors.motor[i].steps_per_mm * cnc_motors.motor[i].speed; // steps per sec
         temp = USEC_PER_SEC / steps_per_sec; // usec per step
         cnc_motors.motor[i].total_timer_count = (uint32_t) (temp / MOTOR_TIMER_PERIOD_US);
      }
   }*/
}

void check_endstop(struct cnc_endstop_struct* endstop){
   uint8_t endstop_read;
   if(endstop->enabled){
      endstop_read = cnc_gpio_read(endstop->pin, endstop->port);
   } else {
      endstop_read = ENDSTOP_NOT_HIT;
   }
   if(endstop_read == ENDSTOP_NOT_HIT){
		// switch not hit
		endstop->status = 0;
	} else {
		// switch hit or no power to switch
		endstop->status = 1;
	}
}

void check_endstops(void){
   
}

void set_step(void){
   
}

void process_motors(struct cnc_motor_list_struct* motors){
   /*for(int i=0;i<NUM_MOTORS;i++){
      if(cnc_motors.motor[i].active){
         handle_step(&cnc_motors.motor[i]);
      }
   }*/
}

void handle_step(struct cnc_motor_struct* motor){
   
}

void get_next_timer_value(void){
   
}

void step_motor_set_step(struct cnc_motor_struct* motor){
   cnc_gpio_write(motor->pins.step_pin, motor->ports.step_port, 1);
}

void step_motor_clear_step(struct cnc_motor_struct* motor){
   cnc_gpio_write(motor->pins.step_pin, motor->ports.step_port, 0);
}

ISR(TIMER3_COMPA_vect){ // timer compare interrupt service routine
   cnc_motors.motor_irq = 1;
   // clear current count to reset
   TCNT3 = 0;
   OCR3A = MAX_TIMER_COUNT;
   //OCR3A = cnc_motors.next_timer_value - 1;
}