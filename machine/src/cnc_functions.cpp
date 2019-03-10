/*
 * cnc_functions.cpp
 *
 *  Created on: Aug 21, 2018
 *      Author: Kyle
 */

#include "cnc_functions.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

void init_cnc(struct cnc_state_struct* cnc){
	uint8_t print_count = 0;


	cnc->state = cnc_idle;
	cnc->end_of_program_read = 0;
	cnc->print_rp = 0;
	cnc->print_wp = 0;
	cnc->print_fullness = 0;

	for(print_count=0;print_count<PRINT_DEPTH;print_count++){
		cnc->print_buffer[print_count][0] = 0; // use % to mark end of print string
	}

}
uint16_t spi_counter = 0;
char spi_data[MAX_SPI_TRANSFER];
char set_write = 0;
void handle_state(struct cnc_state_struct* cnc){
	if(cnc->print_fullness > 0){
		cnc->request_print = 1;
	} else {
		cnc->request_print = 0;
	}
	int32_t value = spi_check_read(spi_data);
	if(value >= 0){
		spi_set_read();
		if(value > 0) set_write = 1;
	}
	if(spi_check_write() >= 0){
		if(set_write){
			if(strlen(spi_data) > 0){
				spi_set_write(spi_data, (uint16_t) strlen(spi_data));
			}
			set_write = 0;
		}
		/*sprintf(spi_data, "Spi Slave Test: %u Count", spi_counter);
		spi_counter++;
		spi_set_write(spi_data, (uint16_t) strlen(spi_data));*/
	}
}

void cnc_printf(struct cnc_state_struct* cnc, const char* print_string, ...){
	va_list print_args;
	char full_string[MAX_PRINT_LENGTH];

	va_start(print_args, print_string);
	vsnprintf(full_string, MAX_PRINT_LENGTH, print_string, print_args);
	va_end(print_args);

	strcpy(cnc->print_buffer[cnc->print_wp], full_string);
	cnc->print_wp = (cnc->print_wp < (PRINT_DEPTH-1)) ? (cnc->print_wp + 1) : 0;
	cnc->print_fullness++;
}

