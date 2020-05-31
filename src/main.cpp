
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
#include "cnc_functions.h"
#include "cnc_instructions.h"
#include "cnc_motors.h"
#include "cnc_heaters.h"
#include "cnc_timers.h"
#include "math.h"

struct cnc_state_struct cnc;
struct cnc_instruction_struct cnc_instruction, cnc_instruction_next;
struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];
uint32_t instruction_array_fullness = 0, instruction_array_wp = 0, instruction_array_rp = 0;

struct cnc_heater_list_struct cnc_heaters;
struct cnc_motor_list_struct cnc_motors;
struct cnc_endstop_list_struct cnc_endstops;

serial_class cnc_serial(0);

uint8_t toggle = 0;

int cnc_main(void)
{
	
   #ifdef SILABS
      /* Chip errata */
	   CHIP_Init();
   #else

   #endif

	cnc.motors = &cnc_motors;
	cnc.heaters = &cnc_heaters;
   cnc.endstops = &cnc_endstops;
   
	variable_init(&cnc);
	system_init(&cnc);

   Serial.begin(115200);
   while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
   }
   
   cnc_printf(&cnc, "Micro Controller Here!");
   /* Infinite loop */
	while (1) {
      if(toggle){
         toggle = 0;
         CLR(*AUX_MOTOR_DIR_PORT, AUX_MOTOR_DIR_PIN);
      } else {
         toggle = 1;
         SET(*AUX_MOTOR_DIR_PORT, AUX_MOTOR_DIR_PIN);
      }
      SET(*AUX_MOTOR_STEP_PORT, AUX_MOTOR_STEP_PIN);
      cnc_serial.process();
      CLR(*AUX_MOTOR_STEP_PORT, AUX_MOTOR_STEP_PIN);
      handle_state(&cnc);
      SET(*AUX_MOTOR_STEP_PORT, AUX_MOTOR_STEP_PIN);
      check_instruction(&cnc);
      CLR(*AUX_MOTOR_STEP_PORT, AUX_MOTOR_STEP_PIN);
      check_instruction_fifo(&cnc);
      SET(*AUX_MOTOR_STEP_PORT, AUX_MOTOR_STEP_PIN);
      handle_heaters(&cnc);
      CLR(*AUX_MOTOR_STEP_PORT, AUX_MOTOR_STEP_PIN);
      handle_motors(&cnc);
   }
   
   return 0;
}

void system_init(struct cnc_state_struct* cnc){
	init_clocks();
	init_gpio(cnc);
	init_timers();
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

void variable_init(struct cnc_state_struct* cnc){
   init_heaters(cnc->heaters);
   init_endstops(cnc->endstops);
	init_motors(cnc->motors, cnc->endstops);
	init_instructions(cnc);
	init_cnc(cnc);
}
