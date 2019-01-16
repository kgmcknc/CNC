/*
 * cnc_spi.cpp
 *
 *  Created on: Aug 18, 2018
 *      Author: Kyle
 */

#include "cnc_spi.h"
#include "cnc_gpio.h"
#include "cnc_functions.h"
#include "string.h"
#include "stdio.h"
#include "util.h"
#include "main.h"

SPIDRV_HandleData_t handleData;
SPIDRV_Handle_t handle = &handleData;

//uint8_t write_data[16] = {0x0, 0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8, 0x9, 0xA, 0xB, 0xC, 0xD, 0xE, 0xF};
SPIDRV_Init_t initData = SPIDRV_SLAVE_USART3;

extern struct cnc_spi_struct cnc_spi;

static void slave_write_done(SPIDRV_Handle_t handleSlave,
              Ecode_t transferStatus,
              int itemsTransferred)
{
  //(void)handleSlave;
  //(void)transferStatus;
  //(void)itemsTransferred;
	cnc_spi.write_finished = 1;
}

static void slave_read_done(SPIDRV_Handle_t handleSlave,
              Ecode_t transferStatus,
              int itemsTransferred)
{
  //(void)handleSlave;
  //(void)transferStatus;
  //(void)itemsTransferred;
	cnc_spi.read_finished = 1;
}

uint8_t init_spi(cnc_spi_struct* spi_struct){
	spi_struct->read_pending = 0;
	spi_struct->read_ready = 0;
	spi_struct->read_requested = 0;
	spi_struct->write_pending = 0;
	spi_struct->pending_length = 0;
	spi_struct->opcode_sent = 0;
	spi_struct->pending_opcode = idle;
	spi_struct->state = spi_inactive;

	uint8_t spi_init_error = 0;
    initData.portLocationClk = SPI_CLK_LOC;
	initData.portLocationCs = SPI_CS_LOC;
	initData.portLocationRx = SPI_MOSI_LOC;
	initData.portLocationTx = SPI_MISO_LOC;
	initData.dummyTxValue = 0x00;

    // Initialize a SPI driver instance
    if(SPIDRV_Init( handle, &initData ) == ECODE_EMDRV_SPIDRV_OK){
	    // all is good
    	spi_init_error = 0;
    	spi_struct->state = spi_initialized;
    } else {
    	spi_init_error = 1;
    	spi_struct->state = spi_inactive;
    }
    return spi_init_error;
}

void handle_spi(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	check_read_request(spi_struct);
	switch(spi_struct->state){
		case spi_inactive : {
			//init_spi();
			break;
		}
		case spi_initialized : {
			wait_for_connect(spi_struct);
			break;
		}
		case spi_connected : {
			send_connected(spi_struct);
			break;
		}
		case spi_idle : {
			set_idle_read(spi_struct, cnc);
			break;
		}
		case spi_loopback_active : {
			loop_back_data(spi_struct);
			break;
		}
		case spi_handle_request : {
			handle_request(spi_struct, cnc);
			break;
		}
		case get_cnc_instruction : {
			receive_instruction(spi_struct, cnc);
			break;
		}
		case send_cnc_status : {
			send_status(spi_struct, cnc);
			break;
		}
		case firmware_update : {
			clear_write_request(spi_struct);
			while(1); // wait for power down and reboot with fw flash
			break;
		}
		case send_cnc_print : {
			send_print(spi_struct, cnc);
			break;
		}
		default : {
			spi_struct->state = spi_inactive;
		}
	}
}

void wait_for_connect(cnc_spi_struct* spi_struct){
	if(spi_struct->read_pending){
		if(spi_struct->read_finished){
			spi_struct->read_finished = 0;
			// pi sent data
			if(!strncmp(spi_struct->read_data, SPI_MASTER_INIT_STRING, strlen(SPI_MASTER_INIT_STRING))){
				clear_read_request(spi_struct);
				spi_struct->state = spi_connected;
			} else {
				SPIDRV_SReceive(handle, spi_struct->read_data, MASTER_INIT_LENGTH, slave_read_done, 0);
			}
		} else {
			// waiting for pi to initialize data write
		}
	} else {
		if(spi_struct->read_ready && spi_struct->read_requested){
			SPIDRV_SReceive(handle, spi_struct->read_data, MASTER_INIT_LENGTH, slave_read_done, 0);
			set_read_request(spi_struct);
		}
	}
}

void send_connected(cnc_spi_struct* spi_struct){
	if(spi_struct->write_pending){
		if(spi_struct->write_finished){
			// pi read data
			spi_struct->write_finished = 0;
			clear_write_request(spi_struct);
			spi_struct->state = spi_idle;
		} else {
			// waiting for pi to initialize data read
		}
	} else {
		strcpy(spi_struct->write_data, SPI_SLAVE_INIT_STRING);
		SPIDRV_STransmit( handle, spi_struct->write_data, SLAVE_INIT_LENGTH, slave_write_done, 0);
		set_write_request(spi_struct);
	}
}

void set_idle_read(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	spi_opcodes opcode;
	char * length_string = 0;

	if(spi_struct->read_pending){
		if(spi_struct->read_finished){
			// pi sent data
			spi_struct->read_finished = 0;
			opcode = parse_opcode(spi_struct->read_data);
			length_string = &spi_struct->read_data[OPCODE_LENGTH];
			spi_struct->pending_length = string_to_int32(length_string, (IDLE_LENGTH-OPCODE_LENGTH));
			handle_opcode(spi_struct, opcode, cnc);
			clear_read_request(spi_struct);
		} else {

		}
	} else {
		if(spi_struct->read_ready  && spi_struct->read_requested){
			SPIDRV_SReceive( handle, spi_struct->read_data, IDLE_LENGTH, slave_read_done, 0);
			printf("Set idle print\n");
			set_read_request(spi_struct);
		} else {
			if(cnc->request_print){
				spi_struct->state = send_cnc_print;
			} else {
				if(cnc->request_instruction) {
					spi_struct->state = get_cnc_instruction;
				}
			}
		}
	}
}

void loop_back_data(cnc_spi_struct* spi_struct){
	if(spi_struct->read_pending){
		if(spi_struct->read_finished){
			spi_struct->read_finished = 0;
			SPIDRV_STransmit( handle, spi_struct->read_data, spi_struct->pending_length, slave_write_done, 0);
			clear_read_request(spi_struct);
			set_write_request(spi_struct);
		}
	} else {
		if(spi_struct->write_pending){
			if(spi_struct->write_finished){
				spi_struct->write_finished = 0;
				spi_struct->state = spi_idle;
				spi_struct->pending_length = 0;
				clear_write_request(spi_struct);
			}
		} else {
			SPIDRV_SReceive( handle, spi_struct->read_data, spi_struct->pending_length, slave_read_done, 0);
			set_read_request(spi_struct);
		}
	}
}

void receive_instruction(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	if(spi_struct->read_pending){
		if(spi_struct->read_finished){
			spi_struct->read_finished = 0;
			parse_instruction(spi_struct, cnc);
			clear_read_request(spi_struct);
			spi_struct->state = spi_idle;
		} else {

		}
	} else {
		if(spi_struct->write_pending){
			if(spi_struct->write_finished){
				spi_struct->write_finished = 0;
				clear_write_request(spi_struct);
				SPIDRV_SReceive( handle, spi_struct->read_data, INSTRUCTION_LENGTH, slave_read_done, 0);
				set_read_request(spi_struct);
			}
		} else {
			set_write_opcode(spi_struct, new_cnc_instruction, INSTRUCTION_LENGTH+IDLE_LENGTH);
			SPIDRV_STransmit( handle, spi_struct->write_data, IDLE_LENGTH, slave_write_done, 0);
			set_write_request(spi_struct);
		}
	}
}

void parse_instruction(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	char valid_flag = 0;
	char opcode_temp = 0;
	uint16_t next_wp = 0;

	// increment pointer ... it's held at last write until new command comes in to observe last received instruction
	next_wp = (cnc->instruction_wp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->instruction_wp + 1 : 0;

	switch((INSTRUCTION_OPCODE) spi_struct->read_data[0]){
		case PROGRAM_IDLE : {

			break;
		}
		case PROGRAM_START : {

			break;
		}
		case PROGRAM_PAUSE : {

			break;
		}
		case PROGRAM_RESUME : {

			break;
		}
		case PROGRAM_END : {
			sscanf(spi_struct->read_data, "%c", &opcode_temp);
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) opcode_temp;
			valid_flag = 1;
			break;
		}
		case PROGRAM_RESET : {

			break;
		}
		case SYSTEM_CHECK : {

			break;
		}
		case INSTRUCTION : {
			/*sscanf(spi_struct->read_data, "%c%llu%lu%lu%lu%lu%lu%lu%lu%lu",
				&opcode_temp,
				&cnc->instruction_array[next_wp].instruction_number,
				&cnc->instruction_array[next_wp].xl_axis.end_position,
				&cnc->instruction_array[next_wp].xl_axis.step_period,
				&cnc->instruction_array[next_wp].yf_axis.end_position,
				&cnc->instruction_array[next_wp].yf_axis.step_period,
				&cnc->instruction_array[next_wp].zl_axis.end_position,
				&cnc->instruction_array[next_wp].zl_axis.step_period,
				&cnc->instruction_array[next_wp].zr_axis.move_count,
				&cnc->instruction_array[next_wp].zr_axis.step_period);
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) opcode_temp;
			*/
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) spi_struct->read_data[0];
			cnc->instruction_array[next_wp].instruction_number = spi_struct->read_data[1];
			cnc->instruction_array[next_wp].xl_axis.end_position = spi_struct->read_data[9];
			cnc->instruction_array[next_wp].xl_axis.step_period = spi_struct->read_data[16];
			cnc->instruction_array[next_wp].xl_axis.valid_instruction = 1;
			valid_flag = 1;
			break;
		}
		case CHECK_ENDSTOPS : {

			break;
		}
		case ZERO_MOTORS : {
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) spi_struct->read_data[0];
			uint32_t temp_period = spi_struct->read_data[9]; //9-12
			if(spi_struct->read_data[13]){
				cnc->instruction_array[next_wp].xl_axis.find_zero = 1;
				cnc->instruction_array[next_wp].xl_axis.find_max = 0;
				cnc->instruction_array[next_wp].xl_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].xl_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].xl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].xl_axis.find_max = 0;
				cnc->instruction_array[next_wp].xl_axis.valid_instruction = 0;
			}
			if(spi_struct->read_data[14]){
				cnc->instruction_array[next_wp].yf_axis.find_zero = 1;
				cnc->instruction_array[next_wp].yf_axis.find_max = 0;
				cnc->instruction_array[next_wp].yf_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].yf_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].yf_axis.find_zero = 0;
				cnc->instruction_array[next_wp].yf_axis.find_max = 0;
				cnc->instruction_array[next_wp].yf_axis.valid_instruction = 0;
			}
			if(spi_struct->read_data[15]){
				cnc->instruction_array[next_wp].zl_axis.find_zero = 1;
				cnc->instruction_array[next_wp].zl_axis.find_max = 0;
				cnc->instruction_array[next_wp].zl_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].zl_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].zl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zl_axis.find_max = 0;
				cnc->instruction_array[next_wp].zl_axis.valid_instruction = 0;
			}
			if(spi_struct->read_data[16]){
				cnc->instruction_array[next_wp].zr_axis.find_zero = 1;
				cnc->instruction_array[next_wp].zr_axis.find_max = 0;
				cnc->instruction_array[next_wp].zr_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].zr_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].zr_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zr_axis.find_max = 0;
				cnc->instruction_array[next_wp].zr_axis.valid_instruction = 0;
			}
			valid_flag = 1;
			break;
		}
		case MAX_MOTORS : {
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) spi_struct->read_data[0];
			uint32_t temp_period = spi_struct->read_data[9]; //9-12
			if(spi_struct->read_data[13]){
				cnc->instruction_array[next_wp].xl_axis.find_max = 1;
				cnc->instruction_array[next_wp].xl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].xl_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].xl_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].xl_axis.find_max = 0;
				cnc->instruction_array[next_wp].xl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].xl_axis.valid_instruction = 0;
			}
			if(spi_struct->read_data[14]){
				cnc->instruction_array[next_wp].yf_axis.find_max = 1;
				cnc->instruction_array[next_wp].yf_axis.find_zero = 0;
				cnc->instruction_array[next_wp].yf_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].yf_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].yf_axis.find_max = 0;
				cnc->instruction_array[next_wp].yf_axis.find_zero = 0;
				cnc->instruction_array[next_wp].yf_axis.valid_instruction = 0;
			}
			if(spi_struct->read_data[15]){
				cnc->instruction_array[next_wp].zl_axis.find_max = 1;
				cnc->instruction_array[next_wp].zl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zl_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].zl_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].zl_axis.find_max = 0;
				cnc->instruction_array[next_wp].zl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zl_axis.valid_instruction = 0;
			}
			if(spi_struct->read_data[16]){
				cnc->instruction_array[next_wp].zr_axis.find_max = 1;
				cnc->instruction_array[next_wp].zr_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zr_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].zr_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].zr_axis.find_max = 0;
				cnc->instruction_array[next_wp].zr_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zr_axis.valid_instruction = 0;
			}
			valid_flag = 1;
			break;
		}
		case ENABLE_MOTORS : {
			sscanf(spi_struct->read_data, "%c%c%c%c%c%c%c%c",
				&opcode_temp,
				&cnc->instruction_array[next_wp].pending_motor_enables[0],
				&cnc->instruction_array[next_wp].pending_motor_enables[1],
				&cnc->instruction_array[next_wp].pending_motor_enables[2],
				&cnc->instruction_array[next_wp].pending_motor_enables[3],
				&cnc->instruction_array[next_wp].pending_motor_enables[4],
				&cnc->instruction_array[next_wp].pending_motor_enables[5],
				&cnc->instruction_array[next_wp].pending_motor_enables[6]);
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) opcode_temp;
			valid_flag = 1;
			break;
		}
		case DISABLE_MOTORS : {
			sscanf(spi_struct->read_data, "%c%c%c%c%c%c%c%c",
				&opcode_temp,
				&cnc->instruction_array[next_wp].pending_motor_disables[0],
				&cnc->instruction_array[next_wp].pending_motor_disables[1],
				&cnc->instruction_array[next_wp].pending_motor_disables[2],
				&cnc->instruction_array[next_wp].pending_motor_disables[3],
				&cnc->instruction_array[next_wp].pending_motor_disables[4],
				&cnc->instruction_array[next_wp].pending_motor_disables[5],
				&cnc->instruction_array[next_wp].pending_motor_disables[6]);
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) opcode_temp;
			valid_flag = 1;
			break;
		}
		case ENABLE_HEATERS : {

			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) opcode_temp;
			valid_flag = 1;
			break;
		}
		case DISABLE_HEATERS : {
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) opcode_temp;

			valid_flag = 1;
			break;
		}
		case ENABLE_FANS : {

			break;
		}
		case DISABLE_FANS : {

			break;
		}
		case ENABLE_TOOLS : {

			break;
		}
		case DISABLE_TOOLS : {

			break;
		}
		default : {

			break;
		}
	}

	if(valid_flag){
		cnc->instruction_wp = (cnc->instruction_wp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->instruction_wp + 1 : 0;
		cnc->instruction_array[cnc->instruction_wp].instruction_valid = 1;
		cnc->instruction_fullness++;
	}

	strcpy(spi_struct->read_data, "");
}

void parse_status(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	char status_string[STATUS_LENGTH - OPCODE_LENGTH];
	sprintf(status_string, "L:%d%d,R:%d%d,ZL:%d%d,ZR:%d%d,X:%ld,Y:%ld, T0:%f, T1:%f",
			cnc->motors->xl_axis.min_range_flag, cnc->motors->xl_axis.max_range_flag,
			cnc->motors->yf_axis.min_range_flag, cnc->motors->yf_axis.max_range_flag,
			cnc->motors->zl_axis.min_range_flag, cnc->motors->zl_axis.max_range_flag,
			cnc->motors->zr_axis.min_range_flag, cnc->motors->zr_axis.max_range_flag,
			cnc->motors->xl_axis.position,
			cnc->motors->yf_axis.position,
			cnc->heaters->heater_0.current_temp,
			cnc->heaters->heater_1.current_temp);
	set_write_opcode(spi_struct, get_cnc_status, STATUS_LENGTH + IDLE_LENGTH);
	strcpy(spi_struct->write_data + (IDLE_LENGTH), status_string);
}

void parse_print(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	set_write_opcode(spi_struct, new_cnc_print, PRINT_LENGTH+IDLE_LENGTH);
	strcpy(spi_struct->write_data + (IDLE_LENGTH), cnc->print_buffer[cnc->print_rp]);
	cnc->print_buffer[cnc->print_rp][0] = 0;
	cnc->print_rp = (cnc->print_rp < (PRINT_DEPTH-1)) ? (cnc->print_rp + 1) : 0;
	cnc->print_fullness--;
}

void send_status(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	if(spi_struct->read_pending){
		if(spi_struct->read_finished){
			spi_struct->read_finished = 0;
			clear_read_request(spi_struct);
		}
	}
	if(spi_struct->write_pending){
		if(spi_struct->write_finished){
			spi_struct->write_finished = 0;
			clear_write_request(spi_struct);
			spi_struct->state = spi_idle;
		}
	} else {
		parse_status(spi_struct, cnc);
		SPIDRV_STransmit( handle, spi_struct->write_data, STATUS_LENGTH+IDLE_LENGTH, slave_write_done, 0);
		set_write_request(spi_struct);
	}
}

void send_print(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	if(spi_struct->read_pending){
		if(spi_struct->read_finished){
			spi_struct->read_finished = 0;
			clear_read_request(spi_struct);
		}
	}
	if(spi_struct->write_pending){
		if(spi_struct->write_finished){
			spi_struct->write_finished = 0;
			if(spi_struct->opcode_sent){
				spi_struct->opcode_sent = 0;
				parse_print(spi_struct, cnc);
				spi_struct->pending_length = PRINT_LENGTH;
				SPIDRV_STransmit(handle, spi_struct->write_data, spi_struct->pending_length, slave_write_done, 0);
			} else {
				clear_write_request(spi_struct);
				spi_struct->state = spi_idle;
			}
		}
	} else {
		set_write_opcode(spi_struct, new_cnc_print, PRINT_LENGTH+IDLE_LENGTH);
		spi_struct->pending_length = IDLE_LENGTH;
		SPIDRV_STransmit(handle, spi_struct->write_data, spi_struct->pending_length, slave_write_done, 0);
		set_write_request(spi_struct);
		spi_struct->opcode_sent = 1;
	}
}

void handle_opcode(cnc_spi_struct* spi_struct, spi_opcodes new_opcode, cnc_state_struct* cnc){
	switch(new_opcode){
		case loopback_test : {
			spi_struct->state = spi_loopback_active;
			spi_struct->pending_opcode = loopback_test;
			break;
		}
		case flash_firmware : {
			spi_struct->state = firmware_update;
			spi_struct->pending_opcode = flash_firmware;
			break;
		}
		case reconnect_spi : {
			spi_struct->state = spi_initialized;
			spi_struct->pending_opcode = reconnect_spi;
			break;
		}
		case start_cnc_program : {
			spi_struct->state = spi_handle_request;
			spi_struct->pending_opcode = start_cnc_program;
			break;
		}
		case end_cnc_program : {
			spi_struct->state = spi_handle_request;
			spi_struct->pending_opcode = end_cnc_program;
			break;
		}
		case new_cnc_instruction : {
			spi_struct->state = spi_handle_request;
			spi_struct->pending_opcode = new_cnc_instruction;
			break;
		}
		case new_cnc_print : {
			spi_struct->state = spi_handle_request;
			spi_struct->pending_opcode = new_cnc_print;
			break;
		}
		case get_cnc_status : {
			spi_struct->state = send_cnc_status;
			spi_struct->pending_opcode = get_cnc_status;
			break;
		}
		case disable_route : {
			spi_struct->state = spi_handle_request;
			spi_struct->pending_opcode = disable_route;
			break;
		}
		case enable_route : {
			spi_struct->state = spi_handle_request;
			spi_struct->pending_opcode = enable_route;
			break;
		}
		default : {
			spi_struct->state = spi_idle;
			spi_struct->pending_opcode = idle;
		}
	}
}

void handle_request(cnc_spi_struct* spi_struct, cnc_state_struct* cnc){
	switch(spi_struct->pending_opcode){
		case start_cnc_program : {
			cnc->start_program = 1;
			spi_struct->state = spi_idle;
			break;
		}
		case end_cnc_program : {
			cnc->abort_program = 1;
			spi_struct->state = spi_idle;
			break;
		}
		case disable_route : {
			disable_route_pins();
			cnc_printf(cnc,"Disabled Route Pins");
			spi_struct->state = spi_idle;
			break;
		}
		case enable_route : {
			enable_route_pins();
			cnc_printf(cnc,"Enabled Route Pins");
			spi_struct->state = spi_idle;
			break;
		}
		default : {

		}
	}
}

spi_opcodes parse_opcode(char opcode_string[MAX_STRING_LENGTH]){
	spi_opcodes new_opcode;
	new_opcode = (spi_opcodes) string_to_int32(opcode_string, OPCODE_LENGTH);
	return new_opcode;
}

void check_read_request(cnc_spi_struct* spi_struct){
	if(GPIO_PinInGet(SPI_M_READY_PORT, SPI_M_READY_PIN)){
		spi_struct->read_requested = 1;
		if(spi_struct->read_pending) spi_struct->read_ready = 0;
	} else {
		spi_struct->read_requested = 0;
		spi_struct->read_ready = 1;
	}
}

void set_read_request(cnc_spi_struct* spi_struct){
	spi_struct->read_pending = 1;
}

void clear_read_request(cnc_spi_struct* spi_struct){
	spi_struct->read_pending = 0;
}

void set_write_request(cnc_spi_struct* spi_struct){
	GPIO_PinOutSet(SPI_S_READY_PORT, SPI_S_READY_PIN);
	spi_struct->write_pending = 1;
}
void clear_write_request(cnc_spi_struct* spi_struct){
	GPIO_PinOutClear(SPI_S_READY_PORT, SPI_S_READY_PIN);
	spi_struct->write_pending = 0;
}

void set_write_opcode(cnc_spi_struct* spi_struct, spi_opcodes instruction, uint8_t byte_length){
	uint8_t count = 0;
	for(count=0;count<OPCODE_LENGTH;count++){
		spi_struct->write_data[count] = ((instruction >> (count*8)) & 0xFF);
	}
	for(count=0;count<byte_length;count++){
		spi_struct->write_data[count+OPCODE_LENGTH] = ((byte_length >> (count*8)) & 0xFF);
	}
}

