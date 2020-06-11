
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
//struct cnc_instruction_struct cnc_instruction, cnc_instruction_next;
//struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];

struct cnc_heater_list_struct cnc_heaters;
struct cnc_motor_list_struct cnc_motors;
struct cnc_endstop_list_struct cnc_endstops;

serial_class cnc_serial(0);

int cnc_main(void)
{
	
   #ifdef SILABS
      /* Chip errata */
	   CHIP_Init();
   #else

   #endif

	variable_init();
	system_init();

   Serial.begin(115200);
   while (!Serial) {
      ; // wait for serial port to connect. Needed for native USB port only
   }
   
   cnc_printf("Micro Controller Here!");
   /* Infinite loop */
	while (1) {
      cnc_serial.process();
      handle_state();
      check_instruction();
      check_instruction_fifo();
      handle_heaters();
      handle_motors();
   }
   
   return 0;
}

void system_init(void){
	init_clocks();
	init_gpio();
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

void variable_init(void){
   init_heaters();
   init_endstops();
	init_motors();
	init_instructions();
	init_cnc();
}
