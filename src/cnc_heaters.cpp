/*
 * cnc_heaters.cpp
 *
 *  Created on: Sep 13, 2018
 *      Author: Kyle
 */

#include "Arduino.h"
#include "stdint.h"
#include "cnc_heaters.h"
#include "cnc_gpio.h"

#include "cnc_motors.h"
#include "cnc_timers.h"
#include "cnc_functions.h"
#include "math.h"

uint8_t duty_count = 0;

#define P_VAL ((cnc_double) 0.05)
#define I_VAL ((cnc_double) 0.0001)
#define D_VAL ((cnc_double) 0.5)
#define MAX_ADJUSTMENTS 256
#define PID_PRINTS

/*
working temp backup
cnc_double current_voltage;
cnc_double current_resistance;
cnc_double log_val;
current_voltage = ((cnc_double) IN_VOLT*cnc->heaters->heater[i].adc_value)/ADC_MAX;
if(current_voltage > 0.0){
   current_resistance = (((cnc_double) IN_VOLT*BASE_RESISTANCE)/current_voltage)-BASE_RESISTANCE;
   if(current_resistance > 0){
      log_val = (cnc_double) log(BASE_RESISTANCE/current_resistance);
      cnc->heaters->heater[i].current_temp = ((cnc_double) (log_val*(KELVIN_CONV*(BASE_TEMP+KELVIN_CONV)))+(BASE_TEMP*BETA_VALUE))/(BETA_VALUE-(log_val*(BASE_TEMP+KELVIN_CONV)));
   }
}
*/
//#define CALC_TEMP
void handle_heaters(struct cnc_state_struct* cnc){
   if(cnc->heaters->heater_irq){// && !cnc->motors->motor_irq){
      cnc->heaters->heater_irq = 0;

      // handle heaters
      for(int i=0;i<NUM_HEATERS;i++){
         // update fan adc value
         read_heater_adc(&cnc->heaters->heater[i]);
      }

      for(int i=0;i<NUM_HEATERS;i++){
         if(cnc->heaters->heater[i].pid_reset){
            reset_heater_pid(&cnc->heaters->heater[i]);
         }
      }

      for(int i=0;i<NUM_HEATERS;i++){
         cnc->heaters->heater[i].current_temp = (cnc_double) cnc->heaters->heater[i].adc_value;
         if(cnc->heaters->heater[i].heater_active){
            update_heater_pid(&cnc->heaters->heater[i]);
         } else {
            if(cnc->heaters->heater[i].heater_on){
               cnc_gpio_write(cnc->heaters->heater[i].heater_pin, cnc->heaters->heater[i].heater_port, 0);
               cnc->heaters->heater[i].heater_on = 0;
            }
         }
         
         #ifdef CALC_TEMP
         current_voltage = ((cnc_double) IN_VOLT*)/ADC_MAX;
         if(current_voltage > 0.0){
            current_resistance = (((cnc_double) IN_VOLT*BASE_RESISTANCE)/current_voltage)-BASE_RESISTANCE;
            if(current_resistance > 0){
               log_val = (cnc_double) log(BASE_RESISTANCE/current_resistance);
               cnc->heaters->heater[i].current_temp = ((cnc_double) (log_val*(KELVIN_CONV*(BASE_TEMP+KELVIN_CONV)))+(BASE_TEMP*BETA_VALUE))/(BETA_VALUE-(log_val*(BASE_TEMP+KELVIN_CONV)));
            }
         }
         #endif
      }
      
      // handle fans
      for(int i=0;i<NUM_HEATERS;i++){
         if(cnc->heaters->heater[i].fan_active){
            if(cnc->heaters->heater[i].fan_on){
               if(duty_count >= cnc->heaters->heater[i].fan_duty){
                  cnc->heaters->heater[i].fan_on = 0;
                  cnc_gpio_write(cnc->heaters->heater[i].fan_pin, cnc->heaters->heater[i].fan_port, 0);
               }
            } else {
               if(duty_count == 0){
                  cnc_gpio_write(cnc->heaters->heater[i].fan_pin, cnc->heaters->heater[i].fan_port, 1);
                  cnc->heaters->heater[i].fan_on = 1;
               }
            }
         } else {
            if(cnc->heaters->heater[i].adc_value > SHUTOFF_ADC){
               if(cnc->heaters->heater[i].fan_on == 0){
                  cnc->heaters->heater[i].fan_on = 1;
                  cnc_gpio_write(cnc->heaters->heater[i].fan_pin, cnc->heaters->heater[i].fan_port, 1);
               }
            } else {
               if(cnc->heaters->heater[i].fan_on){
                  cnc->heaters->heater[i].fan_on = 0;
                  cnc_gpio_write(cnc->heaters->heater[i].fan_pin, cnc->heaters->heater[i].fan_port, 0);
               }
            }
         }
      }
      duty_count = duty_count + 1;
      if(duty_count >= 100){
         duty_count = 0;
      }
   }
}

void init_heaters(struct cnc_heater_list_struct* heaters){
   heaters->heater[0].heater_pin     = HEAT_0_PIN;
   heaters->heater[1].heater_pin     = HEAT_1_PIN;
   heaters->heater[2].heater_pin     = HEAT_2_PIN;
   heaters->heater[3].heater_pin     = HEAT_3_PIN;
   
   heaters->heater[0].fan_pin        = FANS_0_PIN;
   heaters->heater[1].fan_pin        = FANS_1_PIN;
   heaters->heater[2].fan_pin        = FANS_2_PIN;
   heaters->heater[3].fan_pin        = FANS_3_PIN;

   heaters->heater[0].adc_pin        = TEMP_ADC_0_PIN;
   heaters->heater[1].adc_pin        = TEMP_ADC_1_PIN;
   heaters->heater[2].adc_pin        = TEMP_ADC_2_PIN;
   heaters->heater[3].adc_pin        = TEMP_ADC_3_PIN;

   heaters->heater_irq = 0;
   heaters->update_counter = 0;
   for(int i=0;i<NUM_HEATERS;i++){
      init_heater(&heaters->heater[i]);
   }
}

void init_heater(struct cnc_heater_struct* heater){
   heater->fan_on = 0;
   heater->fan_duty = 0;
   heater->heater_active = 0;
   heater->fan_active = 0;
   heater->cooldown_active = 0;
   heater->reset_heater = 0;
   heater->temp_locked = 0;
   heater->wait_for_temp = 0;
   heater->target_temp = 0.0;
   heater->current_temp = 0.0;
   heater->adc_value = 0;
   heater->target_adc = 0;
   heater->anti_windup = 0;
   heater->pid_reset = 0;
   heater->last_p_error = 0;
   heater->last_adj = 0.0;
   heater->i_error = 0;
   heater->d_sign = 0;
   heater->windup_rounds = 0;
   heater->adj_count = 0;
   cnc_gpio_write(heater->heater_pin, heater->heater_port, 0);
   cnc_gpio_write(heater->fan_pin, heater->fan_port, 0);
}

void enable_heater_fans(void){
	//GPIO_PinOutSet(FANS_PORT, FANS_PIN);
}

void disable_heater_fans(void){
	//GPIO_PinOutClear(FANS_PORT, FANS_PIN);
}

void read_heater_adc(struct cnc_heater_struct* heater){
   heater->adc_value = analogRead(heater->adc_pin);
}

void reset_heater_pid(struct cnc_heater_struct* heater){
   heater->pid_reset = 0;
   heater->average_pointer = 0;
   heater->last_p_error = 0;
   heater->last_adj = 0.0;
   heater->i_error = 0;
   heater->d_sign = 0;
   heater->anti_windup = 1;
   heater->windup_rounds = WINDUP_COUNT;
   heater->adj_count = 0;
   for(int i=0;i<AVERAGE_COUNT;i++){
      heater->pid_average[i] = (int16_t) 65535;
   }
}

void update_heater_pid(struct cnc_heater_struct* heater){
   int16_t p_average;
   int16_t p_error;
   int16_t d_error;
   cnc_double adj;

   p_error = heater->adc_value - heater->target_adc;
   d_error = p_error - heater->last_p_error;

   //if(abs(p_error) > 200){
   //   heater->anti_windup = 1;
   //} else {
   //   heater->anti_windup = 0;
   //}
   if(heater->anti_windup == 0){
      heater->i_error = heater->i_error + p_error;
   }

   heater->pid_average[heater->average_pointer] = p_error;
   heater->average_pointer = heater->average_pointer + 1;
   if(heater->average_pointer >= AVERAGE_COUNT){
      heater->average_pointer = 0;
   }

   p_average = 0;
   for(int i=0;i<AVERAGE_COUNT;i++){
      p_average = p_average + heater->pid_average[i];
   }
   p_average = p_average / AVERAGE_COUNT;

   adj = ((cnc_double) P_VAL*p_error) + ((cnc_double) I_VAL*heater->i_error) + ((cnc_double) D_VAL*d_error);
   
   if(adj < 0.0){
      cnc_gpio_write(heater->heater_pin, heater->heater_port, 1);
      heater->heater_on = 1;
      //if(heater->adj_count < MAX_ADJUSTMENTS){
         heater->adj_count = heater->adj_count + 1;
      //}
   } else {
      cnc_gpio_write(heater->heater_pin, heater->heater_port, 0);
      heater->heater_on = 0;
      if(heater->adj_count > 0){
         heater->adj_count = heater->adj_count - 1;
      }
   }

   if(heater->windup_rounds){
      heater->windup_rounds = heater->windup_rounds - 1;
   } else {
      if(heater->anti_windup){
         if(heater->last_p_error > 0.0){
            if(p_error < 0.0){
               cnc_printf(&cnc, "done anti windup now neg");
               heater->anti_windup = 0;
            }
         } else {
            if(p_error > 0.0){
               cnc_printf(&cnc, "done anti windup now pos");
               heater->anti_windup = 0;
            }
         }
      }
   }

   #ifdef PID_PRINTS
      cnc_printf(&cnc, "PID:%d,%d,%d,%d,%d,%d", p_error,(int16_t) heater->i_error,d_error,(int16_t) adj,heater->adj_count,heater->anti_windup);
   #endif

   heater->last_p_error = p_error;
   heater->last_adj = adj;
}
