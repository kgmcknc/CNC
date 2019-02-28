
#include "main.h"
#include "revision.h"
#include "keyboard.h"
#include "interface_spi.h"
#include "socket.h"
#include "gcode.h"

char quit_system = 0;
int32_t spi_fd = 0;
pid_t system_control_fork;

uint16_t spi_counter = 0;

int main(int argc, char **argv) {
	uint8_t command_ready = 0;
	uint8_t command_set = 0;
	char system_command[MAX_FUNCTION_STRING];
	struct cnc_state cnc;
	struct spi_struct cnc_spi;
	int socket_addr;
	int wstatus;
	uint8_t connected = 0;
	cnc.spi = &cnc_spi;
	cnc.state = FORK_INPUT;
	cnc.motors_enabled = 0;
	
	char string_counter = 0;
	char control_string[80] = {0};
	
    printf("\n----------------------------------------\n");
    printf("\nStarting CNC Interface\n");
    printf("\nVersion %d.%d.%d.%d\n", FW_REV_BETA, FW_REV_MAJOR, FW_REV_MINOR, FW_REV_PATCH);
    
    printf("Checking to see if spi is enabled on the pi...\n");
    printf("If this crashes, than enable spi on raspberry pi\n");
    check_spi();
    
    printf("Everything safe... Starting initialization Process\n");
    while(quit_system == 0){
		switch(cnc.state){
			case FORK_INPUT : {
				system_control_fork = fork();
				if(system_control_fork == 0){
					cnc.state = CREATE_CHILD_SOCKET;
				} else {
					signal(SIGTERM, &system_shutdown);
					signal(SIGINT, &system_shutdown);
					cnc.state = CREATE_SOCKET;
				}
				break;
			}
			case CREATE_SOCKET : {
				socket_addr = create_unix_socket(CONTROL_SOCKET_PATH);
				if(socket_addr == -1){
					printf("socket attempt failed... try again...\n");
				} else {
					printf("Created Unix Socket\n");
					unix_sockets[active_unix] = socket_addr;
					active_unix = active_unix + 1;
					cnc.state = INTERFACE_INIT;
				}
				break;
			}
			case CREATE_CHILD_SOCKET : {
				control_socket = connect_unix_socket(CONTROL_SOCKET_PATH);
				if(control_socket == -1){
					cnc.state = CREATE_CHILD_SOCKET;
				} else {
					cnc.state = CHILD_PROCESS;
				}
				break;
			}
			case INTERFACE_INIT : {
				printf("Initializing SPI\n");
				init_spi_driver();
				cnc.state = ENABLE_GPIO;
				break;
			}
			case ENABLE_GPIO : {
				printf("\nSetting Up WiringPi\n");
				init_interface_gpio();
				cnc.state = IDLE;
				break;
			}
			case IDLE : {
				char read_string[MAX_SPI_TRANSFER];
				connected = handle_spi();
				if(connected){
					if(spi_check_write() >= 0){
						sprintf(read_string, "Spi Master Test: %u Count", spi_counter);
						spi_counter++;
						spi_set_write(read_string, (uint16_t) strlen(read_string));
					}
					if(spi_check_read(read_string) >= 0){
						spi_set_read();
					}
				} else {
					usleep(50000);
				}
				if(command_set){
					socket_handler(&command_ready, system_command);
					if(command_ready){
						//printf("Data: %c\n", system_command[0]);
						if(system_command[0] == 13 || system_command[0] == 10){
							// new line or CR so finish string
							//printf("Got end char\n");
							control_string[string_counter] = '\0';
							handle_input(&cnc, control_string);
							while(string_counter>1){
								control_string[string_counter] = '\0';
								string_counter--;
							};
							control_string[0] = '\0';
							system_command[0] = '\0';
							string_counter = 0;
							command_set = 0;
						} else {
							if(system_command[0] == 8 || system_command[0] == 127){
								// backspace
								string_counter = (string_counter > 0) ? string_counter - 1 : 0;
								control_string[string_counter] = '\0';
							} else {
								control_string[string_counter] = system_command[0];
								string_counter++;
							}
						}
						//printf("Command: %s\n", control_string);
					} else {
						//check_idle(cnc.spi);
						/*if(cnc.spi->state != spi_idle){
							command_set = 0;
						}*/
					}
				} else {
					//check_idle(cnc.spi);
					command_set = 1;
					print_interface_menu();
				}
				break;
			}
			case HANDLE_SPI : {
				/*if(cnc.spi->state == spi_idle){
					cnc.state = IDLE;
				} else {
					//handle_interface_spi(cnc.spi);
				}*/
				break;
			}
			case PROCESS_INPUT : {
				/*if(cnc.spi->pending_opcode == idle){
					cnc.state = IDLE;
				} else {
					//handle_interface_spi(cnc.spi);
					cnc.state = PROCESS_INPUT;
				}*/
				break;
			}
			case POWER_MOTORS_ON : {
				//if(cnc.spi->pending_opcode == idle){
					enable_motors();
					cnc.motors_enabled = 1;
					cnc.state = IDLE;
				//} else {
					//handle_interface_spi(cnc.spi);
				//}
				break;
			}
			case POWER_MOTORS_OFF : {
				//if(cnc.spi->pending_opcode == idle){
					disable_motors();
					cnc.motors_enabled = 0;
					cnc.state = IDLE;
				//} else {
					//handle_interface_spi(cnc.spi);
				//}
				break;
			}
			case GET_PROGRAM : {
				if(command_set){
					socket_handler(&command_ready, system_command);
					if(command_ready){
						if(system_command[0] == 13 || system_command[0] == 10){
							// new line or CR so finish string
							control_string[string_counter] = '\0';
							string_counter = 0;
							command_set = 0;
							cnc.state = OPEN_PROGRAM;
						} else {
							if(system_command[0] == 8 || system_command[0] == 127){
								// backspace
								string_counter = (string_counter > 0) ? string_counter - 1 : 0;
								control_string[string_counter] = '\0';
							} else {
								control_string[string_counter] = system_command[0];
								string_counter++;
							}
						}
						command_ready = 0;
						printf("\rFile Name: %s", control_string);
					}
				} else {
					printf("Type Full Path to File and Press Enter:\n");
					printf("Typing q and enter will cancel\n");
					command_set = 1;
				}
				break;
			}
			case OPEN_PROGRAM : {
				if((control_string[0] == 'q' || control_string[0] == 'Q') && (control_string[1] == '\0')){
					printf("Going back to idle\n");
					cnc.state = IDLE;
				} else {
					if(open_gcode(&cnc, control_string) == 0){
						printf("File Opened\n");
						cnc.state = RUN_PROGRAM;
					} else {
						cnc.state = IDLE;
					}
				}
				break;
			}
			case RUN_PROGRAM : {
				printf("We would start the program here\n");
				cnc.state = IDLE;
				break;
			}
			case SEND_TO_MICRO : {
				if(command_set){
					socket_handler(&command_ready, system_command);
					if(command_ready){
						if(system_command[0] == 13 || system_command[0] == 10){
							// new line or CR so finish string
							control_string[string_counter] = '\0';
							string_counter = 0;
							command_set = 0;
							cnc.state = OPEN_PROGRAM;
						} else {
							if(system_command[0] == 8 || system_command[0] == 127){
								// backspace
								string_counter = (string_counter > 0) ? string_counter - 1 : 0;
								control_string[string_counter] = '\0';
							} else {
								control_string[string_counter] = system_command[0];
								string_counter++;
							}
						}
						command_ready = 0;
						printf("\rCommand: %s", control_string);
					}
				} else {
					printf("Enter Commands for Machine and press enter\n");
					printf("Typing q and enter will cancel\n");
					command_set = 1;
				}
				break;
			}
			case UPDATE_FIRMARE : {
				//update_si_firmware(spi);
				if(cnc.spi->state == spi_initialized){
					cnc.state = CONNECT_MICRO;
				} else {
					cnc.spi->connected = 0;
					//handle_interface_spi(cnc.spi);
				}
				break;
			}
			case DISABLE_GPIO : {
				disable_interface_gpio();
				cnc.state = EXIT_INTERFACE;
				break;
			}
			case EXIT_INTERFACE : {
				kill(system_control_fork, SIGTERM);
				waitpid(system_control_fork, &wstatus, 0);
				quit_system = 1;
				break;
			}
			case CHILD_PROCESS : {
				system_control(control_socket);
				exit(EXIT_SUCCESS); // should never hit this - parent kills child
				break;
			}
			default : {
				cnc.state = DISABLE_GPIO;
				quit_system = 0;
				break;
			}
		}
	}
	
    return 0;
}

void system_shutdown(int sig){
	int wstatus;
	// disable gpio to shutdown safely
	disable_interface_gpio();
	kill(system_control_fork, SIGTERM);
	waitpid(system_control_fork, &wstatus, 0);
	exit(0);
}

void system_control(int control_socket){
    char control_char;
	uint8_t exit_child = 0;
    // execute system shutdown on sigterm
	signal(SIGTERM, &system_control_shutdown);
	signal(SIGINT, &system_control_shutdown);

    while(!exit_child){
		// use getche to echo each input character
		control_char = getch();
		// send character to main process
        send(control_socket, &control_char, sizeof(control_char), 0);
    }
}
void system_control_shutdown(int sig){
	// reset terminal before exiting
	resetTermios();
	exit(0);
}

void print_interface_menu(void){
	printf("\n\n");
	printf("<<--------------------------->>\n");
	printf("<<--User Input Options------->>\n");
	printf("<<--------------------------->>\n");
	printf("<<--q : Shutdown and Quit---->>\n");
	printf("<<--e : Enable Gpio --------->>\n");
	printf("<<--d : Disable Gpio -------->>\n");
	printf("<<--g : Get CNC Status  ----->>\n");
	printf("<<--s : Send Spi String ----->>\n");
	printf("<<--r : Receive Spi String -->>\n");
	printf("<<--u : Update Si Firmware -->>\n");
	printf("<<--i : Init Program -------->>\n");
	printf("<<--t : Terminate Program --->>\n");
	printf("<<--v : Read Micro Version -->>\n");
	printf("<<--o : Open GCode File    -->>\n");
	printf("<<--c : Reconnect Spi      -->>\n");
	printf("<<--------------------------->>\n");
	printf("\nType Input and Press Enter: ");
}

