/*
 * cnc_motors.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#include "Arduino.h"
#include "cnc_motors.h"
#include "cnc_timers.h"
#include "cnc_functions.h"
#include "common_cnc.h"
#include "math.h"

void init_motors(struct cnc_motor_list_struct* motors, struct cnc_endstop_list_struct* endstops){
	motors->motor[MOTOR_AUX].ports.dir_port   = AUX_MOTOR_DIR_PORT;
	motors->motor[MOTOR_AUX].ports.en_port    = AUX_MOTOR_EN_PORT;
	motors->motor[MOTOR_AUX].ports.ms0_port   = AUX_MOTOR_MS0_PORT;
	motors->motor[MOTOR_AUX].ports.ms1_port   = AUX_MOTOR_MS1_PORT;
	motors->motor[MOTOR_AUX].ports.step_port  = AUX_MOTOR_STEP_PORT;
	motors->motor[MOTOR_AUX].pins.dir_pin     = AUX_MOTOR_DIR_PIN;
	motors->motor[MOTOR_AUX].pins.en_pin      = AUX_MOTOR_EN_PIN;
	motors->motor[MOTOR_AUX].pins.ms0_pin     = AUX_MOTOR_MS0_PIN;
	motors->motor[MOTOR_AUX].pins.ms1_pin     = AUX_MOTOR_MS1_PIN;
	motors->motor[MOTOR_AUX].pins.step_pin    = AUX_MOTOR_STEP_PIN;
   motors->motor[MOTOR_AUX].steps_per_mm     = AUX_STEPS_PER_MM;
   motors->motor[MOTOR_AUX].mm_per_step      = AUX_MM_PER_STEP;
   motors->motor[MOTOR_AUX].float_error      = AUX_MM_PER_STEP;
   motors->motor[MOTOR_AUX].min_range_flag   = &endstops->empty_endstop;
   motors->motor[MOTOR_AUX].max_range_flag   = &endstops->empty_endstop;

	motors->motor[MOTOR_EXTRUDER_0].ports.dir_port   = EXTRUDER_0_MOTOR_DIR_PORT;
	motors->motor[MOTOR_EXTRUDER_0].ports.en_port    = EXTRUDER_0_MOTOR_EN_PORT;
	motors->motor[MOTOR_EXTRUDER_0].ports.ms0_port   = EXTRUDER_0_MOTOR_MS0_PORT;
	motors->motor[MOTOR_EXTRUDER_0].ports.ms1_port   = EXTRUDER_0_MOTOR_MS1_PORT;
	motors->motor[MOTOR_EXTRUDER_0].ports.step_port  = EXTRUDER_0_MOTOR_STEP_PORT;
	motors->motor[MOTOR_EXTRUDER_0].pins.dir_pin     = EXTRUDER_0_MOTOR_DIR_PIN;
	motors->motor[MOTOR_EXTRUDER_0].pins.en_pin      = EXTRUDER_0_MOTOR_EN_PIN;
	motors->motor[MOTOR_EXTRUDER_0].pins.ms0_pin     = EXTRUDER_0_MOTOR_MS0_PIN;
	motors->motor[MOTOR_EXTRUDER_0].pins.ms1_pin     = EXTRUDER_0_MOTOR_MS1_PIN;
	motors->motor[MOTOR_EXTRUDER_0].pins.step_pin    = EXTRUDER_0_MOTOR_STEP_PIN;
   motors->motor[MOTOR_EXTRUDER_0].steps_per_mm     = EXTRUDER_STEPS_PER_MM;
   motors->motor[MOTOR_EXTRUDER_0].mm_per_step      = EXTRUDER_MM_PER_STEP;
   motors->motor[MOTOR_EXTRUDER_0].float_error      = EXTRUDER_MM_PER_STEP;
   motors->motor[MOTOR_EXTRUDER_0].min_range_flag   = &endstops->empty_endstop;
   motors->motor[MOTOR_EXTRUDER_0].max_range_flag   = &endstops->empty_endstop;

	motors->motor[MOTOR_EXTRUDER_1].ports.dir_port   = EXTRUDER_1_MOTOR_DIR_PORT;
	motors->motor[MOTOR_EXTRUDER_1].ports.en_port    = EXTRUDER_1_MOTOR_EN_PORT;
	motors->motor[MOTOR_EXTRUDER_1].ports.ms0_port   = EXTRUDER_1_MOTOR_MS0_PORT;
	motors->motor[MOTOR_EXTRUDER_1].ports.ms1_port   = EXTRUDER_1_MOTOR_MS1_PORT;
	motors->motor[MOTOR_EXTRUDER_1].ports.step_port  = EXTRUDER_1_MOTOR_STEP_PORT;
	motors->motor[MOTOR_EXTRUDER_1].pins.dir_pin     = EXTRUDER_1_MOTOR_DIR_PIN;
	motors->motor[MOTOR_EXTRUDER_1].pins.en_pin      = EXTRUDER_1_MOTOR_EN_PIN;
	motors->motor[MOTOR_EXTRUDER_1].pins.ms0_pin     = EXTRUDER_1_MOTOR_MS0_PIN;
	motors->motor[MOTOR_EXTRUDER_1].pins.ms1_pin     = EXTRUDER_1_MOTOR_MS1_PIN;
	motors->motor[MOTOR_EXTRUDER_1].pins.step_pin    = EXTRUDER_1_MOTOR_STEP_PIN;
   motors->motor[MOTOR_EXTRUDER_1].steps_per_mm     = EXTRUDER_STEPS_PER_MM;
   motors->motor[MOTOR_EXTRUDER_1].mm_per_step      = EXTRUDER_MM_PER_STEP;
   motors->motor[MOTOR_EXTRUDER_1].float_error      = EXTRUDER_MM_PER_STEP;
   motors->motor[MOTOR_EXTRUDER_1].min_range_flag   = &endstops->empty_endstop;
   motors->motor[MOTOR_EXTRUDER_1].max_range_flag   = &endstops->empty_endstop;

	motors->motor[MOTOR_AXIS_XL].ports.dir_port   = XL_AXIS_MOTOR_DIR_PORT;
	motors->motor[MOTOR_AXIS_XL].ports.en_port    = XL_AXIS_MOTOR_EN_PORT;
	motors->motor[MOTOR_AXIS_XL].ports.ms0_port   = XL_AXIS_MOTOR_MS0_PORT;
	motors->motor[MOTOR_AXIS_XL].ports.ms1_port   = XL_AXIS_MOTOR_MS1_PORT;
	motors->motor[MOTOR_AXIS_XL].ports.step_port  = XL_AXIS_MOTOR_STEP_PORT;
	motors->motor[MOTOR_AXIS_XL].pins.dir_pin     = XL_AXIS_MOTOR_DIR_PIN;
	motors->motor[MOTOR_AXIS_XL].pins.en_pin      = XL_AXIS_MOTOR_EN_PIN;
	motors->motor[MOTOR_AXIS_XL].pins.ms0_pin     = XL_AXIS_MOTOR_MS0_PIN;
	motors->motor[MOTOR_AXIS_XL].pins.ms1_pin     = XL_AXIS_MOTOR_MS1_PIN;
	motors->motor[MOTOR_AXIS_XL].pins.step_pin    = XL_AXIS_MOTOR_STEP_PIN;
   motors->motor[MOTOR_AXIS_XL].steps_per_mm     = AXIS_STEPS_PER_MM;
   motors->motor[MOTOR_AXIS_XL].mm_per_step      = AXIS_MM_PER_STEP;
   motors->motor[MOTOR_AXIS_XL].float_error      = AXIS_MM_PER_STEP;
   motors->motor[MOTOR_AXIS_XL].min_range_flag   = &endstops->endstop[X_L_MIN].status;
   motors->motor[MOTOR_AXIS_XL].max_range_flag   = &endstops->endstop[X_R_MAX].status;

	motors->motor[MOTOR_AXIS_YF].ports.dir_port   = YF_AXIS_MOTOR_DIR_PORT;
	motors->motor[MOTOR_AXIS_YF].ports.en_port    = YF_AXIS_MOTOR_EN_PORT;
	motors->motor[MOTOR_AXIS_YF].ports.ms0_port   = YF_AXIS_MOTOR_MS0_PORT;
	motors->motor[MOTOR_AXIS_YF].ports.ms1_port   = YF_AXIS_MOTOR_MS1_PORT;
	motors->motor[MOTOR_AXIS_YF].ports.step_port  = YF_AXIS_MOTOR_STEP_PORT;
	motors->motor[MOTOR_AXIS_YF].pins.dir_pin     = YF_AXIS_MOTOR_DIR_PIN;
	motors->motor[MOTOR_AXIS_YF].pins.en_pin      = YF_AXIS_MOTOR_EN_PIN;
	motors->motor[MOTOR_AXIS_YF].pins.ms0_pin     = YF_AXIS_MOTOR_MS0_PIN;
	motors->motor[MOTOR_AXIS_YF].pins.ms1_pin     = YF_AXIS_MOTOR_MS1_PIN;
	motors->motor[MOTOR_AXIS_YF].pins.step_pin    = YF_AXIS_MOTOR_STEP_PIN;
   motors->motor[MOTOR_AXIS_YF].steps_per_mm     = AXIS_STEPS_PER_MM;
   motors->motor[MOTOR_AXIS_YF].mm_per_step      = AXIS_MM_PER_STEP;
   motors->motor[MOTOR_AXIS_YF].float_error      = AXIS_MM_PER_STEP;
   motors->motor[MOTOR_AXIS_YF].min_range_flag   = &endstops->endstop[Y_F_MIN].status;
   motors->motor[MOTOR_AXIS_YF].max_range_flag   = &endstops->endstop[Y_B_MAX].status;

	motors->motor[MOTOR_AXIS_ZL].ports.dir_port   = ZL_AXIS_MOTOR_DIR_PORT;
	motors->motor[MOTOR_AXIS_ZL].ports.en_port    = ZL_AXIS_MOTOR_EN_PORT;
	motors->motor[MOTOR_AXIS_ZL].ports.ms0_port   = ZL_AXIS_MOTOR_MS0_PORT;
	motors->motor[MOTOR_AXIS_ZL].ports.ms1_port   = ZL_AXIS_MOTOR_MS1_PORT;
	motors->motor[MOTOR_AXIS_ZL].ports.step_port  = ZL_AXIS_MOTOR_STEP_PORT;
	motors->motor[MOTOR_AXIS_ZL].pins.dir_pin     = ZL_AXIS_MOTOR_DIR_PIN;
	motors->motor[MOTOR_AXIS_ZL].pins.en_pin      = ZL_AXIS_MOTOR_EN_PIN;
	motors->motor[MOTOR_AXIS_ZL].pins.ms0_pin     = ZL_AXIS_MOTOR_MS0_PIN;
	motors->motor[MOTOR_AXIS_ZL].pins.ms1_pin     = ZL_AXIS_MOTOR_MS1_PIN;
	motors->motor[MOTOR_AXIS_ZL].pins.step_pin    = ZL_AXIS_MOTOR_STEP_PIN;
   motors->motor[MOTOR_AXIS_ZL].steps_per_mm     = AXIS_STEPS_PER_MM;
   motors->motor[MOTOR_AXIS_ZL].mm_per_step      = AXIS_MM_PER_STEP;
   motors->motor[MOTOR_AXIS_ZL].float_error      = AXIS_MM_PER_STEP;
   motors->motor[MOTOR_AXIS_ZL].min_range_flag   = &endstops->endstop[Z_L_MIN].status;
   motors->motor[MOTOR_AXIS_ZL].max_range_flag   = &endstops->endstop[Z_L_MAX].status;

	motors->motor[MOTOR_AXIS_ZR].ports.dir_port   = ZR_AXIS_MOTOR_DIR_PORT;
	motors->motor[MOTOR_AXIS_ZR].ports.en_port    = ZR_AXIS_MOTOR_EN_PORT;
	motors->motor[MOTOR_AXIS_ZR].ports.ms0_port   = ZR_AXIS_MOTOR_MS0_PORT;
	motors->motor[MOTOR_AXIS_ZR].ports.ms1_port   = ZR_AXIS_MOTOR_MS1_PORT;
	motors->motor[MOTOR_AXIS_ZR].ports.step_port  = ZR_AXIS_MOTOR_STEP_PORT;
	motors->motor[MOTOR_AXIS_ZR].pins.dir_pin     = ZR_AXIS_MOTOR_DIR_PIN;
	motors->motor[MOTOR_AXIS_ZR].pins.en_pin      = ZR_AXIS_MOTOR_EN_PIN;
	motors->motor[MOTOR_AXIS_ZR].pins.ms0_pin     = ZR_AXIS_MOTOR_MS0_PIN;
	motors->motor[MOTOR_AXIS_ZR].pins.ms1_pin     = ZR_AXIS_MOTOR_MS1_PIN;
	motors->motor[MOTOR_AXIS_ZR].pins.step_pin    = ZR_AXIS_MOTOR_STEP_PIN;
   motors->motor[MOTOR_AXIS_ZR].steps_per_mm     = AXIS_STEPS_PER_MM;
   motors->motor[MOTOR_AXIS_ZR].mm_per_step      = AXIS_MM_PER_STEP;
   motors->motor[MOTOR_AXIS_ZR].float_error      = AXIS_MM_PER_STEP;
   motors->motor[MOTOR_AXIS_ZR].min_range_flag   = &endstops->endstop[Z_R_MIN].status;
   motors->motor[MOTOR_AXIS_ZR].max_range_flag   = &endstops->endstop[Z_R_MAX].status;

   motors->motor_irq = 0;
   motors->next_timer_value = DEFAULT_TIMER_COUNT;
   motors->next_timer_value_loaded = 0;
   motors->valid_irq = 0;
   motors->next_valid_irq = 0;

	init_motor(&motors->motor[MOTOR_AUX], "Aux");
	init_motor(&motors->motor[MOTOR_EXTRUDER_0], "Extd-0");
	init_motor(&motors->motor[MOTOR_EXTRUDER_1], "Extd-1");
	init_motor(&motors->motor[MOTOR_AXIS_XL], "X-LR");
	init_motor(&motors->motor[MOTOR_AXIS_YF], "Y-FB");
	init_motor(&motors->motor[MOTOR_AXIS_ZL], "ZL-UD");
	init_motor(&motors->motor[MOTOR_AXIS_ZR], "ZR-UD");
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

void init_endstops(struct cnc_endstop_list_struct* endstops){
   endstops->new_event = 0;
   endstops->endstop[0].enabled = 1;
   endstops->endstop[0].pin = ENDSTOP_0_PINBIT;
   endstops->endstop[0].port = ENDSTOP_0_PINPORT;

   endstops->endstop[1].enabled = 1;
   endstops->endstop[1].pin = ENDSTOP_1_PINBIT;
   endstops->endstop[1].port = ENDSTOP_1_PINPORT;

   endstops->endstop[2].enabled = 1;
   endstops->endstop[2].pin = ENDSTOP_2_PINBIT;
   endstops->endstop[2].port = ENDSTOP_2_PINPORT;

   endstops->endstop[3].enabled = 1;
   endstops->endstop[3].pin = ENDSTOP_3_PINBIT;
   endstops->endstop[3].port = ENDSTOP_3_PINPORT;

   endstops->endstop[4].enabled = 1;
   endstops->endstop[4].pin = ENDSTOP_4_PINBIT;
   endstops->endstop[4].port = ENDSTOP_4_PINPORT;

   endstops->endstop[5].enabled = 1;
   endstops->endstop[5].pin = ENDSTOP_5_PINBIT;
   endstops->endstop[5].port = ENDSTOP_5_PINPORT;

   endstops->endstop[6].enabled = 1;
   endstops->endstop[6].pin = ENDSTOP_6_PINBIT;
   endstops->endstop[6].port = ENDSTOP_6_PINPORT;

   endstops->endstop[7].enabled = 1;
   endstops->endstop[7].pin = ENDSTOP_7_PINBIT;
   endstops->endstop[7].port = ENDSTOP_7_PINPORT;

   endstops->endstop[8].enabled = 1;
   endstops->endstop[8].pin = ENDSTOP_8_PINBIT;
   endstops->endstop[8].port = ENDSTOP_8_PINPORT;

   endstops->endstop[9].enabled = 1;
   endstops->endstop[9].pin = ENDSTOP_9_PINBIT;
   endstops->endstop[9].port = ENDSTOP_9_PINPORT;

   endstops->endstop[10].enabled = 1;
   endstops->endstop[10].pin = ENDSTOP_10_PINBIT;
   endstops->endstop[10].port = ENDSTOP_10_PINPORT;

   endstops->endstop[11].enabled = 1;
   endstops->endstop[11].pin = ENDSTOP_11_PINBIT;
   endstops->endstop[11].port = ENDSTOP_11_PINPORT;

   endstops->empty_endstop = 0;

   for(int i=0;i<NUM_ENDSTOPS;i++){
      check_endstop(&endstops->endstop[i]);
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
      cnc_gpio_write(motor->pins.dir_pin, motor->ports.dir_port, 0);
      motor->direction = direction;
   }
   if(direction == MOTOR_MOVE_INCREASE){
      cnc_gpio_write(motor->pins.dir_pin, motor->ports.dir_port, 1);
      motor->direction = direction;
   }
}

void handle_motors(struct cnc_state_struct* cnc){
   if(cnc->motors->motor_irq){
      cnc->motors->motor_irq = 0;
      if(cnc->motors->valid_irq){
         cnc->motors->next_timer_value_loaded = 0;
         cnc->motors->valid_irq = 0;
         set_step(cnc->motors);
         process_motors(cnc->motors);
         check_endstops(cnc);
      }
   }
   if(cnc->motors->next_timer_value_loaded == 0){
      get_motor_speed(cnc->motors);
      get_next_timer_value(cnc->motors);
      cnc->motors->next_timer_value_loaded = 1;
   }
}

void set_step(struct cnc_motor_list_struct* motors){
   for(int i=0;i<NUM_MOTORS;i++){
      if(motors->motor[i].active){
         if(((motors->motor[i].position > motors->motor[i].target) || motors->motor[i].find_zero) && !motors->motor[i].find_max){
            motors->motor[i].direction = MOTOR_MOVE_DECREASE;
         }
         if(((motors->motor[i].position < motors->motor[i].target) || motors->motor[i].find_max) && !motors->motor[i].find_zero){
            motors->motor[i].direction = MOTOR_MOVE_INCREASE;
         }
         if((fabs(motors->motor[i].position - motors->motor[i].target) >= motors->motor[i].float_error) || motors->motor[i].find_max || motors->motor[i].find_zero){
            motors->motor[i].next_step_count = motors->motor[i].next_step_count - motors->next_timer_value;
            
            if(motors->motor[i].next_step_count <= MIN_TIMER_COUNT){
               motors->motor[i].step_set = 1;
               motors->motor[i].last_timer_error = motors->motor[i].next_step_count;
               motors->motor[i].step_count_set = 0;
               motors->motor[i].next_step_count = 0;
            } else {
               motors->motor[i].step_set = 0;
               motors->motor[i].last_timer_error = 0;
            }
         }
      } else {
         motors->motor[i].step_set = 0;
      }
   }
}

void get_motor_speed(struct cnc_motor_list_struct* motors){
   for(int i=0;i<NUM_MOTORS;i++){
      if(motors->motor[i].active){
         if(motors->motor[i].find_max || motors->motor[i].find_zero){
            motors->motor[i].speed = motors->motor_speed;
         } else {
            motors->motor[i].speed = motors->motor_speed * motors->motor[i].distance;
            motors->motor[i].speed = motors->motor[i].speed / motors->max_distance;
         }
      }
   }
}

void get_next_timer_value(struct cnc_motor_list_struct* motors){
   uint32_t smallest_count = 4294967295;
   uint8_t count_set = 0;
   motors->next_timer_value = DEFAULT_TIMER_COUNT;
   cnc_double steps_per_sec;
   cnc_double temp;
   cnc_motor_struct *this_motor;
   
   for(int i=0;i<NUM_MOTORS;i++){
      this_motor = &motors->motor[i];
      if(this_motor->active && !this_motor->step_count_set){
         steps_per_sec = this_motor->steps_per_mm * this_motor->speed; // steps per sec
         temp = USEC_PER_SEC / steps_per_sec; // usec per step
         this_motor->total_timer_count = (uint32_t) (temp / MOTOR_TIMER_PERIOD_US);
         this_motor->next_step_count = this_motor->total_timer_count;
         this_motor->step_count_set = 1;
         if(this_motor->last_timer_error > 0){
            this_motor->next_step_count = this_motor->next_step_count + this_motor->last_timer_error;
            this_motor->last_timer_error = 0;
         }
      }
      if(this_motor->active && this_motor->step_count_set){
         if(this_motor->next_step_count <= smallest_count){
            smallest_count = this_motor->next_step_count;
            count_set = 1;
         }
      }
   }

   if(count_set){
      motors->next_timer_value = smallest_count;
      motors->next_timer_value_loaded = 1;
   } else {
      motors->next_timer_value = DEFAULT_TIMER_COUNT;
      motors->next_timer_value_loaded = 0;
   }
   
   /*for(int i=0;i<NUM_MOTORS;i++){
      if(motors->motor[i].active){
         if(motors->motor[i].next_step_count <= motors->next_timer_value){
            // time for motor to move and then find next step
            motors->motor[i].step_count_set = 0;
            motors->motor[i].next_step_count = 0;
         } else {
            motors->motor[i].next_step_count = motors->motor[i].next_step_count - motors->next_timer_value;
         }
      }
   }*/

}

void check_endstop(struct cnc_endstop_struct* endstop){
   uint8_t endstop_status;
   if(endstop->enabled){
      endstop_status = cnc_gpio_read(endstop->pin, endstop->port);
   } else {
      endstop_status = ENDSTOP_NOT_HIT;
   }
   if(endstop_status == ENDSTOP_NOT_HIT){
		// switch not hit
		endstop->status = 0;
	} else {
		// switch hit or no power to switch
		endstop->status = 1;
	}
}

void check_endstops(struct cnc_state_struct* cnc){
   for(int i=0;i<NUM_ENDSTOPS;i++){
      cnc->endstops->endstop[i].previous_status = cnc->endstops->endstop[i].status;
      check_endstop(&cnc->endstops->endstop[i]);
      cnc->endstops->new_event |= (cnc->endstops->endstop[i].status && !cnc->endstops->endstop[i].previous_status);
   }
}

void handle_step(struct cnc_motor_struct* motor){
   if(motor->active && motor->step_set){
      motor->step_set = 0;
      step_motor_clear_step(motor);
      if(motor->direction == MOTOR_MOVE_INCREASE){
         if(*motor->max_range_flag){
            cnc_printf(&cnc, "Hit Max %s", motor->name);
            //motor->find_max = 0;
            //motor->active = 0;
         } else {
            // step motor, adjust position, set flag to clear rising edge next loop
            set_motor_direction(motor, MOTOR_MOVE_INCREASE);
            motor->position = motor->position + motor->mm_per_step;
            step_motor_set_step(motor);
         }
      } else {
         if(*motor->min_range_flag){
            // min endstop hit
            //motor->find_zero = 0;
            //motor->active = 0;
            cnc_printf(&cnc, "Hit Min %s", motor->name);
         } else {
            // step motor, adjust position, set flag to clear rising edge next loop
            set_motor_direction(motor, MOTOR_MOVE_DECREASE);
            motor->position = motor->position - motor->mm_per_step;
            step_motor_set_step(motor);
         }
      }
   }
}

void process_motors(struct cnc_motor_list_struct* motors){
   for(int i=0;i<NUM_MOTORS;i++){
      if(motors->motor[i].active){
         handle_step(&motors->motor[i]);
      }
   }
}

void step_motor_set_step(struct cnc_motor_struct* motor){
   cnc_gpio_write(motor->pins.step_pin, motor->ports.step_port, 1);
}

void step_motor_clear_step(struct cnc_motor_struct* motor){
   cnc_gpio_write(motor->pins.step_pin, motor->ports.step_port, 0);
}
