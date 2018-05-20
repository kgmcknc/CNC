#include "main.h"

void interface_functions(void){
	
	switch(system_command[0]){
		case 'q' :
			quit_system = 1;
			break;
		case 'e' :
			
			break;
		case 'd' :
			
			break;
		case 's' :
			send_spi_string(system_command);
			break;
		case 'r' :
			receive_spi_string();
			break;
		default :
			
			break;
	}
};

void send_spi_string(char spi_string[200]){
	char spi_data[200] = {0};
	int spi_length = 0;
	strcpy(spi_data, spi_string+1);
	spi_length = strlen(spi_data);
	wiringPiSPIDataRW(SPI_CHANNEL, spi_data, spi_length);
	spi_length = strlen(spi_data);
	printf("Read with Send%d: %s\n", spi_length, spi_data);
}

void receive_spi_string(void){
	char spi_data[200] = {0};
	int spi_length = 200;
	int read_length = 0;
	int last_length = 0;
	char read_done = 0;
	printf("Reading Spi Bus\n");
	while((last_length < (spi_length-9)) && (read_done == 0)){
		wiringPiSPIDataRW(SPI_CHANNEL, spi_data + last_length, 8);
		
		for(read_length = 0;read_length < 8; read_length++){
			if(spi_data[last_length + read_length] == '\0'){
				read_done = 1;
				break;
			}
		}
		
		last_length = last_length + read_length;
	}
	
	printf("Read %d: %s\n", last_length, spi_data);
}
