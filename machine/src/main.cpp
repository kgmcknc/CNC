#include "em_device.h"
#include "em_chip.h"
#include "em_usart.h"
#include "em_gpio.h"
#include "em_adc.h"

#include "main.h"
#include "cnc_gpio.h"
#include "cnc_spi.h"
#include "cnc_functions.h"
#include "cnc_instructions.h"
#include "cnc_motors.h"
#include "cnc_adc.h"
#include "cnc_heaters.h"
#include "cnc_timers.h"

struct cnc_state_struct cnc;
struct cnc_spi_struct cnc_spi;
struct cnc_instruction_struct cnc_instruction, cnc_instruction_next;
struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];
uint32_t instruction_array_fullness = 0, instruction_array_wp = 0, instruction_array_rp = 0;

int main(void)
{
	struct cnc_heater_list_struct cnc_heaters;
	struct cnc_motor_list_struct cnc_motors;

	/* Chip errata */
	CHIP_Init();

	cnc.motors = &cnc_motors;
	cnc.heaters = &cnc_heaters;

	system_init(&cnc_spi, cnc.motors);
	variable_init(&cnc, &cnc_spi, cnc.motors);

	//cnc.motors->xl_axis.period = 20;
	//cnc.motors->yf_axis.period = 20;
	//cnc.motors.zl_axis.period = 200;
	//cnc.motors.zr_axis.period = 200;
	//set_motor_direction(&cnc.motors->xl_axis, MOTOR_MOVE_RIGHT);
	//set_motor_direction(&cnc.motors->yf_axis, MOTOR_MOVE_FORWARD);
	//set_motor_direction(&cnc.motors->zl_axis, MOTOR_MOVE_UP);
	//set_motor_direction(&cnc.motors->zr_axis, MOTOR_MOVE_UP);
	//cnc.motors->xl_axis.active = 1;
	//cnc.motors->yf_axis.active = 1;
	//cnc.motors.zl_axis.active = 1;
	//cnc.motors.zr_axis.active = 1;
	//enable_axis_motors();
	//enable_motor(&cnc.motors.xl_axis);
	//enable_motor(&cnc.motors.yf_axis);
	//enable_motor(&cnc.motors.zl_axis);
	//enable_motor(&cnc.motors.zr_axis);

	cnc_printf(&cnc, "System Initialized");

	/* Infinite loop */
	while (1) {
		handle_state(&cnc);
		handle_spi(&cnc_spi, &cnc);
		handle_instructions(&cnc);
		handle_program(&cnc);
		handle_motors(&cnc);
		handle_heaters(&cnc_heaters);
	}
}

void system_init(cnc_spi_struct* spi_struct, cnc_motor_list_struct* cnc_motors){
	init_clocks();
	init_spi(spi_struct);
	init_gpio();
	init_adc();
	init_timers(cnc_motors);

	// init timer with interrupt for hotend pid loop
	// init timer with interrupt for motor movements
}

void init_clocks(void){
	CMU_ClockEnable(cmuClock_HFPER, true);
	CMU_ClockEnable(cmuClock_GPIO, true);

	/* Switch HFCLK to HFXO and disable HFRCO */
	CMU_ClockSelectSet(cmuClock_HF, cmuSelect_HFXO);
	CMU_OscillatorEnable(cmuOsc_HFRCO, false, false);
}

void variable_init(cnc_state_struct* cnc, cnc_spi_struct* spi_struct, cnc_motor_list_struct* cnc_motors){
	init_motors(cnc_motors);
	init_instructions(cnc);
	init_cnc(cnc);
	//init_pid();
}
