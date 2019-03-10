#include "main.h"
#include "stdio.h"
#include "common_spi.h"
#include "common_cnc.h"
#include "interface_spi.h"
#include "gcode.h"

uint8_t print_set = 0;

void handle_cnc_state(struct interface_struct* interface){
	switch(interface->machine_state){
		case MACHINE_IDLE : {
			if(check_spi(interface)){
				// received spi transaction
				// process that instead of user input
				interface->machine_state = PROCESS_SPI;
			} else {
				// if not processing spi, process user input
				socket_handler(&interface->user_command_set, interface->user_input);
				receive_user_input(interface);
				if(interface->user_command_finished){
					interface->machine_state = PROCESS_INPUT;
				}
			}
			break;
		}
		case PROCESS_SPI : {
			process_spi_request(interface);
			break;
		}
		case PROCESS_INPUT : {
			handle_input(interface);
			break;
		}
		/*case SEND_TEST_STRING : {
			int32_t value;
			char read_string[MAX_SPI_TRANSFER];
			read_string[0] = (char) SEND_TEST_STRING;
			strcpy(&read_string[1], "thisislongthisislongthisislongthisislongthisislongthisislongthisislongthisislongthisislongthisislong");
			//value = spi_set_write(interface->user_command, strlen(interface->user_command));
			value = spi_set_write(read_string, strlen(read_string));
			if(value > 0){
				printf("sent data: %s\n", &interface->user_command[1]);
				interface->machine_state = MACHINE_IDLE;
			} else {
				printf("write hasn't finished\n");
			}
			break;
		}*/
		case POWER_MOTORS_ON : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					enable_motors();
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				interface->cnc_write_data[0] = (char) DISABLE_ROUTE;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = POWER_MOTORS_ON;
			}
			break;
		}
		case POWER_MOTORS_OFF : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					disable_motors();
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				interface->cnc_write_data[0] = (char) ENABLE_ROUTE;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = POWER_MOTORS_OFF;
			}
			break;
		}
		case GET_STATUS : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				interface->cnc_write_data[0] = (char) GET_CNC_STATUS;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = GET_STATUS;
			}
			break;
		}
		case GET_VERSION : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				interface->cnc_write_data[0] = (char) GET_CNC_VERSION;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = GET_VERSION;
			}
			break;
		}
		case USER_CONTROL : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
				}
			} else {
				socket_handler(&interface->user_command_set, interface->user_input);
				receive_user_input(interface);
				// check if enter was set
				// if so
				// interface->machine_state = MACHINE_IDLE;
				// else
				// send instruction
				//interface->cnc_write_data[0] = (char) GET_CNC_VERSION;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = USER_CONTROL;
			}
			break;
		}
		case GET_PROGRAM : {
			interface->machine_state = MACHINE_IDLE;
			break;
		}
		case OPEN_PROGRAM : {
			interface->machine_state = MACHINE_IDLE;
			break;
		}
		case RUN_PROGRAM : {
			interface->machine_state = MACHINE_IDLE;
			break;
		}
		case UPDATE_FIRMARE : {
			update_si_firmware(interface);
			interface->machine_state = MACHINE_IDLE;
			break;
		}
		default : {
			interface->machine_state = MACHINE_IDLE;
			break;
		}
	}
}

uint8_t check_spi(struct interface_struct* interface){
	// always check write to complete transfer if done
	spi_check_write();
	// check if read is finished
	interface->cnc_read_length = spi_check_read(interface->cnc_read_data);
	if(interface->cnc_read_length == 0){
		// not reading, so set read ready
		spi_set_read();
		return 0;
	} else {
		if(interface->cnc_read_length > 0){
			// read finished, so process
			return 1;
		} else {
			return 0;
		}
	}
}

void receive_user_input(struct interface_struct* interface){
	if(print_set){
		if(interface->user_command_set && !interface->user_command_finished){
			if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
				// new line or CR so finish string
				printf("\n");
				interface->user_command[interface->command_counter] = '\0';
				if((interface->user_command[0] > 64) && (interface->user_command[0] < 91)){
					interface->user_command[0] = interface->user_command[0] + 32;
				}
				while(interface->command_counter>1){
					interface->user_command[interface->command_counter] = '\0';
					interface->command_counter--;
				};
				interface->user_command_finished = 1;
				print_set = 0;
			} else {
				if(interface->user_input[0] == 8 || interface->user_input[0] == 127){
					// backspace
					interface->command_counter = (interface->command_counter > 0) ? interface->command_counter - 1 : 0;
					interface->user_command[interface->command_counter] = ' ';
					printf("\rCommand: %s", interface->user_command);
					fflush(stdout);
					interface->user_command[interface->command_counter] = '\0';
				} else {
					interface->user_command[interface->command_counter] = interface->user_input[0];
					interface->command_counter++;
					interface->user_command[interface->command_counter] = '\0';
				}
			}
			interface->user_command_set = 0;
			printf("\rCommand: %s", interface->user_command);
			fflush(stdout);
		}
	} else {
		print_set = 1;
		printf("Enter Command and Press Enter. Enter h For Help\n");
		printf("\rCommand: %s", interface->user_command);
		fflush(stdout);
	}
}

void process_spi_request(struct interface_struct* interface){
	// setting machine state to default...
	// it will change in switch if needed
	interface->machine_state = MACHINE_IDLE;
	switch((CNC_OPCODES) interface->cnc_read_data[0]){
		case GET_CNC_VERSION : {
			printf("CNC Version: %s\n", &interface->cnc_read_data[1]);
			break;
		}
		case GET_CNC_STATUS : {
			printf("CNC Status: %s\n", &interface->cnc_read_data[1]);
			break;
		}
		case NEW_CNC_PRINT : {
			printf("CNC Printf: %s\n", &interface->cnc_read_data[1]);
			break;
		}
		case FLASH_FIRMWARE : {

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
			interface->machine_state = USER_CONTROL;
			printf("Getting User Input To Control Machine:\n");
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
			interface->machine_state = MACHINE_IDLE;
			break;
		}
	}
}

void handle_input(struct interface_struct* interface){
	// set states back to default states...
	// all tasks that need it to move to another state will set it
	interface->state = INTERFACE_RUNNING;
	interface->machine_state = MACHINE_IDLE;
	if(interface->user_command_finished){
		switch(interface->user_command[0]){
			case 'q' : {
				interface->state = DISABLE_GPIO;
				break;
			}
			case 'h' : {
				print_interface_menu();
				break;
			}
			/*case 's' : {
				interface->machine_state = SEND_TEST_STRING;
				break;
			}*/
			case 'u' : {
				interface->machine_state = UPDATE_FIRMARE;
				break;
			}
			case 'r' : {
				spi_reconnect();
				break;
			}
			case 'g' : {
				interface->machine_state = GET_STATUS;
				break;
			}
			case 'v' : {
				interface->machine_state = GET_VERSION;
				break;
			}
			case 'e' : {
				interface->machine_state = POWER_MOTORS_ON;
				break;
			}
			case 'd' : {
				interface->machine_state = POWER_MOTORS_OFF;
				break;
			}
			/*case 'r' : {
				char read_string[MAX_SPI_TRANSFER];
				uint16_t len;
				value = spi_check_read(read_string);
				if(value > 0){
					printf("read data: %s, %d\n", read_string, value);
				} else {
					if(value < 0){
						printf("read hasn't finished\n");
					} else {
						spi_set_read();
					}
				}
				break;
			}
			case 'c' : {
				//cnc->spi->pending_opcode = reconnect_spi;
				cnc->state = PROCESS_INPUT;
				break;
			}
			case 'o' : {
				cnc->state = GET_PROGRAM;
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
				interface->state = INTERFACE_RUNNING;
				break;
			}
		}
		interface->user_command[0] = '\0';
		interface->user_input[0] = '\0';
		interface->command_counter = 0;
		interface->user_command_set = 0;
		interface->user_command_finished = 0;
	}
}

/*
void interface_functions(uint8_t command_ready, char* system_command, struct cnc_spi_struct* spi_struct, struct cnc_state* cnc){
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
*/
void receive_spi_string(uint8_t string_length){
	unsigned char spi_data[64] = {0};
	int spi_length = 8;
	int read_length = 0;
	int last_length = 0;
	char read_done = 0;
	printf("Reading Spi Bus\n");
	//while((last_length < spi_length) && (read_done == 0)){
	//wiringPiSPIDataRW(SPI_CHANNEL, spi_data, IDLE_LENGTH);
	delay(1);
	//last_length = wiringPiSPIDataRW(SPI_CHANNEL, spi_data, 60);
		
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

void update_si_firmware(struct interface_struct* interface){
	//enum spi_opcodes opcode;
	//char send_string[IDLE_LENGTH] = {0};
	//if(spi->updating_firmware == 1){
		//if(!get_s_ready_state(spi)){
			system("cp \"../machine/GNU ARM v7.2.1 - Debug/machine.hex\" /media/pi/STK3402/");
			//spi->updating_firmware = 0;
			//spi->state = spi_initialized;
		//}
	//} else {
		if(system("ls /media/pi/STK3402/") == 0){ // need to find better check/mask prints in system call
			//spi->updating_firmware = 1;
			//opcode = flash_firmware;
			//opcode_to_string(opcode, &send_string[0]);
			system("cp \"../machine/GNU ARM v7.2.1 - Debug/machine.hex\" /media/pi/STK3402/");
			//wiringPiSPIDataRW(SPI_CHANNEL, send_string, IDLE_LENGTH);
		} else {
			printf("Can't upgrade firmware - not connected\n");
			//spi->state = spi_idle;
		}
		spi_reconnect();
	//}
	interface->machine_state = MACHINE_IDLE;
}

int open_gcode(struct interface_struct* interface, char gcode_file[200]){
	printf("openeing file: %s\n", gcode_file);
	interface->gcode_fp = fopen(gcode_file, "r");
	if(interface->gcode_fp == NULL){
		printf("Couln't Open File\n");
		return -1;
	} else {
		printf("Opened File!\n");
		
		parse_gcode_file(interface->gcode_fp, &interface->program);
		
		fclose(interface->gcode_fp);
		return 0;
	}
}


// case HANDLE_SPI : {
// 				/*if(cnc.spi->state == spi_idle){
// 					cnc.state = IDLE;
// 				} else {
// 					//handle_interface_spi(cnc.spi);
// 				}*/
// 				break;
// 			}
// 			case PROCESS_INPUT : {
// 				/*if(cnc.spi->pending_opcode == idle){
// 					cnc.state = IDLE;
// 				} else {
// 					//handle_interface_spi(cnc.spi);
// 					cnc.state = PROCESS_INPUT;
// 				}*/
// 				break;
// 			}
// 			case POWER_MOTORS_ON : {
// 				//if(cnc.spi->pending_opcode == idle){
// 					enable_motors();
// 					cnc.motors_enabled = 1;
// 					cnc.state = IDLE;
// 				//} else {
// 					//handle_interface_spi(cnc.spi);
// 				//}
// 				break;
// 			}
// 			case POWER_MOTORS_OFF : {
// 				//if(cnc.spi->pending_opcode == idle){
// 					disable_motors();
// 					cnc.motors_enabled = 0;
// 					cnc.state = IDLE;
// 				//} else {
// 					//handle_interface_spi(cnc.spi);
// 				//}
// 				break;
// 			}
// 			case GET_PROGRAM : {
// 				if(command_set){
// 					socket_handler(&command_ready, system_command);
// 					if(command_ready){
// 						if(system_command[0] == 13 || system_command[0] == 10){
// 							// new line or CR so finish string
// 							control_string[string_counter] = '\0';
// 							string_counter = 0;
// 							command_set = 0;
// 							cnc.state = OPEN_PROGRAM;
// 						} else {
// 							if(system_command[0] == 8 || system_command[0] == 127){
// 								// backspace
// 								string_counter = (string_counter > 0) ? string_counter - 1 : 0;
// 								control_string[string_counter] = '\0';
// 							} else {
// 								control_string[string_counter] = system_command[0];
// 								string_counter++;
// 							}
// 						}
// 						command_ready = 0;
// 						printf("\rFile Name: %s", control_string);
// 					}
// 				} else {
// 					printf("Type Full Path to File and Press Enter:\n");
// 					printf("Typing q and enter will cancel\n");
// 					command_set = 1;
// 				}
// 				break;
// 			}
// 			case OPEN_PROGRAM : {
// 				if((control_string[0] == 'q' || control_string[0] == 'Q') && (control_string[1] == '\0')){
// 					printf("Going back to idle\n");
// 					cnc.state = IDLE;
// 				} else {
// 					if(open_gcode(&cnc, control_string) == 0){
// 						printf("File Opened\n");
// 						cnc.state = RUN_PROGRAM;
// 					} else {
// 						cnc.state = IDLE;
// 					}
// 				}
// 				break;
// 			}
// 			case RUN_PROGRAM : {
// 				printf("We would start the program here\n");
// 				cnc.state = IDLE;
// 				break;
// 			}
// 			case SEND_TO_MICRO : {
// 				if(command_set){
// 					socket_handler(&command_ready, system_command);
// 					if(command_ready){
// 						if(system_command[0] == 13 || system_command[0] == 10){
// 							// new line or CR so finish string
// 							control_string[string_counter] = '\0';
// 							string_counter = 0;
// 							command_set = 0;
// 							cnc.state = OPEN_PROGRAM;
// 						} else {
// 							if(system_command[0] == 8 || system_command[0] == 127){
// 								// backspace
// 								string_counter = (string_counter > 0) ? string_counter - 1 : 0;
// 								control_string[string_counter] = '\0';
// 							} else {
// 								control_string[string_counter] = system_command[0];
// 								string_counter++;
// 							}
// 						}
// 						command_ready = 0;
// 						printf("\rCommand: %s", control_string);
// 					}
// 				} else {
// 					printf("Enter Commands for Machine and press enter\n");
// 					printf("Typing q and enter will cancel\n");
// 					command_set = 1;
// 				}
// 				break;
// 			}
// 			case UPDATE_FIRMARE : {
// 				//update_si_firmware(spi);
// 				if(cnc.spi->state == spi_initialized){
// 					cnc.state = CONNECT_MICRO;
// 				} else {
// 					cnc.spi->connected = 0;
// 					//handle_interface_spi(cnc.spi);
// 				}
// 				break;
// 			}