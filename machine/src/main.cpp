#include "em_device.h"
#include "em_chip.h"
#include "em_usart.h"
#include "em_gpio.h"
#include "em_adc.h"

#include "main.h"
#include "revision.h"
#include "cnc_gpio.h"
#include "cnc_spi.h"
#include "cnc_functions.h"
#include "cnc_instructions.h"
#include "cnc_motors.h"
#include "cnc_adc.h"
#include "cnc_heaters.h"
#include "cnc_timers.h"

struct cnc_state_struct cnc;
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

	system_init(cnc.motors);
	variable_init(&cnc, cnc.motors);

	cnc_printf(&cnc, "System Initialized");
	//cnc_printf(&cnc, "This is 256 character string: asdfjkl;asdfjkl;asdfjkl;asdfjkl;asdfjkl;asdfjkl;asdfjkl;asdfjkl;asdfjk;ltestingiamtestingtestingiamtestingtestingiamtestingtestingiamtestingGGGGGSSDFSDFSDFSDTITITITITITITITSDFLKJSFLKJSFDTHISISSOCLOSEANDIAMALMOSTTHERENOWIGOTIT!");
	                //1234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456789012345678901234567890123456
                    //1        10        20        30        40        50        60        70        80        90        100       110       120       130       140       150       160       170       180       190       200       210       220       230       240       250
	/* Infinite loop */
	while (1) {
		cnc.spi_connected = handle_spi();
		handle_state(&cnc);
		handle_instructions(&cnc);
		check_errors(&cnc);
	}
}

void system_init(cnc_motor_list_struct* cnc_motors){
	init_clocks();
	init_spi_driver();
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

void variable_init(cnc_state_struct* cnc, cnc_motor_list_struct* cnc_motors){
	init_motors(cnc_motors);
	init_instructions(cnc);
	init_cnc(cnc);
	init_config(&cnc->config);
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
