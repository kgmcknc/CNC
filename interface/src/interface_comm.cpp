#include "main.h"
#include "interface_serial.h"

//void display_cnc_print(struct spi_struct* spi){
	/*char read_string[PRINT_LENGTH];
	if(spi->transfer_length > PRINT_LENGTH) spi->transfer_length = PRINT_LENGTH;
	//wiringPiSPIDataRW(SPI_CHANNEL, spi->spi_data, spi->transfer_length+IDLE_LENGTH);
	strncpy(read_string, spi->spi_data + IDLE_LENGTH, spi->transfer_length);
	read_string[spi->transfer_length] = '\0';
	printf("CNC Print: %s\n", read_string);
	spi->state = spi_idle;
	spi->pending_opcode = idle;*/
//}

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
/*
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
	//if(get_s_ready_state(spi)){
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
	//}
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
*/