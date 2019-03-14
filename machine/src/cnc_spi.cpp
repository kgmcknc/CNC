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

SPIDRV_Init_t initData = SPIDRV_SLAVE_USART3;

char spi_read_data[MAX_SPI_LENGTH];
char spi_write_data[MAX_SPI_LENGTH];

static void slave_transfer_done(SPIDRV_Handle_t handleSlave,
              Ecode_t transferStatus,
              int itemsTransferred)
{
  //(void)handleSlave;
  //(void)transferStatus;
  //(void)itemsTransferred;
  //cnc_spi.transfer_finished = 1;
	spi_finish_transfer(spi_read_data, (uint16_t) itemsTransferred);
}

uint8_t spi_get_master_request_pin(void){
	return GPIO_PinInGet(SPI_RESET_PORT, SPI_RESET_PIN);
}
void spi_set_slave_request_pin(void){
	GPIO_PinOutSet(SPI_REQUEST_PORT, SPI_REQUEST_PIN);
}
void spi_clear_slave_request_pin(void){
	GPIO_PinOutClear(SPI_REQUEST_PORT, SPI_REQUEST_PIN);
}
uint8_t spi_transfer_data(char* spi_data, uint16_t data_length){
	int value = 0;
	if(data_length < 8){
		value = 1;
	} else {
		if(data_length > 250){
			value = 1;
		}
	}
	memcpy(spi_write_data, spi_data, data_length);
	value = SPIDRV_STransfer(handle, spi_write_data, spi_read_data, data_length, slave_transfer_done, 0);
	if(value == 0){
		value = 1;
	} else {
		value = 0;
	}
	return 0;
}

void init_spi_driver(void){
	initData.portLocationClk = SPI_CLK_LOC;
	initData.portLocationCs = SPI_CS_LOC;
	initData.portLocationRx = SPI_MOSI_LOC;
	initData.portLocationTx = SPI_MISO_LOC;
	initData.dummyTxValue = 'X';
	//initData.slaveStartMode = spidrvSlaveStartDelayed;
	SPIDRV_Init( handle, &initData );
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
/*
void parse_instruction(spi_struct* spi, cnc_state_struct* cnc){
	char valid_flag = 0;
	char opcode_temp = 0;
	uint16_t next_wp = 0;

	// increment pointer ... it's held at last write until new command comes in to observe last received instruction
	next_wp = (cnc->instruction_wp<(INSTRUCTION_FIFO_DEPTH-1)) ? cnc->instruction_wp + 1 : 0;

	switch((INSTRUCTION_OPCODE) spi->spi_data[0]){
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
			sscanf(spi->spi_data, "%c", &opcode_temp);
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
			sscanf(spi->read_data, "%c%llu%lu%lu%lu%lu%lu%lu%lu%lu",
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

			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) spi->spi_data[0];
			cnc->instruction_array[next_wp].instruction_number = spi->spi_data[1];
			cnc->instruction_array[next_wp].xl_axis.end_position = spi->spi_data[9];
			cnc->instruction_array[next_wp].xl_axis.step_period = spi->spi_data[16];
			cnc->instruction_array[next_wp].xl_axis.valid_instruction = 1;
			valid_flag = 1;
			break;
		}
		case CHECK_ENDSTOPS : {

			break;
		}
		case ZERO_MOTORS : {
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) spi->spi_data[0];
			uint32_t temp_period = spi->spi_data[9]; //9-12
			if(spi->spi_data[13]){
				cnc->instruction_array[next_wp].xl_axis.find_zero = 1;
				cnc->instruction_array[next_wp].xl_axis.find_max = 0;
				cnc->instruction_array[next_wp].xl_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].xl_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].xl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].xl_axis.find_max = 0;
				cnc->instruction_array[next_wp].xl_axis.valid_instruction = 0;
			}
			if(spi->spi_data[14]){
				cnc->instruction_array[next_wp].yf_axis.find_zero = 1;
				cnc->instruction_array[next_wp].yf_axis.find_max = 0;
				cnc->instruction_array[next_wp].yf_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].yf_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].yf_axis.find_zero = 0;
				cnc->instruction_array[next_wp].yf_axis.find_max = 0;
				cnc->instruction_array[next_wp].yf_axis.valid_instruction = 0;
			}
			if(spi->spi_data[15]){
				cnc->instruction_array[next_wp].zl_axis.find_zero = 1;
				cnc->instruction_array[next_wp].zl_axis.find_max = 0;
				cnc->instruction_array[next_wp].zl_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].zl_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].zl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zl_axis.find_max = 0;
				cnc->instruction_array[next_wp].zl_axis.valid_instruction = 0;
			}
			if(spi->spi_data[16]){
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
			cnc->instruction_array[next_wp].opcode_flags = (INSTRUCTION_OPCODE) spi->spi_data[0];
			uint32_t temp_period = spi->spi_data[9]; //9-12
			if(spi->spi_data[13]){
				cnc->instruction_array[next_wp].xl_axis.find_max = 1;
				cnc->instruction_array[next_wp].xl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].xl_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].xl_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].xl_axis.find_max = 0;
				cnc->instruction_array[next_wp].xl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].xl_axis.valid_instruction = 0;
			}
			if(spi->spi_data[14]){
				cnc->instruction_array[next_wp].yf_axis.find_max = 1;
				cnc->instruction_array[next_wp].yf_axis.find_zero = 0;
				cnc->instruction_array[next_wp].yf_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].yf_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].yf_axis.find_max = 0;
				cnc->instruction_array[next_wp].yf_axis.find_zero = 0;
				cnc->instruction_array[next_wp].yf_axis.valid_instruction = 0;
			}
			if(spi->spi_data[15]){
				cnc->instruction_array[next_wp].zl_axis.find_max = 1;
				cnc->instruction_array[next_wp].zl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zl_axis.step_period = temp_period;
				cnc->instruction_array[next_wp].zl_axis.valid_instruction = 1;
			} else {
				cnc->instruction_array[next_wp].zl_axis.find_max = 0;
				cnc->instruction_array[next_wp].zl_axis.find_zero = 0;
				cnc->instruction_array[next_wp].zl_axis.valid_instruction = 0;
			}
			if(spi->spi_data[16]){
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
			sscanf(spi->spi_data, "%c%c%c%c%c%c%c%c",
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
			sscanf(spi->spi_data, "%c%c%c%c%c%c%c%c",
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

	strcpy(spi->spi_data, "");
}

spi_opcodes parse_opcode(char opcode_string[MAX_STRING_LENGTH]){
	spi_opcodes new_opcode;
	new_opcode = (spi_opcodes) string_to_int32(opcode_string, OPCODE_LENGTH);
	return new_opcode;
}

void check_read_request(spi_struct* spi){
	if(GPIO_PinInGet(SPI_M_READY_PORT, SPI_M_READY_PIN)){
		spi->read_requested = 1;
		if(spi->read_pending) spi_struct->read_ready = 0;
	} else {
		spi->read_requested = 0;
		spi->read_ready = 1;
	}
}
*/
void set_pending_transfer(spi_struct* spi){
	spi->transfer_pending = 1;
}

void clear_pending_transfer(spi_struct* spi){
	spi->transfer_pending = 0;
}

void set_request_ready(spi_struct* spi){
	GPIO_PinOutSet(SPI_REQUEST_PORT, SPI_REQUEST_PIN);
	spi->transfer_request = 1;
}
void clear_request_ready(spi_struct* spi){
	GPIO_PinOutClear(SPI_REQUEST_PORT, SPI_REQUEST_PIN);
	spi->transfer_request = 0;
}
/*
void set_write_opcode(spi_struct* spi, spi_opcodes instruction, uint8_t byte_length){
	uint8_t count = 0;
	for(count=0;count<OPCODE_LENGTH;count++){
		spi->spi_data[count] = ((instruction >> (count*8)) & 0xFF);
	}
	for(count=0;count<byte_length;count++){
		spi->spi_data[count+OPCODE_LENGTH] = ((byte_length >> (count*8)) & 0xFF);
	}
}*/

