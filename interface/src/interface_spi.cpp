#include "main.h"
#include "interface_spi.h"

void check_spi(void){
	// wiringPiSPISetupMode exits without return if spi not enabled...
	// this checks and closes (or exits out before forking in main)
	// so that the c program doesn't start a child and then crash...
	int temp_fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_SPEED, 0);
	close(temp_fd);
}

void init_spi_driver(void){
	spi_fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_SPEED, 0);
	
	if(spi_fd < 0){
		printf("Spi error...\n");
	} else {
		printf("Spi Driver Initialized...\n");
	}
}

uint8_t spi_get_slave_request_pin(void){
	return digitalRead(CNC_SPI_REQUEST);
}
void spi_set_master_request_pin(void){
	digitalWrite(CNC_SPI_RESET, HIGH);
}
void spi_clear_master_request_pin(void){
	digitalWrite(CNC_SPI_RESET, LOW);
}
uint8_t spi_transfer_data(char* spi_data, uint16_t data_length){
	int value = 0;
	if(data_length == 0){
		printf("Zero Length!!!");
	} else {
		if(data_length > 250){
			printf("Too High Length!!!");
		}
	}
	value = wiringPiSPIDataRW(SPI_CHANNEL, (unsigned char*) spi_data, data_length);
	if(value == 0){
		printf("ZERO BAD transfer\n");
	} else {
		if(value > 0){
			
		} else {
			printf("BAD transfer\n");
		}
	}
	spi_finish_transfer(spi_data, data_length);
	return 0;
}
/*
void handle_interface_spi(struct spi_struct* spi){
	switch(spi->state){
		case spi_reset : {
			printf("Spi being reset or bad state??\n");
			set_spi_reset();
			spi->connected = 0;
			handle_spi_reset(spi);
			break;
		}
		case spi_initialized : {
			send_connected(spi);
			break;
		}
		case spi_idle : {
			check_idle(spi);
			break;
		}
		case spi_handle_request : {

			break;
		}
		case spi_read_print : {
			display_cnc_print(spi);
			break;
		}
		case spi_get_status : {
			request_cnc_status(spi);
			break;
		}
		case spi_send_instruction : {
			send_cnc_instruction(spi);
			break;
		}
		case spi_read_version : {
			
			break;
		}
		case spi_send_opcode : {
			send_cnc_opcode(spi);
			break;
		}
		case spi_request_reconnect : {
			request_reconnect(spi);
			break;
		}
		case firmware_update : {
			update_si_firmware(spi);
			break;
		}
		default : {
			spi->state = spi_reset;
		}
	}
}

void handle_spi_reset(struct spi_struct* spi){
	uint32_t receive_length;
	
	strcpy(spi->spi_data, SPI_IDLE_STRING);
	receive_length = wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, strlen(SPI_IDLE_STRING));
	// Adding nul character at end of string since we don't send it
	spi->spi_data[receive_length] = '\0';
	if(!strcmp(spi->spi_data, SPI_RESET_STRING)){
		printf("Received Reset String %s, so CNC Spi is reset\n", spi->spi_data);
		clear_spi_reset();
		usleep(50000);
		wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, strlen(SPI_RESET_STRING));
		spi->state = spi_initialized;
	} else {
		printf("CNC Spi isn't reset... Received: %s\n", spi->spi_data);
	}
}

void send_connected(struct spi_struct* spi){
	uint32_t receive_length;
	strcpy(spi->spi_data, SPI_MASTER_INIT_STRING);
	usleep(50000);
	receive_length = wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, strlen(SPI_MASTER_INIT_STRING));
	// Adding nul character at end of string since we don't send it
	spi->spi_data[receive_length] = '\0';
	if(!strcmp(spi->spi_data, SPI_SLAVE_INIT_STRING)){
		printf("Received %s, so CNC is connected\n", spi->spi_data);
		spi->state = spi_idle;
		spi->connected = 1;
		spi->pending_opcode = idle;
	} else {
		printf("CNC Spi Wasn't Ready... Received: %s, length: %d\n", spi->spi_data, receive_length);
	}
}

void check_idle(struct spi_struct* spi){
	if(spi->state == spi_idle){
		// only check cnc spi request when interface is idle
		if(check_spi_request(spi)){
			// cnc is requesting read or write data...
			enum spi_opcodes opcode;
			char * length_string = 0;
			wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, IDLE_LENGTH);
			opcode = parse_opcode(spi->spi_data);
			length_string = &spi->spi_data[OPCODE_LENGTH];
			spi->transfer_length = string_to_int32(length_string, (IDLE_LENGTH-OPCODE_LENGTH));
			handle_opcode(spi, opcode);
		} else {
			handle_request(spi);
		}
	} else {
		handle_request(spi);
	}
}

void handle_request(struct spi_struct* spi){
	switch(spi->pending_opcode){
		case reconnect_spi : {
			spi->state = spi_request_reconnect;
			break;
		}
		case get_cnc_status : {
			spi->state = spi_get_status;
			break;
		}
		case flash_firmware : {
			spi->state = firmware_update;
			break;
		}
		case read_version : {
			spi->state = spi_read_version;
			break;
		}
		case new_cnc_instruction : {
			spi->state = spi_idle;
			break;
		}
		case enable_route : {
			spi->state = spi_send_opcode;
			break;
		}
		case disable_route : {
			spi->state = spi_send_opcode;
			break;
		}
		case start_cnc_program : {
			spi->state = spi_send_opcode;
			break;
		}
		case end_cnc_program : {
			spi->state = spi_send_opcode;
			break;
		}
		default : {
			spi->state = spi_idle;
		}
	}
}
*/

void display_cnc_print(struct spi_struct* spi){
	/*char read_string[PRINT_LENGTH];
	if(spi->transfer_length > PRINT_LENGTH) spi->transfer_length = PRINT_LENGTH;
	//wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, spi->transfer_length+IDLE_LENGTH);
	strncpy(read_string, spi->spi_data + IDLE_LENGTH, spi->transfer_length);
	read_string[spi->transfer_length] = '\0';
	printf("CNC Print: %s\n", read_string);
	spi->state = spi_idle;
	spi->pending_opcode = idle;*/
}

char instruction_count = 0;

void send_cnc_instruction(struct spi_struct* spi){
	/*char clear_count = 0;
	if(get_s_ready_state(spi)){
		// writing... so don't send?
		// should never happen
		printf("Can't send instruction because writing\n");
	} else {
		set_m_ready(spi);
		for(clear_count=0;clear_count<MAX_SPI_LENGTH;clear_count++) spi->spi_data[clear_count] = 0;
		if(instruction_count == 0){
			// turn on motor
			spi->spi_data[0] = 11;
			spi->spi_data[1] = 1;
			spi->spi_data[2] = 1;
		}
		if(instruction_count == 1){
			// move left
			spi->spi_data[0] = 9;
			
			spi->spi_data[1] = 0;
			spi->spi_data[2] = 0;
			spi->spi_data[3] = 0;
			spi->spi_data[4] = 0;
			spi->spi_data[5] = 0;
			spi->spi_data[6] = 0;
			spi->spi_data[7] = 0;
			spi->spi_data[8] = 0;
			
			spi->spi_data[9] = 10;
			spi->spi_data[10] = 0;
			spi->spi_data[11] = 0;
			spi->spi_data[12] = 0;
			
			spi->spi_data[13] = 1;
			spi->spi_data[14] = 1;
			spi->spi_data[15] = 0;
			spi->spi_data[16] = 0;
		}
		if(instruction_count == 2){
			// move left
			spi->spi_data[0] = 10;
			
			spi->spi_data[1] = 0;
			spi->spi_data[2] = 0;
			spi->spi_data[3] = 0;
			spi->spi_data[4] = 0;
			spi->spi_data[5] = 0;
			spi->spi_data[6] = 0;
			spi->spi_data[7] = 0;
			spi->spi_data[8] = 0;
			
			spi->spi_data[9] = 10;
			spi->spi_data[10] = 0;
			spi->spi_data[11] = 0;
			spi->spi_data[12] = 0;
			
			spi->spi_data[13] = 1;
			spi->spi_data[14] = 1;
			spi->spi_data[15] = 0;
			spi->spi_data[16] = 0;
		}
		if(instruction_count == 3){
			//turn off motor
			spi->spi_data[0] = 12;
			spi->spi_data[1] = 1;
			spi->spi_data[2] = 1;
		}
		if(instruction_count == 4){
			//end
			spi->spi_data[0] = 4;
			spi->spi_data[1] = 1;
		}
		printf("sent instruction! %d\n", instruction_count);
		
		delay(100);
		wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, spi->transfer_length+IDLE_LENGTH);
		instruction_count = (instruction_count < 4) ? instruction_count + 1: instruction_count;
		clear_m_ready(spi);
		spi->transfer_length = 0;
		spi->state = spi_idle;
		spi->pending_opcode = idle;
	}*/
}

void send_cnc_opcode(struct spi_struct* spi){
	/*instruction_count = 0;
	if(get_s_ready_state(spi)){
		// writing... so don't send?
		printf("Can't send because writing\n");
	} else {
		set_m_ready(spi);
		printf("opcode Data: %d\n", spi->pending_opcode);
		set_write_opcode(spi, spi->pending_opcode, IDLE_LENGTH);
		delay(100);
		spi->transfer_length = IDLE_LENGTH;
		clear_m_ready(spi);
		wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, IDLE_LENGTH);
		printf("Sent Data: %s, %d\n", spi->spi_data, spi->pending_opcode);
		delay(100);
		spi->state = spi_idle;
		spi->pending_opcode = idle;
	}*/
}

void request_reconnect(struct spi_struct* spi){
	//set_m_ready(spi);
	//set_write_opcode(spi, reconnect_spi, IDLE_LENGTH);
	delay(100);
	spi->transfer_length = IDLE_LENGTH;
	//wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, IDLE_LENGTH);
	delay(100);
	//clear_m_ready(spi);
	spi->state = spi_initialized;
	spi->connected = 1;
	//spi->pending_opcode = idle;
}

void request_cnc_status(struct spi_struct* spi){
	/*//if(get_s_ready_state(spi)){
		char read_string[STATUS_LENGTH];
		//wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, spi->transfer_length+IDLE_LENGTH);
		strncpy(read_string, spi->spi_data + IDLE_LENGTH, STATUS_LENGTH);
		read_string[spi->transfer_length] = '\0';
		printf("Status Print: %s\n", read_string);
		spi->transfer_pending = 0;
		spi->state = spi_idle;
		spi->pending_opcode = idle;
	//} else {
		if(!spi->transfer_pending){
			printf("Requesting CNC Status...\n");
			//set_m_ready(spi);
			//set_write_opcode(spi, get_cnc_status, IDLE_LENGTH);
			usleep(50000);
			spi->transfer_length = STATUS_LENGTH;
			//wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, IDLE_LENGTH);
			usleep(50000);
			//clear_m_ready(spi);
			spi->transfer_pending = 1;
			printf("Sent Request...\n");
		}
	//}*/
}

void set_write_opcode(struct spi_struct* spi, enum spi_opcodes instruction, uint8_t byte_length){
	uint8_t count = 0;
	for(count=0;count<OPCODE_LENGTH;count++){
		spi->spi_data[count] = ((instruction >> (count*8)) & 0xFF);
	}
	for(count=0;count<byte_length;count++){
		spi->spi_data[count+OPCODE_LENGTH] = ((byte_length >> (count*8)) & 0xFF);
	}
}
