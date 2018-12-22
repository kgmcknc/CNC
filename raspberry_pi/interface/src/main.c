
#include "main.h"
#include "interface_spi.h"

char quit_system = 0;
int32_t spi_fd = 0;

int main(int argc, char **argv) {
	uint8_t command_ready = 0;
	uint8_t system_command[MAX_FUNCTION_STRING];
	struct cnc_spi_struct cnc_spi;
	
    printf("\n----------------------------------------\n");
    printf("\nStarting CNC Interface\n");
    
    printf("\nSetting Up WiringPi\n");
    init_interface_gpio();
    
    printf("Everything safe... Turning on driver\n");

    system_control_fork = fork();
    if(system_control_fork == 0){
		printf("Forked... In child process\n");
		control_socket = connect_unix_socket(CONTROL_SOCKET_PATH);
		delay(250);
		system_control(control_socket);
		exit(EXIT_SUCCESS); // should never hit this - parent kills child
	} else {
		printf("Forked... In Parent process\n");
		struct cnc_state cnc;
		init_spi(&cnc_spi);
		unix_sockets[active_unix] = create_unix_socket(CONTROL_SOCKET_PATH);
		active_unix = active_unix + 1;
		
		while(quit_system == 0){
			socket_handler(&command_ready, system_command);
			//input_handler();
			interface_functions(command_ready, system_command, &cnc_spi, &cnc);
			command_ready = 0;
			handle_spi(&cnc_spi);
			
		    delay(100);
		}
		
		kill(system_control_fork, SIGKILL);
		disable_interface_gpio();
	}
    
    return 0;
}

void system_control(int control_socket){
    int exit_system = 0;
    char control_string[80] = {0};
    // parent will close system control child - don't ever exit
    while(!exit_system){
		sleep(1);
        print_interface_menu();
		scanf("%s", control_string);
		printf("\n");
		if(control_string[0] == 's'){
			printf("Type String to Send and Press Enter: ");
			printf("\n");
			scanf("%s", control_string+1);
		}
		if(control_string[0] == 'o'){
			printf("Type Full Path to File and Press Enter: ");
			printf("\n");
			scanf("%s", control_string+1);
		}
        send(control_socket, control_string, sizeof(control_string), 0);
    }
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

