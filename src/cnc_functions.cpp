/*
 * cnc_functions.cpp
 *
 *  Created on: Aug 21, 2018
 *      Author: Kyle
 */

#include "main.h"
#include "cnc_functions.h"
#include "cnc_serial.h"
#include "stdio.h"
#include "stdarg.h"
#include "Arduino.h"
#include "math.h"
#include "string.h"

void init_cnc(void){
	uint8_t print_count = 0;

	cnc.state = CNC_IDLE;
	cnc.program.program_running = 0;
	cnc.print_rp = 0;
	cnc.print_wp = 0;
	cnc.print_fullness = 0;
	cnc.write_complete = 0;
	cnc.write_in_progress = 0;
	cnc.read_complete = 0;
	cnc.read_in_progress = 0;
   cnc.instant_instruction_done = 0;

	for(print_count=0;print_count<PRINT_DEPTH;print_count++){
		cnc.print_buffer[print_count][0] = 0; // use % to mark end of print string
	}

}

void handle_state(void){
   
	if(cnc_serial.rx_queue_fullness > 0){
      cnc.cnc_read_length = cnc_serial.receive(cnc.cnc_read_data);
		process_request();
	}
	switch(cnc.state){
		case CNC_IDLE : {
			cnc.state = CNC_IDLE;
			if((cnc.print_fullness > 0) && (!cnc_serial.tx_pending) && (!cnc.program.instruction_request_sent)){
				// send print if print is ready
				cnc.state = SEND_CNC_PRINT;
			} else {
            if(cnc.instant_instruction_done){
               cnc.state = SEND_INSTANT_INSTRUCTION_DONE;
            }
				if(cnc_endstops.new_event){
					cnc.state = SEND_ENDPOINT_EVENT;
				}
			}
			break;
		}
		case GET_STATUS : {
         cnc.cnc_write_data[0] = (char) GET_CNC_STATUS;
         update_status();
         cnc.cnc_write_length = status_to_string(&cnc.status, &cnc.cnc_write_data[1]);
         cnc.cnc_write_length = cnc.cnc_write_length + 1;
         if(cnc_serial.send(cnc.cnc_write_length, cnc.cnc_write_data) > 0){
            cnc.state = CNC_IDLE;
         }
			break;
		}
		case GET_VERSION : {
         cnc.cnc_write_data[0] = (char) GET_CNC_VERSION;
         parse_version();
         if(cnc_serial.send(cnc.cnc_write_length, cnc.cnc_write_data) > 0){
            cnc.state = CNC_IDLE;
         }
			break;
		}
		case SEND_CNC_PRINT : {
         cnc.cnc_write_data[0] = (char) NEW_CNC_PRINT;
         parse_print();
         cnc.cnc_write_length = cnc.cnc_write_length + 1;
         if(cnc_serial.send(cnc.cnc_write_length, cnc.cnc_write_data) > 0){
            cnc.state = CNC_IDLE;
         }
			break;
		}
		case SEND_ENDPOINT_EVENT : {
         cnc.cnc_write_data[0] = (char) MARKER;
         update_status();
         cnc.cnc_write_length = status_to_string(&cnc.status, &cnc.cnc_write_data[1]);
         cnc.cnc_write_length = cnc.cnc_write_length + 1;
         if(cnc_serial.send(cnc.cnc_write_length, cnc.cnc_write_data) > 0){
            cnc_endstops.new_event = 0;
            cnc.state = CNC_IDLE;
         }
			break;
		}
      case SEND_INSTANT_INSTRUCTION_DONE : {
         cnc.cnc_write_data[0] = (char) INSTANT_DONE;
         cnc.cnc_write_length = 1;
         if(cnc_serial.send(cnc.cnc_write_length, cnc.cnc_write_data) > 0){
            cnc.instant_instruction_done = 0;
            cnc.state = CNC_IDLE;
         }
			break;
		}
		default : {
			cnc.state = CNC_IDLE;
			break;
		}
	}
}

void process_request(void){
	// setting state to default...
	// it will change in switch if needed
	
	switch((CNC_OPCODES) cnc.cnc_read_data[0]){
		case GET_CNC_VERSION : {
			cnc.state = GET_VERSION;
			break;
		}
		case GET_CNC_STATUS : {
			cnc.state = GET_STATUS;
			break;
		}
		/*case SET_CNC_CONFIG : {
			string_to_config(&cnc.config, &cnc.cnc_read_data[1]);
			if(cnc.config.valid_config){
				cnc.config.config_loaded = 1;
				load_config(cnc);
			}
			cnc_printf(cnc, "Received Configuration!");
			cnc.state = CNC_IDLE;
			break;
		}*/
		case NEW_CNC_PRINT : {
			// shouldn't receive this
			break;
		}
		case FLASH_FIRMWARE : {
			// could do something to shutdown before reflash/reboot...
			break;
		}
		case NEW_CNC_INSTRUCTION : {
			receive_instruction();
			break;
		}
		default : {
			cnc.state = CNC_IDLE;
			break;
		}
	}
}

void update_status(void){
   for(int i=0;i<NUM_MOTORS;i++){
      cnc.status.position[i] = cnc_motors.motor[i].position;
      cnc.status.target[i] = cnc_motors.motor[i].target;
   }
   for(int i=0;i<NUM_ENDSTOPS;i++){
      cnc.status.endstop_status[i] = cnc_endstops.endstop[i].status;
   }
   for(int i=0;i<NUM_HEATERS;i++){
      cnc.status.temp[i] = cnc_heaters.heater[i].adc_value;
      cnc.status.temp_locked[i] = cnc_heaters.heater[i].temp_locked;
   }
   cnc.status.instruction_active = cnc.program.current_instruction->instruction_valid;
}

void parse_print(void){
	strcpy((char*) &cnc.cnc_write_data[1], (char*) cnc.print_buffer[cnc.print_rp]);
	cnc.print_buffer[cnc.print_rp][0] = 0;
	cnc.print_rp = (cnc.print_rp < (PRINT_DEPTH-1)) ? (cnc.print_rp + 1) : 0;
	cnc.print_fullness--;
	cnc.cnc_write_length = strlen((char*) cnc.cnc_write_data);
}

void parse_version(void){
	sprintf((char*) &cnc.cnc_write_data[1], "Major: %d, Minor: %d, Beta: %d, Patch: %d",
			(int16_t) HW_REV_MAJOR, (int16_t) HW_REV_MINOR,
			(int16_t) HW_REV_BETA, (int16_t) HW_REV_PATCH);
	cnc.cnc_write_length = (strlen((char*) &cnc.cnc_write_data[1]) + 1);
}

void cnc_printf(const char* print_string, ...){
   if(cnc.print_fullness < PRINT_DEPTH){
      va_list print_args;
      char full_string[MAX_PRINT_LENGTH];

      va_start(print_args, print_string);
      vsnprintf(full_string, MAX_PRINT_LENGTH, print_string, print_args);
      va_end(print_args);

      strcpy((char*) cnc.print_buffer[cnc.print_wp], full_string);
      cnc.print_wp = (cnc.print_wp < (PRINT_DEPTH-1)) ? (cnc.print_wp + 1) : 0;
      cnc.print_fullness++;
   }
}

