
#include "main.h"


int main(int argc, char **argv) {
    printf("\n----------------------------------------\n");
    printf("\nStarting CNC Controller\n");
    
    printf("\nSetting Up WiringPi\n");
    wiringPiSetup();
    
    init_control_gpio();

    system_control_fork = fork();
    if(system_control_fork == 0){
		printf("Forked... In child process\n");
		control_socket = connect_unix_socket(CONTROL_SOCKET_PATH);
		system_control(control_socket);
		exit(EXIT_SUCCESS); // should never hit this - parent kills child
	} else {
		printf("Forked... In Parent process\n");
		unix_sockets[active_unix] = create_unix_socket(CONTROL_SOCKET_PATH);
		active_unix = active_unix + 1;
		
		while(quit_system == 0){
			socket_handler();
			if(command_ready){
				printf("Got %s\n", system_command);
				control_functions();
			} else {
				
			}
			if(calibration_running) process_calibration();
			process_motors();
			delayMicroseconds(1);
		}
		
		kill(control_socket, SIGKILL);
		disable_control_gpio();
	}
    
    return 0;
}

void system_control(int control_socket){
    int exit_system = 0;
    char control_string[40];
    // parent will close system control child - don't ever exit
    while(!exit_system){
		sleep(1);
        print_control_menu();
		scanf("%s", control_string);
		if(control_string[0] == 'm'){
			printf("Type direction to move and how many steps\n");
			printf("Example: \"r1000\" then press enter\n");
			printf("Or enter 0 to stop movement: ");
			printf("\n");
			scanf("%s", control_string+1);
		}
		if(control_string[0] == 's'){
			printf("Type String to Send and Press Enter: ");
			printf("\n");
			scanf("%s", control_string+1);
		}
        send(control_socket, control_string, sizeof(control_string), 0);
    }
}

void print_control_menu(void){
	printf("\n\n");
	printf("<<--------------------------->>\n");
	printf("<<--User Input Options------->>\n");
	printf("<<--------------------------->>\n");
	printf("<<--q : Shutdown and Quit---->>\n");
	printf("<<--e : Enable Gpio --------->>\n");
	printf("<<--d : Disable Gpio -------->>\n");
	printf("<<--m : Move Axis ----------->>\n");
	printf("<<--d : Calibrate Motors ---->>\n");
	printf("<<--s : Send Spi String ----->>\n");
	printf("<<--r : Receive Spi String -->>\n");
	printf("<<--------------------------->>\n");
	printf("\nType Input and Press Enter: ");
}


