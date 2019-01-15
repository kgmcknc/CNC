#include "main.h"
#include "stdio.h"
#include "interface_spi.h"
#include "gcode.h"

void handle_input(struct cnc_state* cnc, uint8_t system_command[MAX_FUNCTION_STRING]){
	switch(system_command[0]){
		case 'q' : {
			cnc->state = DISABLE_GPIO;
			break;
		}
		case 'c' : {
			cnc->spi->pending_opcode = reconnect_spi;
			cnc->state = PROCESS_INPUT;
			break;
		}
		case 'g' : {
			cnc->spi->pending_opcode = get_cnc_status;
			cnc->state = PROCESS_INPUT;
			break;
		}
		case 'v' : {
			cnc->spi->pending_opcode = read_version;
			cnc->state = PROCESS_INPUT;
			break;
		}
		case 'e' : {
			cnc->spi->pending_opcode = disable_route;
			cnc->state = POWER_MOTORS_ON;
			break;
		}
		case 'd' : {
			cnc->spi->pending_opcode = enable_route;
			cnc->state = POWER_MOTORS_OFF;
			break;
		}
		case 'o' : {
			cnc->state = GET_PROGRAM;
			break;
		}
		case 'u' : {
			cnc->spi->pending_opcode = flash_firmware;
			cnc->state = UPDATE_FIRMARE;
			break;
		}
		case 'i' : {
			
			cnc->state = SEND_TO_MICRO;
			break;
		}
		/*case 's' : {
			send_spi_string(system_command, MAX_SPI_LENGTH);
			break;
		}
		case 'i' : {
			cnc->spi->pending_opcode = start_cnc_program;
			printf("got start interface function: %d\n", cnc->spi->pending_opcode);
			break;
		}
		case 't' : {
			printf("got end interface function\n");
			cnc->spi->pending_opcode = end_cnc_program;
			break;
		}*/
		default : {
			//spi_struct->pending_opcode = idle;
			cnc->state = IDLE;
			break;
		}
	}
}
/*
void interface_functions(uint8_t command_ready, uint8_t system_command[MAX_FUNCTION_STRING], struct cnc_spi_struct* spi_struct, struct cnc_state* cnc){
	if(command_ready){
		switch(system_command[0]){
			case 'q' :
				quit_system = 1;
				break;
			case 'e' :
				spi_struct->pending_opcode = disable_route;
				enable_motors();
				break;
			case 'd' :
				disable_motors();
				spi_struct->pending_opcode = enable_route;
				break;
			case 'g' :
				spi_struct->pending_opcode = get_cnc_status;
				break;
			case 's' :
				send_spi_string(system_command, MAX_SPI_LENGTH);
				break;
			case 'o' :
				//open_gcode(cnc, system_command+1);
				break;
			case 'r' :
				receive_spi_string(MAX_SPI_LENGTH);
				break;
			case 'u' :
				spi_struct->pending_opcode = flash_firmware;
				break;
			case 'v' :
				spi_struct->pending_opcode = read_version;
				break;
			case 'c' :
				spi_struct->pending_opcode = reconnect_spi;
				break;
			case 'i' :
				spi_struct->pending_opcode = start_cnc_program;
				printf("got start interface function: %d\n", spi_struct->pending_opcode);
				break;
			case 't' :
				printf("got end interface function\n");
				spi_struct->pending_opcode = end_cnc_program;
				break;
			default : {
				spi_struct->pending_opcode = idle;
			}
		}
	}
}
*/
void send_spi_string(char spi_string[MAX_SPI_LENGTH], uint8_t string_length){
	unsigned char spi_data[MAX_SPI_LENGTH] = {0};
	unsigned char snd_data[MAX_SPI_LENGTH] = {0};
	unsigned char ret_data[MAX_SPI_LENGTH] = {0};
	unsigned char loop_cmd[6] = {0};
	int spi_length = 0;
	enum spi_opcodes opcode;
	
	strcpy(spi_data, spi_string+1);
	strcpy(snd_data, spi_data);
	spi_length = strlen(spi_data);
	
	opcode = loopback_test;
	opcode_to_string(opcode, &loop_cmd[0]);
	length_to_string(spi_length, &loop_cmd[OPCODE_LENGTH]);
	
	wiringPiSPIDataRW(SPI_CHANNEL, loop_cmd, IDLE_LENGTH);
	delay(1);
	wiringPiSPIDataRW(SPI_CHANNEL, spi_data, spi_length);
	delay(1);
	wiringPiSPIDataRW(SPI_CHANNEL, ret_data, spi_length);
	delay(1);
	
	spi_length = strlen(spi_data);
	printf("Sent: %s, Read: %s\n", snd_data, ret_data);
}

void receive_spi_string(uint8_t string_length){
	unsigned char spi_data[64] = {0};
	int spi_length = 8;
	int read_length = 0;
	int last_length = 0;
	char read_done = 0;
	printf("Reading Spi Bus\n");
	//while((last_length < spi_length) && (read_done == 0)){
	wiringPiSPIDataRW(SPI_CHANNEL, spi_data, IDLE_LENGTH);
	delay(1);
	last_length = wiringPiSPIDataRW(SPI_CHANNEL, spi_data, 60);
		
		/*for(read_length = 0;read_length < 8; read_length++){
			if(spi_data[last_length + read_length] == '\0'){
				read_done = 1;
				break;
			}
		}*/
		
		//last_length = last_length + read_length;
	//}
	
	printf("Read %d: Data: %s\n", last_length, spi_data);
}

void length_to_string(uint8_t byte_length, char* length_string){
	uint8_t count = 0;
	
	for(count=0;count<byte_length;count++){
		length_string[count] = ((byte_length >> (count*8)) & 0xFF);
	}
}

void opcode_to_string(enum spi_opcodes opcode, char* opcode_string){
	uint8_t count = 0;
	
	for(count=0;count<OPCODE_LENGTH;count++){
		opcode_string[count] = ((opcode >> (count*8)) & 0xFF);
	}
}

void update_si_firmware(struct cnc_spi_struct* spi_struct){
	enum spi_opcodes opcode;
	char send_string[IDLE_LENGTH] = {0};
	if(spi_struct->updating_firmware == 1){
		if(!get_s_ready_state(spi_struct)){
			system("cp ./firmware/spitest.hex /media/pi/STK3402/");
			spi_struct->updating_firmware = 0;
			spi_struct->state = spi_initialized;
		}
	} else {
		if(system("ls /media/pi/STK3402/") == 0){ // need to find better check/mask prints in system call
			spi_struct->updating_firmware = 1;
			opcode = flash_firmware;
			opcode_to_string(opcode, &send_string[0]);
			
			wiringPiSPIDataRW(SPI_CHANNEL, send_string, IDLE_LENGTH);
		} else {
			printf("Can't upgrade firmware - not connected\n");
			spi_struct->state = spi_idle;
		}
	}
}

int open_gcode(struct cnc_state* cnc, char gcode_file[200]){
	printf("openeing file: %s\n", gcode_file);
	cnc->gcode_fp = fopen(gcode_file, "r");
	if(cnc->gcode_fp == NULL){
		printf("Couln't Open File\n");
		return -1;
	} else {
		printf("Opened File!\n");
		
		parse_gcode_file(cnc->gcode_fp, &cnc->program);
		
		fclose(cnc->gcode_fp);
		return 0;
	}
}
