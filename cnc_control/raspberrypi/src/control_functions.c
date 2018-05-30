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
	unsigned long int move_count;
	unsigned int move_speed;
	sscanf(move_string, "%c%c.%lu.%u", &cmd, &dir, &move_count, &move_speed);
	printf("Moving: %c, Number: %lu, at Speed: %u\n", dir, move_count, move_speed);
	if(dir == '0'){
		printf("Stopping Movement\n");
		current_move.x_period_count = 0;
		current_move.x_act = 0;
		current_move.y_period_count = 0;
		current_move.y_act = 0;
		current_move.z_period_count = 0;
		current_move.z_act = 0;
	} else {
		if(dir == 'l'){
			if(move_speed > 0){
				next_move.x_period_count = move_speed;
			} else {
				next_move.x_period_count = NORMAL_SPEED;
			}
			end_position.x = current_position.x - move_count;
			next_move.x_dir = MOTOR_MOVE_L;
			next_move.x_act = 1;
			printf("Moving Left\n");
		}
		if(dir == 'r'){
			if(move_speed > 0){
				next_move.x_period_count = move_speed;
			} else {
				next_move.x_period_count = NORMAL_SPEED;
			}
			end_position.x = current_position.x + move_count;
			next_move.x_dir = MOTOR_MOVE_R;
			next_move.x_act = 1;
			printf("Moving Right\n");
		}
		if(dir == 't'){
			if(move_speed > 0){
				next_move.y_period_count = move_speed;
			} else {
				next_move.y_period_count = NORMAL_SPEED;
			}
			end_position.y = current_position.y - move_count;
			next_move.y_dir = MOTOR_MOVE_T;
			next_move.y_act = 1;
			printf("Moving Towards\n");
		}
		if(dir == 'a'){
			if(move_speed > 0){
				next_move.y_period_count = move_speed;
			} else {
				next_move.y_period_count = NORMAL_SPEED;
			}
			end_position.y = current_position.y + move_count;
			next_move.y_dir = MOTOR_MOVE_A;
			next_move.y_act = 1;
			printf("Moving Away\n");
		}
		if(dir == 'u'){
			if(move_speed > 0){
				next_move.z_period_count = move_speed;
			} else {
				next_move.z_period_count = NORMAL_SPEED;
			}
			end_position.z = current_position.z + move_count;
			next_move.z_dir = MOTOR_MOVE_U;
			next_move.z_act = 1;
			printf("Moving Up\n");
		}
		if(dir == 'd'){
			if(move_speed > 0){
				next_move.z_period_count = move_speed;
			} else {
				next_move.z_period_count = NORMAL_SPEED;
			}
			end_position.z = current_position.z - move_count;
			next_move.z_dir = MOTOR_MOVE_D;
			next_move.z_act = 1;
			printf("Moving Down\n");
		}
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
		next_move.x_period_count = x_period;
	} else {
		next_move.x_period_count = NORMAL_SPEED;
	}
	if(y_period > 0){
		next_move.y_period_count = y_period;
	} else {
		next_move.y_period_count = NORMAL_SPEED;
	}
	if(end_position.x > current_position.x){
		next_move.x_dir = MOTOR_MOVE_R;
	} else {
		next_move.x_dir = MOTOR_MOVE_L;
	}
	if(end_position.y > current_position.y){
		next_move.y_dir = MOTOR_MOVE_A;
	} else {
		next_move.y_dir = MOTOR_MOVE_T;
	}
	next_move.x_act = 1;
	next_move.y_act = 1;
}

void process_calibration(void){
	switch(calibration_state){
		case 0 :
			printf("Cal 0 Moving Left and Towards\n");
			next_move.x_dir = MOTOR_MOVE_L;
			next_move.x_period_count = NORMAL_SPEED;
			next_move.x_act = 1;
			next_move.y_dir = MOTOR_MOVE_T;
			next_move.y_period_count = NORMAL_SPEED;
			next_move.y_act = 1;
			current_position.x = 200000;
			end_position.x = 0;
			current_position.y = 200000;
			end_position.y = 0;
			calibration_state = 1;
			break;
		case 1 :
			if(current_position.x_l_stop == ENDSTOP_HIT){
				if(current_move.x_act) printf("Hit Left End\n");
				current_position.x = 0;
				end_position.x = 0;
				current_move.x_act = 0;
				next_move.x_act = 0;
				current_move.x_period_count = 0;
			}
			if(current_position.y_c_stop == ENDSTOP_HIT){
				if(current_move.y_act) printf("Hit Close End\n");
				current_position.y = 0;
				end_position.y = 0;
				current_move.y_act = 0;
				next_move.y_act = 0;
				current_move.y_period_count = 0;
			}
			if((current_move.x_act == 0) && (current_move.y_act == 0)){
				printf("Moving Right and Away\n");
				next_move.x_dir = MOTOR_MOVE_R;
				next_move.x_period_count = NORMAL_SPEED;
				next_move.x_act = 1;
				next_move.y_dir = MOTOR_MOVE_A;
				next_move.y_period_count = NORMAL_SPEED;
				next_move.y_act = 1;
				end_position.x = 200000;
				end_position.y = 200000;
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
				next_move.x_act = 0;
				current_move.x_period_count = 0;
			}
			if(current_position.y_f_stop == ENDSTOP_HIT){
				if(current_move.y_act) printf("Hit Far End\n");
				w_size.y = current_position.y;
				w_size.y_calibrated = 1;
				current_move.y_act = 0;
				end_position.y = current_position.y;
				next_move.y_act = 0;
				current_move.y_period_count = 0;
			}
			if((current_move.x_act == 0) && (current_move.y_act == 0)){
				printf("Finished - Move to Left and Towards to Middle\n");
				system_calibrated = 1;
				next_move.x_period_count = NORMAL_SPEED;
				next_move.y_period_count = NORMAL_SPEED;
				next_move.x_dir = MOTOR_MOVE_L;
				next_move.y_dir = MOTOR_MOVE_T;
				end_position.x = w_size.x/2;
				next_move.x_act = 1;
				end_position.y = w_size.y/2;
				next_move.y_act = 1;
				calibration_state = 3;
			}
			break;
		case 3 :
			if(current_position.x <= (w_size.x/2)){
				current_move.x_period_count = 0;
				current_move.x_act = 0;
				next_move.x_act = 0;
			}
			if(current_position.y <= (w_size.y/2)){
				current_move.y_period_count = 0;
				current_move.y_act = 0;
				next_move.y_act = 0;
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
	printf("\n<<-- Cur X Pos: %lu, End X Pos: %lu, Speed: %u, Active: %u -->\n", current_position.x, end_position.x, current_move.x_period_count, current_move.x_act);
	printf("\n<<-- Cur Y Pos: %lu, End Y Pos: %lu, Speed: %u, Active: %u -->\n", current_position.y, end_position.y, current_move.y_period_count, current_move.y_act);
	printf("\n<<-- Cur Z Pos: %lu, End Z Pos: %lu, Speed: %u, Active: %u -->\n", current_position.z, end_position.z, current_move.z_period_count, current_move.z_act);
	printf("\n<<-- Work Area Size X: %lu, Y: %lu-->\n", w_size.x, w_size.y);
}

