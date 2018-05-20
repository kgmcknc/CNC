
#include "main.h"

int main(int argc, char **argv) {
    printf("\n----------------------------------------\n");
    printf("\nStarting CNC Controller\n");
    
    printf("\nSetting Up WiringPi\n");
    wiringPiSetup();
    
    init_interface_gpio();
    
    printf("Everything safe... Turning on driver\n");

    /*digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_ON);
    digitalWrite(DRIVER_SLEEP, MOTOR_RESET_OFF);
    digitalWrite(DRIVER_ENABLE, LOW);
    char toggle = 0;
    digitalWrite(L_MOTOR_DIR, MOTOR_MOVE_R);
    digitalWrite(F_MOTOR_DIR, MOTOR_MOVE_F);
    for(;;){
        if(toggle){
	    digitalWrite(L_MOTOR_STP, HIGH);
            digitalWrite(F_MOTOR_STP, HIGH);
	    toggle = 0;
	} else {
	   toggle = 1;
	   digitalWrite(L_MOTOR_STP, LOW);
           digitalWrite(F_MOTOR_STP, LOW);
	}
	delayMicroseconds(1000);
    }
*/
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
		
		//digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_ON);
		//digitalWrite(DRIVER_SLEEP, MOTOR_RESET_OFF);
		//digitalWrite(DRIVER_ENABLE, LOW);
		for(;;){
			socket_handler();
			if(command_ready){
				printf("Got %s\n", system_command);
			} else {
				
			}
			/*if(digitalRead(R_END_STOP)){
				digitalWrite(DRIVER_ENABLE, MOTOR_ACTIVE);
			} else {
			   digitalWrite(DRIVER_ENABLE, MOTOR_STANDBY);
			}*/
		delay(1000);
		}
		
		kill(control_socket, SIGKILL);
		disable_interface_gpio();
	}
    
    return 0;
}

