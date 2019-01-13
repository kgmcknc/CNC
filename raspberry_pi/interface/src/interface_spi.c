#include "main.h"
#include "interface_spi.h"

void check_spi(void){
	// wiringPiSPISetupMode exits without return if spi not enabled...
	// this checks and closes (or exits out before forking in main)
	// so that the c program doesn't start a child and then crash...
	int temp_fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_SPEED, 0);
	close(temp_fd);
}

uint8_t init_spi(struct cnc_spi_struct* spi_struct){
	spi_struct->connected = 0;
	spi_struct->read_pending = 0;
	spi_struct->write_pending = 0;
	spi_struct->pending_length = 0;
	spi_struct->updating_firmware = 0;
	spi_struct->pending_opcode = idle;
	strcpy(spi_struct->read_data, "");

	spi_fd = wiringPiSPISetupMode(SPI_CHANNEL, SPI_SPEED, 0);
	
	if(spi_fd < 0){
		printf("Spi error...\n");
		return 1;
	} else {
		spi_struct->state = spi_initialized;
		return 0;
	}
}

void handle_spi(struct cnc_spi_struct* spi_struct){
	switch(spi_struct->state){
		case spi_inactive : {
			spi_struct->connected = 0;
			// re initialize?
			printf("Spi was inactive??\n");
			break;
		}
		case spi_initialized : {
			send_connected(spi_struct);
			break;
		}
		case spi_connected : {
			wait_for_connect(spi_struct);
			break;
		}
		case spi_idle : {
			check_idle(spi_struct);
			break;
		}
		case spi_read_print : {
			display_cnc_print(spi_struct);
			break;
		}
		case spi_get_status : {
			request_cnc_status(spi_struct);
			break;
		}
		case spi_send_instruction : {
			send_cnc_instruction(spi_struct);
			break;
		}
		case spi_read_version : {
			
			break;
		}
		case spi_send_opcode : {
			send_cnc_opcode(spi_struct);
			break;
		}
		case spi_request_reconnect : {
			request_reconnect(spi_struct);
			break;
		}
		case firmware_update : {
			update_si_firmware(spi_struct);
			break;
		}
		default : {
			spi_struct->state = spi_inactive;
		}
	}
}

void send_connected(struct cnc_spi_struct* spi_struct){
	if(get_s_ready_state(spi_struct)){
		spi_struct->state = spi_connected;
		spi_struct->pending_opcode = idle;
	} else {
		set_m_ready(spi_struct);
		strcpy(spi_struct->write_data, SPI_MASTER_INIT_STRING);
		wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->write_data, MASTER_INIT_LENGTH);
		clear_m_ready(spi_struct);
	}
}

void wait_for_connect(struct cnc_spi_struct* spi_struct){
	if(get_s_ready_state(spi_struct)){
		wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->read_data, SLAVE_INIT_LENGTH);
		spi_struct->state = spi_idle;
		spi_struct->pending_opcode = idle;
		spi_struct->connected = 1;
		printf("Read: %s... Connected to Mirco\n", spi_struct->read_data);
	}
}

void check_idle(struct cnc_spi_struct* spi_struct){
	if(get_s_ready_state(spi_struct)){
		enum spi_opcodes opcode;
		char * length_string = 0;
		delay(100);
		wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->read_data, IDLE_LENGTH);
		opcode = parse_opcode(spi_struct->read_data);
		length_string = &spi_struct->read_data[OPCODE_LENGTH];
		spi_struct->pending_length = string_to_int32(length_string, (IDLE_LENGTH-OPCODE_LENGTH));
		handle_opcode(spi_struct, opcode);
	} else {
		handle_request(spi_struct);
	}
}

void handle_request(struct cnc_spi_struct* spi_struct){
	switch(spi_struct->pending_opcode){
		case reconnect_spi : {
			spi_struct->state = spi_request_reconnect;
			break;
		}
		case get_cnc_status : {
			spi_struct->state = spi_get_status;
			break;
		}
		case flash_firmware : {
			spi_struct->state = firmware_update;
			break;
		}
		case read_version : {
			spi_struct->state = spi_read_version;
			break;
		}
		case new_cnc_instruction : {
			spi_struct->state = spi_idle;
			break;
		}
		case enable_route : {
			spi_struct->state = spi_send_opcode;
			break;
		}
		case disable_route : {
			spi_struct->state = spi_send_opcode;
			break;
		}
		case start_cnc_program : {
			spi_struct->state = spi_send_opcode;
			break;
		}
		case end_cnc_program : {
			spi_struct->state = spi_send_opcode;
			break;
		}
		default : {
			spi_struct->state = spi_idle;
		}
	}
}

void set_m_ready(struct cnc_spi_struct* spi_struct){
	digitalWrite(SPI_RX_READY, HIGH);
}
void clear_m_ready(struct cnc_spi_struct* spi_struct){
	digitalWrite(SPI_RX_READY, LOW);
}

uint8_t get_s_ready_state(struct cnc_spi_struct* spi_struct){
	return digitalRead(SPI_TX_READY);
}

enum spi_opcodes parse_opcode(char opcode_string[MAX_SPI_LENGTH]){
	enum spi_opcodes new_opcode;
	new_opcode = (enum spi_opcodes) string_to_int32(opcode_string, OPCODE_LENGTH);
	return new_opcode;
}

int32_t string_to_int32(char length_string[MAX_SPI_LENGTH], uint8_t byte_length){
	int32_t temp = 0;
	uint8_t length_count = 0;
	
	// parse string into int
	for(length_count=0;length_count<byte_length;length_count++){
		temp = (temp << 8) | length_string[byte_length - 1 - length_count];
	}
	
	// check if negative and pad
	if(length_string[byte_length - 1 - length_count] >> 7){
		// top bit was set... so fill negative
		temp = (-1 << (byte_length*8)) | temp;
	}
	
	return temp;
}

void handle_opcode(struct cnc_spi_struct* spi_struct, enum spi_opcodes new_opcode){
	switch(new_opcode){
		case reconnect_spi : {
			spi_struct->state = spi_request_reconnect;
			break;
		}
		case new_cnc_print : {
			spi_struct->state = spi_read_print;
			break;
		}
		case new_cnc_instruction : {
			spi_struct->state = spi_send_instruction;
			break;
		}
		default : {
			spi_struct->state = spi_idle;
		}
	}
}

void display_cnc_print(struct cnc_spi_struct* spi_struct){
	char read_string[PRINT_LENGTH];
	wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->read_data, spi_struct->pending_length);
	strncpy(read_string, spi_struct->read_data + IDLE_LENGTH, PRINT_LENGTH-IDLE_LENGTH);
	printf("CNC Print: %s\n", read_string);
	strcpy(spi_struct->read_data, "");
	spi_struct->state = spi_idle;
	spi_struct->pending_opcode = idle;
}

char instruction_count = 0;

void send_cnc_instruction(struct cnc_spi_struct* spi_struct){
	char clear_count = 0;
	if(get_s_ready_state(spi_struct)){
		// writing... so don't send?
		// should never happen
		printf("Can't send instruction because writing\n");
	} else {
		set_m_ready(spi_struct);
		for(clear_count=0;clear_count<MAX_SPI_LENGTH;clear_count++) spi_struct->write_data[clear_count] = 0;
		if(instruction_count == 0){
			// turn on motor
			spi_struct->write_data[0] = 11;
			spi_struct->write_data[1] = 1;
			spi_struct->write_data[2] = 1;
		}
		if(instruction_count == 1){
			// move left
			spi_struct->write_data[0] = 9;
			
			spi_struct->write_data[1] = 0;
			spi_struct->write_data[2] = 0;
			spi_struct->write_data[3] = 0;
			spi_struct->write_data[4] = 0;
			spi_struct->write_data[5] = 0;
			spi_struct->write_data[6] = 0;
			spi_struct->write_data[7] = 0;
			spi_struct->write_data[8] = 0;
			
			spi_struct->write_data[9] = 10;
			spi_struct->write_data[10] = 0;
			spi_struct->write_data[11] = 0;
			spi_struct->write_data[12] = 0;
			
			spi_struct->write_data[13] = 1;
			spi_struct->write_data[14] = 1;
			spi_struct->write_data[15] = 0;
			spi_struct->write_data[16] = 0;
		}
		if(instruction_count == 2){
			// move left
			spi_struct->write_data[0] = 10;
			
			spi_struct->write_data[1] = 0;
			spi_struct->write_data[2] = 0;
			spi_struct->write_data[3] = 0;
			spi_struct->write_data[4] = 0;
			spi_struct->write_data[5] = 0;
			spi_struct->write_data[6] = 0;
			spi_struct->write_data[7] = 0;
			spi_struct->write_data[8] = 0;
			
			spi_struct->write_data[9] = 10;
			spi_struct->write_data[10] = 0;
			spi_struct->write_data[11] = 0;
			spi_struct->write_data[12] = 0;
			
			spi_struct->write_data[13] = 1;
			spi_struct->write_data[14] = 1;
			spi_struct->write_data[15] = 0;
			spi_struct->write_data[16] = 0;
		}
		if(instruction_count == 3){
			//turn off motor
			spi_struct->write_data[0] = 12;
			spi_struct->write_data[1] = 1;
			spi_struct->write_data[2] = 1;
		}
		if(instruction_count == 4){
			//end
			spi_struct->write_data[0] = 4;
			spi_struct->write_data[1] = 1;
		}
		printf("sent instruction! %d\n", instruction_count);
		
		delay(100);
		wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->write_data, spi_struct->pending_length);
		instruction_count = (instruction_count < 4) ? instruction_count + 1: instruction_count;
		clear_m_ready(spi_struct);
		spi_struct->pending_length = 0;
		spi_struct->state = spi_idle;
		spi_struct->pending_opcode = idle;
	}
}

void send_cnc_opcode(struct cnc_spi_struct* spi_struct){
	instruction_count = 0;
	if(get_s_ready_state(spi_struct)){
		// writing... so don't send?
		printf("Can't send because writing\n");
	} else {
		set_m_ready(spi_struct);
		printf("opcode Data: %d\n", spi_struct->pending_opcode);
		set_write_opcode(spi_struct, spi_struct->pending_opcode, IDLE_LENGTH);
		delay(100);
		spi_struct->pending_length = IDLE_LENGTH;
		clear_m_ready(spi_struct);
		wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->write_data, IDLE_LENGTH);
		printf("Sent Data: %s, %d\n", spi_struct->write_data, spi_struct->pending_opcode);
		delay(100);
		spi_struct->state = spi_idle;
		spi_struct->pending_opcode = idle;
	}
}

void request_reconnect(struct cnc_spi_struct* spi_struct){
	set_m_ready(spi_struct);
	set_write_opcode(spi_struct, reconnect_spi, IDLE_LENGTH);
	delay(100);
	spi_struct->pending_length = IDLE_LENGTH;
	wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->write_data, IDLE_LENGTH);
	delay(100);
	clear_m_ready(spi_struct);
	spi_struct->state = spi_initialized;
	spi_struct->connected = 1;
	spi_struct->pending_opcode = idle;
}

void request_cnc_status(struct cnc_spi_struct* spi_struct){
	if(get_s_ready_state(spi_struct)){
		char read_string[STATUS_LENGTH];
		wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->read_data, spi_struct->pending_length);
		strncpy(read_string, spi_struct->read_data + IDLE_LENGTH, STATUS_LENGTH-IDLE_LENGTH);
		printf("Status Print: %s\n", read_string);
		spi_struct->write_pending = 0;
		spi_struct->state = spi_idle;
		spi_struct->pending_opcode = idle;
	} else {
		if(!spi_struct->write_pending){
			printf("Requesting CNC Status...\n");
			set_m_ready(spi_struct);
			set_write_opcode(spi_struct, get_cnc_status, IDLE_LENGTH);
			delay(100);
			spi_struct->pending_length = STATUS_LENGTH+IDLE_LENGTH;
			wiringPiSPIDataRW(SPI_CHANNEL, spi_struct->write_data, IDLE_LENGTH);
			delay(100);
			clear_m_ready(spi_struct);
			spi_struct->write_pending = 1;
			printf("Sent Request...\n");
		}
	}
}

void set_write_opcode(struct cnc_spi_struct* spi_struct, enum spi_opcodes instruction, uint8_t byte_length){
	uint8_t count = 0;
	for(count=0;count<OPCODE_LENGTH;count++){
		spi_struct->write_data[count] = ((instruction >> (count*8)) & 0xFF);
	}
	for(count=0;count<byte_length;count++){
		spi_struct->write_data[count+OPCODE_LENGTH] = ((byte_length >> (count*8)) & 0xFF);
	}
}
