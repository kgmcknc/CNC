/*
 * cnc_functions.cpp
 *
 *  Created on: Aug 21, 2018
 *      Author: Kyle
 */

#include "cnc_functions.h"
#include "revision.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

void init_cnc(struct cnc_state_struct* cnc){
	uint8_t print_count = 0;

	cnc->state = CNC_IDLE;
	cnc->end_of_program_read = 0;
	cnc->print_rp = 0;
	cnc->print_wp = 0;
	cnc->print_fullness = 0;
	cnc->write_complete = 0;
	cnc->write_in_progress = 0;
	cnc->read_complete = 0;
	cnc->read_in_progress = 0;

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
	switch(cnc->state){
		case CNC_IDLE : {
			if(cnc->request_print > 0){
				// send print if print is ready
				cnc->state = SEND_CNC_PRINT;
			} else {
				// if not printing, then process spi inputs
				if(check_spi(cnc)){
					cnc->state = PROCESS_SPI;
				} else {
					cnc->state = CNC_IDLE;
				}
			}
			break;
		}
		case PROCESS_SPI : {
			process_spi_request(cnc);
			break;
		}
		case GET_STATUS : {
			if(cnc->write_in_progress){
				if(spi_check_write() > 0){
					cnc->write_in_progress = 0;
					cnc->state = CNC_IDLE;
				}
			} else {
				cnc->cnc_write_data[0] = (char) GET_CNC_STATUS;
				parse_status(cnc);
				if(spi_set_write(cnc->cnc_write_data, cnc->cnc_write_length) > 0){
					cnc->write_in_progress = 1;
				} else {
					cnc->write_in_progress = 0;
				}
				cnc->state = GET_STATUS;
			}
			break;
		}
		case GET_VERSION : {
			if(cnc->write_in_progress){
				if(spi_check_write() > 0){
					cnc->write_in_progress = 0;
					cnc->state = CNC_IDLE;
				}
			} else {
				cnc->cnc_write_data[0] = (char) GET_CNC_VERSION;
				parse_version(cnc);
				if(spi_set_write(cnc->cnc_write_data, cnc->cnc_write_length) > 0){
					cnc->write_in_progress = 1;
				} else {
					cnc->write_in_progress = 0;
				}
				cnc->state = GET_VERSION;
			}
			break;
		}
		case SEND_CNC_PRINT : {
			if(cnc->write_in_progress){
				if(spi_check_write() > 0){
					cnc->write_in_progress = 0;
					cnc->state = CNC_IDLE;
				}
			} else {
				cnc->cnc_write_data[0] = (char) NEW_CNC_PRINT;
				parse_print(cnc);
				if(spi_set_write(cnc->cnc_write_data, cnc->cnc_write_length) > 0){
					cnc->write_in_progress = 1;
				} else {
					cnc->write_in_progress = 0;
				}
				cnc->state = SEND_CNC_PRINT;
			}
			break;
		}
		case CNC_PROGRAM_RUNNING : {
			cnc->state = CNC_IDLE;
			break;
		}
		default : {
			cnc->state = CNC_IDLE;
			break;
		}
	}
	/*int32_t value = spi_check_read(spi_data);
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
		//sprintf(spi_data, "Spi Slave Test: %u Count", spi_counter);
		spi_counter++;
		spi_set_write(spi_data, (uint16_t) strlen(spi_data));
	}*/
}

uint8_t check_spi(struct cnc_state_struct* cnc){
	// always check write to complete transfer if done
	spi_check_write();
	// check if read is finished
	cnc->cnc_read_length = spi_check_read(cnc->cnc_read_data);
	if(cnc->cnc_read_length == 0){
		// not reading, so set read ready
		spi_set_read();
		return 0;
	} else {
		if(cnc->cnc_read_length > 0){
			// read finished, so process
			return 1;
		} else {
			return 0;
		}
	}
}

void process_spi_request(struct cnc_state_struct* cnc){
	// setting state to default...
	// it will change in switch if needed
	cnc->state = CNC_IDLE;
	switch((CNC_OPCODES) cnc->cnc_read_data[0]){
		case GET_CNC_VERSION : {
			cnc->state = GET_VERSION;
			break;
		}
		case GET_CNC_STATUS : {
			cnc->state = GET_STATUS;
			break;
		}
		case NEW_CNC_PRINT : {
			// shouldn't receive this
			break;
		}
		case FLASH_FIRMWARE : {
			// could do something to shutdown before reflash/reboot...
			break;
		}
		case START_CNC_PROGRAM : {

			break;
		}
		case END_CNC_PROGRAM : {

			break;
		}
		case NEW_CNC_INSTRUCTION : {

			break;
		}
		case INSTANT_CNC_INSTRUCTION : {

			break;
		}
		case DISABLE_ROUTE : {

			break;
		}
		case ENABLE_ROUTE : {

			break;
		}
		case START_PROGRAM : {

			break;
		}
		case PAUSE_PROGRAM : {

			break;
		}
		case RESUME_PROGRAM : {

			break;
		}
		case END_PROGRAM : {

			break;
		}
		case NEW_INSTRUCTION : {

			break;
		}
		case ERROR : {

			break;
		}
		default : {
			cnc->state = CNC_IDLE;
			break;
		}
	}
}

void parse_status(struct cnc_state_struct* cnc){
	sprintf(&cnc->cnc_write_data[1], "L:%d%d,R:%d%d,ZL:%d%d,ZR:%d%d,X:%ld,Y:%ld, T0:%f, T1:%f",
			cnc->motors->xl_axis.min_range_flag, cnc->motors->xl_axis.max_range_flag,
			cnc->motors->yf_axis.min_range_flag, cnc->motors->yf_axis.max_range_flag,
			cnc->motors->zl_axis.min_range_flag, cnc->motors->zl_axis.max_range_flag,
			cnc->motors->zr_axis.min_range_flag, cnc->motors->zr_axis.max_range_flag,
			cnc->motors->xl_axis.position,
			cnc->motors->yf_axis.position,
			cnc->heaters->heater_0.current_temp,
			cnc->heaters->heater_1.current_temp);
	cnc->cnc_write_length = strlen(cnc->cnc_write_data);
}

void parse_print(struct cnc_state_struct* cnc){
	strcpy(&cnc->cnc_write_data[1], cnc->print_buffer[cnc->print_rp]);
	cnc->print_buffer[cnc->print_rp][0] = 0;
	cnc->print_rp = (cnc->print_rp < (PRINT_DEPTH-1)) ? (cnc->print_rp + 1) : 0;
	cnc->print_fullness--;
	cnc->cnc_write_length = strlen(cnc->cnc_write_data);
}

void parse_version(struct cnc_state_struct* cnc){
	sprintf(&cnc->cnc_write_data[1], "Major: %d, Minor: %d, Beta: %d, Patch: %d",
			(int16_t) HW_REV_MAJOR, (int16_t) HW_REV_MINOR,
			(int16_t) HW_REV_BETA, (int16_t) HW_REV_PATCH);
	cnc->cnc_write_length = (strlen(&cnc->cnc_write_data[1]) + 1);
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

