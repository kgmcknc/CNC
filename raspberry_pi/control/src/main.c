
#include "main.h"

char idle = 0;

int main(int argc, char **argv) {
    printf("\n----------------------------------------\n");
    printf("\nStarting CNC Controller\n");
    
    printf("\nSetting Up WiringPi\n");
    wiringPiSetup();
    
    init_control_gpio();
    reset_motor_state();
    reset_area_state();

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
			process_instruction();
			process_motion();
			process_motors();
			delayMicroseconds(1);
		}
		
		disable_control_gpio();
		kill(control_socket, SIGKILL);
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
		switch(control_string[0]) {
			case 's' :
				printf("Type String to Send and Press Enter: ");
				printf("\n");
				scanf("%s", control_string+1);
				break;
			case 'm' :
				printf("Enter: \"type.xoffset.yoffset.zoffset.speed.arcradius\n");
				printf("Type 0 is line, type 1 is arc. Speed is Max Speed\n");
				printf("Example: \"0.1000.2000.3000.50.0\" then press enter\n");
				printf("Or enter 0 to stop movement: ");
				printf("\n");
				scanf("%s", control_string+1);
				break;
			case 'g' :
				printf("Type coordinates to move to and what max speed\n");
				printf("X.Y.Xperiod.Yperiod\n");
				printf("Example: \"x2454.y4222.50.50\" then press enter\n");
				printf("Or enter c0.c0.Xper.Yper to go to Center: ");
				printf("\n");
				scanf("%s", control_string+1);
				break;
			default :
				break;
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
	printf("<<--g : Go To --------------->>\n");
	printf("<<--d : Calibrate Motors ---->>\n");
	printf("<<--s : Send Spi String ----->>\n");
	printf("<<--r : Receive Spi String -->>\n");
	printf("<<--p : Print System Info --->>\n");
	printf("<<--------------------------->>\n");
	printf("\nType Input and Press Enter: ");
}


