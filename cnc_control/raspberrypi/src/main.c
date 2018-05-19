#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "cnc_pin_map.h"

#define CONTROL_SOCKET_PATH "\0hb_socket"
#define MAX_UNIX_SOCKETS 1
pid_t system_control_fork;
int control_socket;
int unix_sockets[MAX_UNIX_SOCKETS] = {0};
int active_sockets = 0;
int active_unix = 0;
fd_set all_sockets;
int max_fds;

int main(void) {
    printf("\n----------------------------------------\n");
    printf("\nStarting CNC Controller\n");
    
    printf("\nSetting Up WiringPi\n");
    wiringPiSetup();
    
    init_system();
    
    printf("Everything safe... Turning on driver\n");

    digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_ON);
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

    system_control_fork = fork();
    if(system_control_fork == 0){
		control_socket = connect_unix_socket(CONTROL_SOCKET_PATH);
		system_control(control_socket);
		exit(EXIT_SUCCESS); // should never hit this - parent kills child
	} else {
		
		unix_sockets[active_unix] = create_unix_socket(CONTROL_SOCKET_PATH);
		active_unix = active_unix + 1;
		
		//digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_ON);
		//digitalWrite(DRIVER_SLEEP, MOTOR_RESET_OFF);
		//digitalWrite(DRIVER_ENABLE, LOW);
		for(;;){
			socket_handler();
			/*if(digitalRead(R_END_STOP)){
				digitalWrite(DRIVER_ENABLE, MOTOR_ACTIVE);
			} else {
			   digitalWrite(DRIVER_ENABLE, MOTOR_STANDBY);
			}*/
		delay(1000);
		}
		
		kill(control_socket, SIGKILL);
		shutdown_system();
	}
    
    return 0;
}

void init_system(void){
    printf("Initializing System and Setting GPIO\n");
    pinMode(DRIVER_POWER_RELAY, OUTPUT);
    digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_OFF);
    delay(500);
    pinMode(DRIVER_ENABLE, OUTPUT);
    pinMode(DRIVER_SLEEP, OUTPUT);

    pinMode(L_MOTOR_STP, OUTPUT);
    pinMode(L_MOTOR_DIR, OUTPUT);
    pinMode(L_MOTOR_MS2, OUTPUT);
    pinMode(L_MOTOR_MS1, OUTPUT);
    
    pinMode(F_MOTOR_STP, OUTPUT);
    pinMode(F_MOTOR_DIR, OUTPUT);
    pinMode(F_MOTOR_MS2, OUTPUT);
    pinMode(F_MOTOR_MS1, OUTPUT);

    pinMode(F_END_STOP, INPUT);
    pinMode(B_END_STOP, INPUT);
    pinMode(L_END_STOP, INPUT);
    pinMode(R_END_STOP, INPUT);

    digitalWrite(DRIVER_ENABLE, MOTOR_STANDBY);
    digitalWrite(DRIVER_SLEEP, MOTOR_RESET_ON);
    
    digitalWrite(L_MOTOR_STP, LOW);
    digitalWrite(L_MOTOR_DIR, MOTOR_MOVE_R);
    digitalWrite(L_MOTOR_MS2, LOW);
    digitalWrite(L_MOTOR_MS1, LOW);

    digitalWrite(F_MOTOR_STP, LOW);
    digitalWrite(F_MOTOR_DIR, MOTOR_MOVE_B);
    digitalWrite(F_MOTOR_MS2, LOW);
    digitalWrite(F_MOTOR_MS1, LOW);
	
	printf("GPIO Set To Safe Defaults\n");
	
    delay(2000);
    
}

void shutdown_system(void){
	printf("Setting GPIO To Safe Default For Shutdown\n");
	digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_OFF);
	digitalWrite(DRIVER_ENABLE, MOTOR_STANDBY);
    digitalWrite(DRIVER_SLEEP, MOTOR_RESET_ON);
    
    digitalWrite(L_MOTOR_STP, LOW);
    digitalWrite(L_MOTOR_DIR, MOTOR_MOVE_R);
    digitalWrite(L_MOTOR_MS2, LOW);
    digitalWrite(L_MOTOR_MS1, LOW);

    digitalWrite(F_MOTOR_STP, LOW);
    digitalWrite(F_MOTOR_DIR, MOTOR_MOVE_B);
    digitalWrite(F_MOTOR_MS2, LOW);
    digitalWrite(F_MOTOR_MS1, LOW);
    printf("GPIO Safe... Quitting System\n");
}

int create_unix_socket(char path[MAX_FILE_STRING]){
    // Create a unix socket as a server
    int com_fd, com_socket, com_len, com_opt = 1;
    struct sockaddr_un com_addr;
    com_fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if(com_fd == 0){
        printf("Parent Socket Was 0\n");
        return -1;
    }
    /*if(setsockopt(com_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, (char *) &com_opt, sizeof(com_opt))){
        printf("Parent Socket Opt Failed\n");
        return -1;
    }*/
    memset(&com_addr, 0, sizeof(com_addr));
    com_addr.sun_family = AF_UNIX;
    strncpy(com_addr.sun_path, path, sizeof(com_addr.sun_path)-1);
    printf("Doing Bind in Parent\n");
    if(bind(com_fd, (struct sockaddr*) &com_addr, sizeof(com_addr)) < 0){
        printf("Failed in Parent Bind\n");
        return -1;
    }
    printf("Doing Listen in Parent\n");
    if(listen(com_fd, 1) < 0){
        printf("Failed in Parent Listen\n");
        return -1;
    }
    com_len = sizeof(com_addr);
    printf("Doing Accept in Parent\n");
    com_socket = accept(com_fd, (struct sockaddr*) &com_addr, (socklen_t*) &com_len);
    if(com_socket < 0){
        printf("Failed in Parent Accept\n");
        return -1;
    }
    return com_socket;
}

int connect_unix_socket(char path[MAX_FILE_STRING]){
    // connect to unix_socket as client
    int com_fd, com_socket, com_len, com_opt = 1;
    struct sockaddr_un com_addr;
    com_socket = socket(AF_UNIX, SOCK_STREAM, 0);
    memset(&com_addr, 0, sizeof(com_addr));
    com_addr.sun_family = AF_UNIX;
    strncpy(com_addr.sun_path, path, sizeof(com_addr.sun_path)-1);
    printf("Doing Connect in Listener\n");
    connect(com_socket, (struct sockaddr *)&com_addr, sizeof(com_addr));
    return com_socket;
}

int socket_handler(void){
    int s_count, r_count;
    int activity;
    int new_socket = 0;
    int new_addr_len;
    int num_read;
    int shift;
    char read_data[MAX_FUNCTION_STRING];
    struct sockaddr_in new_addr;
    new_addr_len = sizeof(new_addr);
    
    printf("Active Sockets: %d\n", active_unix);

    // clear socket set
    FD_ZERO(&all_sockets);
    // always update max file descriptor for select
    max_fds = 0;

    // set unix sockets
    for(s_count=0;s_count<MAX_UNIX_SOCKETS;s_count++){
        if(s_count >= active_unix) break;
        FD_SET(unix_sockets[s_count], &all_sockets);
        if(unix_sockets[s_count] > max_fds) max_fds = unix_sockets[s_count];
    }
    
    // wait for activity on any sockets, timeout in timeout time
    // nfds, readfds, writefds, exeptfds, timeout
    printf("Starting Select\n");
    activity = select((max_fds + 1), &all_sockets, NULL, NULL, NULL);
    if(activity < 0){
        printf("---- ERROR IN SERVER ---\n");
    }
    printf("Got Something...\n");
    
    // check unix sockets
    for(s_count=0;s_count<MAX_UNIX_SOCKETS;s_count++){
        if(s_count >= active_unix) break;
        if(FD_ISSET(unix_sockets[s_count], &all_sockets)){
            printf("Got Data On Unix:%d\n", s_count);
            num_read = read(unix_sockets[s_count], read_data, sizeof(read_data));
            if(num_read > 0){
                // Got Data
                read_data[num_read] = '\0';
                printf("got: %s\n", read_data);
                //process(0, s_count, read_data);
            } else {
                if(num_read == 0){
                    // No Data... Closing Connection
                    printf("Closing Unix Socket: %d\n", s_count);
                    close(unix_sockets[s_count]);
                    unix_sockets[s_count] = 0;
                } else {
                    // Error... Closing Connection
                    printf("Error On Unix Socket: %d\n", s_count);
                    close(unix_sockets[s_count]);
                    unix_sockets[s_count] = 0;
                }
            }
        }
    }
    
    // shift down to remove gaps in clients after one closes
    shift = 0;
    for(r_count=0;r_count<active_unix;r_count++){
        if(unix_sockets[r_count] == 0){
            while((unix_sockets[r_count+shift] == 0)){
                if((r_count+shift)<active_unix){
                    shift = shift + 1;
                } else {
                    break;
                }
            }
            unix_sockets[r_count] = unix_sockets[r_count+shift];
            active_unix = active_unix - 1;
        }
    }
    
    return 0;
}


void system_control(int control_socket){
    int exit_system = 0;
    char control_string[40];
    // parent will close system control child - don't ever exit
    while(!exit_system){
        printf("test user input:\n");
		scanf("%s", control_string);
        send(control_socket, control_string, sizeof(control_string), 0);
    }
    
}
