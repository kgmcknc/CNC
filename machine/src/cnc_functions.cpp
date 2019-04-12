/*
 * cnc_functions.cpp
 *
 *  Created on: Aug 21, 2018
 *      Author: Kyle
 */

#include "cnc_functions.h"
#include "cnc_spi.h"
#include "revision.h"
#include "stdio.h"
#include "stdarg.h"
#include "string.h"

void init_cnc(struct cnc_state_struct* cnc){
	uint8_t print_count = 0;

	cnc->state = CNC_IDLE;
	cnc->program_running = 0;
	cnc->print_rp = 0;
	cnc->print_wp = 0;
	cnc->print_fullness = 0;
	cnc->write_complete = 0;
	cnc->write_in_progress = 0;
	cnc->read_complete = 0;
	cnc->read_in_progress = 0;
	cnc->marker_set = 0;

	for(print_count=0;print_count<PRINT_DEPTH;print_count++){
		cnc->print_buffer[print_count][0] = 0; // use % to mark end of print string
	}

}

void handle_state(struct cnc_state_struct* cnc){
	if(check_spi(cnc)){
		process_spi_request(cnc);
	}
	switch(cnc->state){
		case CNC_IDLE : {
			cnc->state = CNC_IDLE;
			if((cnc->print_fullness > 0) && (!cnc->write_in_progress) && (!cnc->instruction_request_sent)){
				// send print if print is ready
				cnc->state = SEND_CNC_PRINT;
			} else {
				if(cnc->marker_set){
					cnc->state = SEND_CNC_MARKER;
				}
			}
			break;
		}
		case GET_STATUS : {
			if(cnc->write_in_progress){
				if(spi_check_write() > 0){
					cnc->write_in_progress = 0;
					cnc->state = CNC_IDLE;
				}
			} else {
				if(spi_check_write() < 0){
					// can't write, already writing
					cnc->state = GET_STATUS;
				} else {
					cnc->cnc_write_data[0] = (char) GET_CNC_STATUS;
					update_status(cnc);
					cnc->cnc_write_length = status_to_string(&cnc->status, &cnc->cnc_write_data[1]);
					cnc->cnc_write_length = cnc->cnc_write_length + 1;
					//parse_status(cnc);
					if(spi_set_write(cnc->cnc_write_data, cnc->cnc_write_length) > 0){
						cnc->write_in_progress = 1;
					} else {
						cnc->write_in_progress = 0;
					}
					cnc->state = GET_STATUS;
				}
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
				if(spi_check_write() < 0){
					// can't write, already writing
					cnc->state = GET_VERSION;
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
			}
			break;
		}
		case GET_CONFIG : {
			if(cnc->write_in_progress){
				if(spi_check_write() > 0){
					cnc->write_in_progress = 0;
					cnc->state = CNC_IDLE;
				}
			} else {
				if(spi_check_write() < 0){
					// can't write, already writing
					cnc->state = GET_CONFIG;
				} else {
					cnc->cnc_write_data[0] = (char) SET_CNC_CONFIG;
					if(spi_set_write(cnc->cnc_write_data, 1) > 0){
						cnc->write_in_progress = 1;
					} else {
						cnc->write_in_progress = 0;
					}
					cnc->state = GET_CONFIG;
				}
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
				if(spi_check_write() < 0){
					// can't write, already writing
					cnc->state = SEND_CNC_PRINT;
				} else {
					cnc->cnc_write_data[0] = (char) NEW_CNC_PRINT;
					parse_print(cnc);
					cnc->cnc_write_length = cnc->cnc_write_length + 1;
					if(spi_set_write(cnc->cnc_write_data, cnc->cnc_write_length) > 0){
						cnc->write_in_progress = 1;
					} else {
						cnc->write_in_progress = 0;
					}
					cnc->state = SEND_CNC_PRINT;
				}
			}
			break;
		}
		case SEND_CNC_MARKER : {
			if(cnc->write_in_progress){
				if(spi_check_write() > 0){
					cnc->write_in_progress = 0;
					cnc->marker_set = 0;
					cnc->state = CNC_IDLE;
				}
			} else {
				if(spi_check_write() < 0){
					// can't write, already writing
					cnc->state = SEND_CNC_MARKER;
				} else {
					cnc->cnc_write_data[0] = (char) MARKER;
					cnc->cnc_write_length = 1;
					if(spi_set_write(cnc->cnc_write_data, cnc->cnc_write_length) > 0){
						cnc->write_in_progress = 1;
					} else {
						cnc->write_in_progress = 0;
					}
					cnc->state = SEND_CNC_MARKER;
				}
			}
			break;
		}
		default : {
			cnc->state = CNC_IDLE;
			break;
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
		case SET_CNC_CONFIG : {
			string_to_config(&cnc->config, &cnc->cnc_read_data[1]);
			if(cnc->config.valid_config){
				cnc->config.config_loaded = 1;
				load_config(cnc);
			}
			cnc_printf(cnc, "Received Configuration!");
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
		case NEW_CNC_INSTRUCTION : {
			receive_instruction(cnc);
			break;
		}
		case DISABLE_ROUTE : {

			break;
		}
		case ENABLE_ROUTE : {

			break;
		}
		case PAUSE_PROGRAM : {

			break;
		}
		case RESUME_PROGRAM : {

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

void update_status(struct cnc_state_struct* cnc){
	cnc->status.xl_min_flag = cnc->motors->xl_axis.min_range_flag;
	cnc->status.xl_max_flag = cnc->motors->xl_axis.max_range_flag;
	cnc->status.yf_min_flag = cnc->motors->yf_axis.min_range_flag;
	cnc->status.yf_max_flag = cnc->motors->yf_axis.max_range_flag;
	cnc->status.zl_min_flag = cnc->motors->zl_axis.min_range_flag;
	cnc->status.zl_max_flag = cnc->motors->zl_axis.max_range_flag;
	cnc->status.zr_min_flag = cnc->motors->zr_axis.min_range_flag;
	cnc->status.zr_max_flag = cnc->motors->zr_axis.max_range_flag;
	cnc->status.xl_position = cnc->motors->xl_axis.position;
	cnc->status.yf_position = cnc->motors->yf_axis.position;
	cnc->status.zl_position = cnc->motors->zl_axis.position;
	cnc->status.zr_position = cnc->motors->zr_axis.position;
	cnc->status.heater_0_temp = cnc->heaters->heater_0.current_temp;
	cnc->status.heater_1_temp = cnc->heaters->heater_1.current_temp;
	cnc->status.heater_2_temp = cnc->heaters->heater_2.current_temp;
	cnc->status.heater_3_temp = cnc->heaters->heater_3.current_temp;
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

void load_config(struct cnc_state_struct* cnc){
	cnc->motors->xl_axis.axis_length = cnc->config.x_axis_size;
	cnc->motors->yf_axis.axis_length = cnc->config.y_axis_size;
	cnc->motors->zl_axis.axis_length = cnc->config.zl_axis_size;
	cnc->motors->zr_axis.axis_length = cnc->config.zr_axis_size;

	cnc->motors->xl_axis.safe_position = cnc->config.xl_min_safe_pos;
	cnc->motors->yf_axis.safe_position = cnc->config.yf_min_safe_pos;
	cnc->motors->zl_axis.safe_position = cnc->config.zl_min_safe_pos;
	cnc->motors->zr_axis.safe_position = cnc->config.zr_min_safe_pos;

	cnc->motors->xl_axis.home_position = cnc->config.xl_min_home_pos;
	cnc->motors->yf_axis.home_position = cnc->config.yf_min_home_pos;
	cnc->motors->zl_axis.home_position = cnc->config.zl_min_home_pos;
	cnc->motors->zr_axis.home_position = cnc->config.zr_min_home_pos;
}
