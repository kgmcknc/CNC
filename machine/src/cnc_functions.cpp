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

void handle_state(struct cnc_state_struct* cnc){
	if(cnc->print_fullness > 0){
		cnc->request_print = 1;
	} else {
		cnc->request_print = 0;
	}
}

void cnc_printf(struct cnc_state_struct* cnc, const char* print_string, ...){
	va_list print_args;
	char full_string[PRINT_LENGTH];

	va_start(print_args, print_string);
	vsnprintf(full_string, PRINT_LENGTH, print_string, print_args);
	va_end(print_args);

	strcpy(cnc->print_buffer[cnc->print_wp], full_string);
	cnc->print_wp = (cnc->print_wp < (PRINT_DEPTH-1)) ? (cnc->print_wp + 1) : 0;
	cnc->print_fullness++;
}
