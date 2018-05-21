#include "main.h"

char quit_system = 0;
char calibration_running = 0;
char calibration_state = 0;

void control_functions(void){
	
	switch(system_command[0]){
		case 'q' :
			quit_system = 1;
			break;
		case 'e' :
			enable_motors();
			break;
		case 'd' :
			disable_motors();
			break;
		case 'c' :
			calibrate_system();
			break;
		case 's' :
			send_spi_string(system_command);
			break;
		case 'r' :
			receive_spi_string();
			break;
		case 'm' :
			move(system_command);
			break;
		default :
			
			break;
	}
};

void enable_motors(void){
	printf("enabling motors\n");
	digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_ON);
	digitalWrite(DRIVER_SLEEP, MOTOR_RESET_OFF);
	digitalWrite(DRIVER_ENABLE, MOTOR_ACTIVE);
}

void disable_motors(void){
	printf("disabling motors\n");
	digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_OFF);
	digitalWrite(DRIVER_SLEEP, MOTOR_RESET_ON);
	digitalWrite(DRIVER_ENABLE, MOTOR_STANDBY);
}

void calibrate_system(void){
	if(calibration_running){
		disable_motors();
		printf("done calibration\n");
		calibration_running = 0;
		calibration_state = 0;
	} else {
		printf("calibrating system\n");
		disable_motors();
		reset_motor_state();
		reset_area_state();
		enable_motors();
		calibration_state = 0;
		calibration_running = 1;
	}
}

void move(char move_string[200]){
	char dir;
	char cmd;
	unsigned long int move_count;
	sscanf(move_string, "%c%c%u", &cmd, &dir, &move_count);
	if(dir == '0'){
		printf("Stopping Movement\n");
		cur_dyn.x_total = 0;
		cur_dyn.y_total = 0;
		cur_dyn.z_total = 0;
	} else {
		if(dir == 'l'){
			cur_dyn.x_total = NORNAL_SPEED;
			cur_dyn.x_dir = MOTOR_MOVE_L;
			printf("Moving Left\n");
		}
		if(dir == 'r'){
			cur_dyn.x_total = NORNAL_SPEED;
			cur_dyn.x_dir = MOTOR_MOVE_R;
			printf("Moving Right\n");
		}
		if(dir == 't'){
			cur_dyn.y_total = NORNAL_SPEED;
			cur_dyn.y_dir = MOTOR_MOVE_T;
			printf("Moving Towards\n");
		}
		if(dir == 'a'){
			cur_dyn.y_total = NORNAL_SPEED;
			cur_dyn.y_dir = MOTOR_MOVE_A;
			printf("Moving Away\n");
		}
	}
}

void process_calibration(void){
	switch(calibration_state){
		case 0 :
			printf("Cal 0 Moving Left and Towards\n");
			cur_dyn.x_dir = MOTOR_MOVE_L;
			cur_dyn.x_total = NORNAL_SPEED;
			cur_dyn.y_dir = MOTOR_MOVE_T;
			cur_dyn.y_total = NORNAL_SPEED;
			calibration_state = 1;
			break;
		case 1 :
			if(cur_pos.x_l_stop == ENDSTOP_HIT){
				if(cur_dyn.x_total) printf("Hit Left End\n");
				cur_pos.x = 0;
				cur_dyn.x_total = 0;
			}
			if(cur_pos.y_c_stop == ENDSTOP_HIT){
				if(cur_dyn.y_total) printf("Hit Close End\n");
				cur_pos.y = 0;
				cur_dyn.y_total = 0;
			}
			if((cur_dyn.x_total == 0) && (cur_dyn.y_total == 0)){
				printf("Moving Right and Away\n");
				cur_dyn.x_dir = MOTOR_MOVE_R;
				cur_dyn.x_total = NORNAL_SPEED;
				cur_dyn.y_dir = MOTOR_MOVE_A;
				cur_dyn.y_total = NORNAL_SPEED;
				calibration_state = 2;
			}
			break;
		case 2 :
			if(cur_pos.x_r_stop == ENDSTOP_HIT){
				if(cur_dyn.x_total) printf("Hit Right End\n");
				w_size.x = cur_pos.x;
				w_size.x_calibrated = 1;
				cur_dyn.x_total = 0;
			}
			if(cur_pos.y_f_stop == ENDSTOP_HIT){
				if(cur_dyn.y_total) printf("Hit Far End\n");
				w_size.y = cur_pos.y;
				w_size.y_calibrated = 1;
				cur_dyn.y_total = 0;
			}
			if((cur_dyn.x_total == 0) && (cur_dyn.y_total == 0)){
				printf("Finished - Move to Left and Towards to Middle\n");
				cur_dyn.x_dir = MOTOR_MOVE_L;
				cur_dyn.x_total = NORNAL_SPEED;
				cur_dyn.y_dir = MOTOR_MOVE_T;
				cur_dyn.y_total = NORNAL_SPEED;
				calibration_state = 3;
			}
			break;
		case 3 :
			if(cur_pos.x <= (w_size.x/2)){
				cur_dyn.x_total = 0;
			}
			if(cur_pos.y <= (w_size.y/2)){
				cur_dyn.y_total = 0;
			}
			if((cur_dyn.x_total == 0) && (cur_dyn.y_total == 0)){
				printf("Done Calibration\n");
				printf("X: %lu, Y: %lu\n", w_size.x, w_size.y);
				calibrate_system();
				calibration_state = 0;
			}
			break;
		default :
		
			break;
	}
}

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


