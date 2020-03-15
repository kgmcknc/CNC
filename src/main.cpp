
#include "main.h"

#ifdef SILABS
   #include "em_device.h"
   #include "em_chip.h"
   #include "em_usart.h"
   #include "em_gpio.h"
   #include "em_adc.h"
#else
   #include "Arduino.h"
#endif

#include "stdint.h"

#include "cnc_gpio.h"
#include "cnc_serial.h"
#include "cnc_pid.h"
#include "cnc_functions.h"
#include "cnc_instructions.h"
#include "cnc_motors.h"
#include "cnc_adc.h"
#include "cnc_heaters.h"
#include "cnc_timers.h"
#include "math.h"

struct cnc_state_struct cnc;
struct cnc_instruction_struct cnc_instruction, cnc_instruction_next;
struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];
uint32_t instruction_array_fullness = 0, instruction_array_wp = 0, instruction_array_rp = 0;

struct cnc_heater_list_struct cnc_heaters;
struct cnc_motor_list_struct cnc_motors;

serial_class cnc_serial(0);

int cnc_main(void)
{
	
   #ifdef SILABS
      /* Chip errata */
	   CHIP_Init();
   #else

   #endif

	cnc.motors = &cnc_motors;
	cnc.heaters = &cnc_heaters;
   
	system_init(cnc.motors);
	variable_init(&cnc, cnc.motors);

   Serial.begin(57600);
   while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
   }
   
   /* Infinite loop */
	while (1) {
      cnc_serial.process();
      handle_state(&cnc);
      handle_instructions(&cnc);
   }
   /*char tx_string[32] = "Micro Is Here!";
   char rx_string[32];
   uint8_t send_one = 1;
   uint32_t send_count = 0;
   while(1){
      //check_endstops(cnc.motors);
      cnc_serial.process();
      if(cnc_serial.is_connected){
         sprintf(tx_string, "Micro Is Here! %d", send_count);
         if(cnc_serial.send(32, tx_string)){
            send_count = send_count + 1;
         }
      }
      if(cnc_serial.rx_queue_fullness > 0){
         cnc_serial.receive((uint8_t*) rx_string);
      }
      if(pid_irq){
         pid_irq = 0;
         step_motor_high(&cnc.motors->xl_axis);
      } else {
         step_motor_low(&cnc.motors->xl_axis);
      }
      if(motor_irq){
         motor_irq = 0;
         step_motor_high(&cnc.motors->yf_axis);
      } else {
         step_motor_low(&cnc.motors->yf_axis);
      }
      if(cnc.motors->xl_axis.max_range_flag){
         set_motor_direction(&cnc.motors->xl_axis, MOTOR_MOVE_DECREASE);
      }
      if(cnc.motors->xl_axis.min_range_flag){
         set_motor_direction(&cnc.motors->xl_axis, MOTOR_MOVE_INCREASE);
      }
      if(cnc.motors->yf_axis.max_range_flag){
         set_motor_direction(&cnc.motors->yf_axis, MOTOR_MOVE_DECREASE);
      }
      if(cnc.motors->yf_axis.min_range_flag){
         set_motor_direction(&cnc.motors->yf_axis, MOTOR_MOVE_INCREASE);
      }
   }*/

   return 0;
}

void system_init(cnc_motor_list_struct* cnc_motors){
	//init_clocks();
	//init_gpio();
	//init_adc();
	init_timers();

	// init timer with interrupt for hotend pid loop
	// init timer with interrupt for motor movements
}

void init_clocks(void){
   #ifdef SILABS
      CMU_ClockEnable(cmuClock_HFPER, true);
      CMU_ClockEnable(cmuClock_GPIO, true);

      //Switch HFCLK to HFXO and disable HFRCO
      CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
      CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
   #else

   #endif
}

void variable_init(cnc_state_struct* cnc, cnc_motor_list_struct* cnc_motors){
	init_motors(cnc_motors);
	//init_instructions(cnc);
	//init_cnc(cnc);
	//init_config(&cnc->config);
	//init_pid();
}

void init_config(cnc_config_struct* config){
	config->config_loaded = 0;
	config->valid_config = 0;
	config->max_speed = 0;
	config->min_speed = 0;
	config->ramp_period = 0;
	config->xl_min_safe_pos = 0;
	config->xr_max_safe_pos = 0;
	config->yf_min_safe_pos = 0;
	config->yb_max_safe_pos = 0;
	config->zl_min_safe_pos = 0;
	config->zl_max_safe_pos = 0;
	config->zr_min_safe_pos = 0;
	config->zr_max_safe_pos = 0;
	config->xl_min_home_pos = 0;
	config->xr_max_home_pos = 0;
	config->yf_min_home_pos = 0;
	config->yb_max_home_pos = 0;
	config->zl_min_home_pos = 0;
	config->zl_max_home_pos = 0;
	config->zr_min_home_pos = 0;
	config->zr_max_home_pos = 0;
	config->x_axis_size = 0;
	config->y_axis_size = 0;
	config->zl_axis_size = 0;
	config->zr_axis_size = 0;
}
