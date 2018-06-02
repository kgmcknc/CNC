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
		case 'g' :
			go_to(system_command);
			break;
		case 'p' :
			print_system_info();
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
	motors_enabled = 1;
}

void disable_motors(void){
	printf("disabling motors\n");
	digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_OFF);
	digitalWrite(DRIVER_SLEEP, MOTOR_RESET_ON);
	digitalWrite(DRIVER_ENABLE, MOTOR_STANDBY);
	motors_enabled = 0;
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
	int type;
	unsigned int move_speed;
	unsigned long int total_move;
	long int x_count, y_count, z_count;
	unsigned int radius;
	char input_count = 0;
	input_count = sscanf(move_string, "%c%d.%ld.%ld.%ld.%u.%u", &cmd, &type, &x_count, &y_count, &z_count, &move_speed, &radius);
	//printf("Count: %d, Moving: T: %d, x:%ld, y:%ld, z:%ld, speed:%u, radius:%u\n", input_count, type, x_count, y_count, z_count, move_speed, radius);
	if((input_count < 7) || (!x_count && !y_count && !z_count)){
		printf("Stopping Movement\n");
		current_move.x_period_count = 0;
		current_move.x_act = 0;
		current_move.y_period_count = 0;
		current_move.y_act = 0;
		current_move.z_period_count = 0;
		current_move.z_act = 0;
	} else {
		if(x_count == 0){
			current_move.x_period_count = 0;
			current_move.x_act = 0;
			printf("Stopped X\n");
		} else {
			if(x_count > 0){
				current_move.x_move = x_count;
				current_move.x_dir = MOTOR_MOVE_R;
				printf("Moving X Right\n");
			} else {
				current_move.x_move = x_count*(-1);
				current_move.x_dir = MOTOR_MOVE_L;
				printf("Moving X Left\n");
			}
			end_position.x = current_position.x + x_count;
			current_move.x_move_count = current_move.x_move;
			if(current_move.x_move_count){
				if(move_speed < START_SPEED){
					current_move.x_period = move_speed;
					current_move.x_current_period = START_SPEED;
				} else {
					current_move.x_period = move_speed;
					current_move.x_current_period = move_speed;
				}
			}
			current_move.x_act = 1;
			current_move.x_arc = type;
		}
		if(y_count == 0){
			current_move.y_period_count = 0;
			current_move.y_act = 0;
			printf("Stopped Y\n");
		} else {
			if(y_count > 0){
				current_move.y_move = y_count;
				current_move.y_dir = MOTOR_MOVE_A;
				printf("Moving Y Away\n");
			} else {
				current_move.y_move = y_count*(-1);
				current_move.y_dir = MOTOR_MOVE_T;
				printf("Moving Y Towards");
			}
			end_position.y = current_position.y + y_count;
			current_move.y_move_count = current_move.y_move;
			if(current_move.y_move_count){
				if(move_speed < START_SPEED){
					current_move.y_period = move_speed;
					current_move.y_current_period = START_SPEED;
				} else {
					current_move.y_period = move_speed;
					current_move.y_current_period = move_speed;
				}
			}
			current_move.y_act = 1;
			current_move.y_arc = type;
		}
		if(z_count == 0){
			current_move.z_period_count = 0;
			current_move.z_act = 0;
			printf("Stopped Z\n");
		} else {
			if(z_count > 0){
				current_move.z_move = z_count;
				current_move.z_dir = MOTOR_MOVE_U;
				printf("Moving Z Up\n");
			} else {
				current_move.z_move = z_count*(-1);
				current_move.z_dir = MOTOR_MOVE_D;
				printf("Moving Z Down\n");
			}
			end_position.z = current_position.z + z_count;
			current_move.z_move_count = current_move.z_move;
			if(current_move.z_move_count){
				if(move_speed < START_SPEED){
					current_move.z_period = move_speed;
					current_move.z_current_period = START_SPEED;
				} else {
					current_move.z_period = move_speed;
					current_move.z_current_period = move_speed;
				}
			}
			current_move.z_act = 1;
			current_move.z_arc = type;
		}
		/*if(type == 0){
			total_move = current_move.x_move + current_move.y_move + current_move.z_move;
			current_move.x_period = current_move.x_period*current_move.x_move/total_move;
			current_move.y_period = current_move.y_period*current_move.y_move/total_move;
			current_move.z_period = current_move.z_period*current_move.z_move/total_move;
			current_move.x_current_period = current_move.x_period;
			current_move.y_current_period = current_move.y_period;
			current_move.z_current_period = current_move.z_period;
			printf("X: %lu, Y: %lu, Z: %lu\n", current_move.x_period, current_move.y_period, current_move.z_period);
		} else {
			
		}*/
	}
}

void go_to(char move_string[200]){
	char cmd;
	char x_axis, y_axis;
	unsigned long int x_pos, y_pos;
	unsigned int x_period, y_period;
	sscanf(move_string, "%c%c%lu.%c%lu.%u.%u", &cmd, &x_axis, &x_pos, &y_axis, &y_pos, &x_period, &y_period);
	printf("Going To: Axis: %c, Pos: %lu, Speed: %d, Axis: %c, Pos: %lu, Speed: %d\n", x_axis, x_pos, x_period, y_axis, y_pos, y_period);
	if(x_axis == 'c'){
		end_position.x = w_size.x/2;
	} else {
		end_position.x = x_pos;
	}
	if(y_axis == 'c'){
		end_position.y = w_size.y/2;
	} else {
		end_position.y = y_pos;
	}
	if(x_period > 0){
		current_move.x_period = x_period;
	} else {
		current_move.x_period = NORMAL_SPEED;
	}
	current_move.x_current_period = START_SPEED;
	if(y_period > 0){
		current_move.y_period = y_period;
	} else {
		current_move.y_period = NORMAL_SPEED;
	}
	current_move.y_current_period = START_SPEED;
	if(end_position.x > current_position.x){
		current_move.x_dir = MOTOR_MOVE_R;
		current_move.x_move = end_position.x - current_position.x;
		current_move.x_move_count = current_move.x_move;
	} else {
		current_move.x_dir = MOTOR_MOVE_L;
		current_move.x_move = current_position.x - end_position.x;
		current_move.x_move_count = current_move.x_move;
	}
	if(end_position.y > current_position.y){
		current_move.y_dir = MOTOR_MOVE_A;
		current_move.y_move = end_position.y - current_position.y;
		current_move.y_move_count = current_move.y_move;
	} else {
		current_move.y_dir = MOTOR_MOVE_T;
		current_move.y_move = current_position.y - end_position.y;
		current_move.y_move_count = current_move.y_move;
	}
	current_move.x_act = 1;
	current_move.y_act = 1;
}

void process_calibration(void){
	switch(calibration_state){
		case 0 :
			printf("Cal 0 Moving Left and Towards\n");
			current_move.x_dir = MOTOR_MOVE_L;
			current_move.x_period = NORMAL_SPEED;
			current_move.x_act = 1;
			current_move.y_dir = MOTOR_MOVE_T;
			current_move.y_period = NORMAL_SPEED;
			current_move.y_act = 1;
			current_position.x = 200000;
			current_move.x_move = 200000;
			end_position.x = 0;
			current_position.y = 200000;
			current_move.y_move = 200000;
			end_position.y = 0;
			calibration_state = 1;
			break;
		case 1 :
			if(current_position.x_l_stop == ENDSTOP_HIT){
				if(current_move.x_act) printf("Hit Left End\n");
				current_position.x = 0;
				end_position.x = 0;
				current_move.x_act = 0;
				current_move.x_period = 0;
			}
			if(current_position.y_c_stop == ENDSTOP_HIT){
				if(current_move.y_act) printf("Hit Close End\n");
				current_position.y = 0;
				end_position.y = 0;
				current_move.y_act = 0;
				current_move.y_period = 0;
			}
			if((current_move.x_act == 0) && (current_move.y_act == 0)){
				printf("Moving Right and Away\n");
				current_move.x_dir = MOTOR_MOVE_R;
				current_move.x_period = NORMAL_SPEED;
				current_move.x_act = 1;
				current_move.y_dir = MOTOR_MOVE_A;
				current_move.y_period = NORMAL_SPEED;
				current_move.y_act = 1;
				end_position.x = 200000;
				end_position.y = 200000;
				current_move.x_move = 200000;
				current_move.y_move = 200000;
				calibration_state = 2;
			}
			break;
		case 2 :
			if(current_position.x_r_stop == ENDSTOP_HIT){
				if(current_move.x_act) printf("Hit Right End\n");
				w_size.x = current_position.x;
				w_size.x_calibrated = 1;
				end_position.x = current_position.x;
				current_move.x_act = 0;
				current_move.x_period = 0;
			}
			if(current_position.y_f_stop == ENDSTOP_HIT){
				if(current_move.y_act) printf("Hit Far End\n");
				w_size.y = current_position.y;
				w_size.y_calibrated = 1;
				current_move.y_act = 0;
				end_position.y = current_position.y;
				current_move.y_period = 0;
			}
			if((current_move.x_act == 0) && (current_move.y_act == 0)){
				printf("Finished - Move to Left and Towards to Middle\n");
				system_calibrated = 1;
				current_move.x_period = NORMAL_SPEED;
				current_move.y_period = NORMAL_SPEED;
				current_move.x_dir = MOTOR_MOVE_L;
				current_move.y_dir = MOTOR_MOVE_T;
				end_position.x = w_size.x/2;
				current_move.x_act = 1;
				end_position.y = w_size.y/2;
				current_move.y_act = 1;
				calibration_state = 3;
				current_move.x_move = current_position.x - end_position.x;
				current_move.y_move = current_position.y - end_position.y;
			}
			break;
		case 3 :
			if(current_position.x <= (w_size.x/2)){
				current_move.x_period = 0;
				current_move.x_act = 0;
			}
			if(current_position.y <= (w_size.y/2)){
				current_move.y_period = 0;
				current_move.y_act = 0;
			}
			if((current_move.x_act == 0) && (current_move.y_act == 0)){
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

void print_system_info(void){
	printf("\n\n<<------ SYSTEM INFO ------>>\n");
	printf("\n<<-- Motor Enable: %d, System Calibration: %d-->\n", motors_enabled, system_calibrated);
	printf("\n<<-- Cur X Pos: %lu, End X Pos: %lu, Period: %u, Speed: %u, Active: %u -->\n", current_position.x, end_position.x, current_move.x_period, current_move.x_current_period, current_move.x_act);
	printf("\n<<-- Cur Y Pos: %lu, End Y Pos: %lu, Period: %u, Speed: %u, Active: %u -->\n", current_position.y, end_position.y, current_move.y_period, current_move.y_current_period, current_move.y_act);
	printf("\n<<-- Cur Z Pos: %lu, End Z Pos: %lu, Period: %u, Speed: %u, Active: %u -->\n", current_position.z, end_position.z, current_move.z_period, current_move.z_current_period, current_move.z_act);
	printf("\n<<-- Work Area Size X: %lu, Y: %lu-->\n", w_size.x, w_size.y);
}

