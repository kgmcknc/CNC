#include "main.h"

struct system_state system_status;

struct machine_position start_position, current_position, end_position;

struct motor_movement current_move, next_move;

struct work_area w_size;

int8_t system_calibrated = 0;
int8_t motors_enabled = 0;

void reset_motor_state(void){
	current_position.x = 100000;
	current_position.x_l_stop = 0;
	current_position.x_r_stop = 0;
	current_position.y = 100000;
	current_position.y_c_stop = 0;
	current_position.y_f_stop = 0;
	current_position.z = 100000;
	current_position.z_b_stop = 0;
	current_position.z_t_stop = 0;
	
	current_move.x_act = 0;
	current_move.x_period_count = 0;
	current_move.x_period = 0;
	current_move.x_current_period = 0;
	current_move.x_next_period = 0;
	current_move.x_move_count = 0;
	current_move.x_move = 0;
	current_move.x_dir = 0;
	current_move.x_arc = 0;
	
	current_move.y_act = 0;
	current_move.y_period_count = 0;
	current_move.y_move_count = 0;
	current_move.y_move = 0;
	current_move.y_period = 0;
	current_move.y_current_period = 0;
	current_move.y_next_period = 0;
	current_move.y_dir = 0;
	current_move.y_arc = 0;
	
	current_move.z_act = 0;
	current_move.z_period_count = 0;
	current_move.z_period = 0;
	current_move.z_current_period = 0;
	current_move.z_next_period = 0;
	current_move.z_move_count = 0;
	current_move.z_move = 0;
	current_move.z_dir = 0;
	current_move.z_arc = 0;
	current_move.line_count = 0;
}

void reset_area_state(void){
	system_calibrated = 0;
	w_size.x = 0;
	w_size.x_calibrated = 0;
	w_size.y = 0;
	w_size.y_calibrated = 0;
	w_size.z = 0;
	w_size.z_calibrated = 0;
}

void process_motion(void){

	check_x_endstop();
	check_y_endstop();
	check_z_endstop();
	
	if(current_move.x_act && (current_move.x_move_count == 0)){
		current_move.x_act = 0;
		current_move.x_move = 0;
		current_move.x_period_count = 0;
	}
	if(current_move.y_act && (current_move.y_move_count == 0)){
		current_move.y_act = 0;
		current_move.y_move = 0;
		current_move.y_period_count = 0;
	}
	if(current_move.z_act && (current_move.z_move_count == 0)){
		current_move.z_act = 0;
		current_move.z_move = 0;
		current_move.z_period_count = 0;
	}
}

void process_instruction(void){
	double next_x, next_y, x_arc_2, y_arc_2;
	double x_del, y_del, x_now, y_now, cur_x, cur_y;
	double move_count;
	if((current_move.x_act && current_move.x_arc) && (current_move.y_act && current_move.y_arc)){
		if(current_move.x_arc_cw){
			if(current_position.y < current_move.y_arc_center){
				// x dir left
				current_move.x_dir = MOTOR_MOVE_L;
			} else {
				if((current_position.y == current_move.y_arc_center) && (current_position.x > current_move.x_arc_center)){
					// x dir left
					current_move.x_dir = MOTOR_MOVE_L;
				} else {
					// x dir right
					current_move.x_dir = MOTOR_MOVE_R;
				}
			}
			if(current_position.x < current_move.x_arc_center){
				// y dir away
				current_move.y_dir = MOTOR_MOVE_A;
			} else {
				if((current_position.x == current_move.x_arc_center) && (current_position.y < current_move.y_arc_center)){
					// y dir away
					current_move.y_dir = MOTOR_MOVE_A;
				} else {
					// y dir towards
					current_move.y_dir = MOTOR_MOVE_T;
				}
			}
		} else {
			if(current_position.y > current_move.y_arc_center){
				// x dir left
				current_move.x_dir = MOTOR_MOVE_L;
			} else {
				if((current_position.y == current_move.y_arc_center) && (current_position.x > current_move.x_arc_center)){
					// x dir left
					current_move.x_dir = MOTOR_MOVE_L;
				} else {
					// x dir right
					current_move.x_dir = MOTOR_MOVE_R;
				}
			}
			if(current_position.x > current_move.x_arc_center){
				// y dir away
				current_move.y_dir = MOTOR_MOVE_A;
			} else {
				if((current_position.x == current_move.x_arc_center) && (current_position.y < current_move.y_arc_center)){
					// y dir away
					current_move.y_dir = MOTOR_MOVE_A;
				} else {
					// y dir towards
					current_move.y_dir = MOTOR_MOVE_T;
				}
			}
		}
		if(current_move.x_dir == MOTOR_MOVE_R){
			if(current_move.y_dir == MOTOR_MOVE_A){
				//printf("add to x and y and find new x and y period\n");
				x_arc_2 = current_move.x_arc_radius * current_move.x_arc_radius;
				y_arc_2 = current_move.y_arc_radius * current_move.y_arc_radius;
				y_now = current_position.y - current_move.y_arc_center;
				y_now = y_now * y_now * x_arc_2 / y_arc_2;
				cur_x = round(current_move.x_arc_center - sqrt(x_arc_2 - y_now));
				x_now = current_position.x - current_move.x_arc_center;
				x_now = x_now * x_now * y_arc_2 / x_arc_2;
				cur_y = round(sqrt(y_arc_2 - x_now) + current_move.y_arc_center);
				printf("Currentx: %f, x arc cnter: %f, x rad: %f, x rad 2: %f, y rad: %f, y rad 2: %f\n", current_position.x, current_move.x_arc_center, current_move.x_arc_radius, x_arc_2, current_move.y_arc_radius, y_arc_2);
				x_del = cur_x + 1 - current_move.x_arc_center;
				x_del = x_del * x_del * y_arc_2 / x_arc_2;
				next_y = sqrt(y_arc_2 - x_del) + current_move.y_arc_center;
				y_del = cur_y + 1 - current_move.y_arc_center;
				y_del = y_del * y_del * x_arc_2 / y_arc_2;
				next_x = current_move.x_arc_center - sqrt(x_arc_2 - y_del);
				printf("currentx: %f, currenty: %f, nextx: %f, nexty: %f\n", current_position.x, current_position.y, next_x, next_y);
			} else {
				printf("add to x and subtract from y and find new x and y period\n");
				x_arc_2 = current_move.x_arc_radius * current_move.x_arc_radius;
				y_arc_2 = current_move.y_arc_radius * current_move.y_arc_radius;
				y_del = current_position.y - 1 - current_move.y_arc_center;
				y_del = y_del * y_del * x_arc_2 / y_arc_2;
				next_x = sqrt(x_arc_2 - y_del) - current_move.x_arc_center;
				x_del = current_position.x + 1 - current_move.x_arc_center;
				x_del = x_del * x_del * y_arc_2 / x_arc_2;
				next_y = sqrt(y_arc_2 - x_del) - current_move.y_arc_center;
			}
		} else {
			if(current_move.y_dir == MOTOR_MOVE_A){
				printf("subtract from x and add to y and find new x and y period\n");
				x_arc_2 = current_move.x_arc_radius * current_move.x_arc_radius;
				y_arc_2 = current_move.y_arc_radius * current_move.y_arc_radius;
				y_del = current_position.y + 1 - current_move.y_arc_center;
				y_del = y_del * y_del * x_arc_2 / y_arc_2;
				next_x = sqrt(x_arc_2 - y_del) - current_move.x_arc_center;
				x_del = current_position.x - 1 - current_move.x_arc_center;
				x_del = x_del * x_del * y_arc_2 / x_arc_2;
				next_y = sqrt(y_arc_2 - x_del) - current_move.y_arc_center;
			} else {
				printf("subtract from x and subtract from y and find new x and y period\n");
				x_arc_2 = current_move.x_arc_radius * current_move.x_arc_radius;
				y_arc_2 = current_move.y_arc_radius * current_move.y_arc_radius;
				y_del = current_position.y - 1 - current_move.y_arc_center;
				y_del = y_del * y_del * x_arc_2 / y_arc_2;
				next_x = sqrt(x_arc_2 - y_del) - current_move.x_arc_center;
				x_del = current_position.x - 1 - current_move.x_arc_center;
				x_del = x_del * x_del * y_arc_2 / x_arc_2;
				next_y = sqrt(y_arc_2 - x_del) - current_move.y_arc_center;
			}
		}
		//printf("made it through new\n");
		move_count = sqrt((next_x-current_position.x)*(next_x-current_position.x)+(next_y-current_position.y)*(next_y-current_position.y));
		current_move.x_period = round(current_move.arc_period*move_count/abs(ceil((next_x-current_position.x))));
		current_move.y_period = round(current_move.arc_period*move_count/abs(ceil((next_y-current_position.y))));
		printf("curxper: %f, curyper: %f\n", current_move.x_period, current_move.y_period);
	}
}

void process_speed(void){
	/*if(current_move.x_act){
		if(current_move.x_move_count > (current_move.x_move - current_move.x_move_count)){
			// speeding up
			if(current_move.x_current_period > current_move.x_period){
				if((current_move.x_current_period - PERIOD_RAMP) >= current_move.x_period){
					current_move.x_next_period = current_move.x_current_period - PERIOD_RAMP;
				} else {
					current_move.x_next_period = current_move.x_period;
				}
			} else {
				current_move.x_next_period = current_move.x_period;
			}
		} else {
			// slowing down
			if(current_move.x_move_count <= ((START_SPEED - current_move.x_current_period)/PERIOD_RAMP)){
				if(current_move.x_current_period < START_SPEED){
					if((current_move.x_current_period + PERIOD_RAMP) <= START_SPEED){
						current_move.x_next_period = current_move.x_current_period + PERIOD_RAMP;
					} else {
						current_move.x_next_period = START_SPEED;
					}
				} else {
					current_move.x_next_period = START_SPEED;
				}
			} else {
				current_move.x_next_period = current_move.x_period;
			}
		}
	}
	if(current_move.y_act){
		if(current_move.y_move_count > (current_move.y_move - current_move.y_move_count)){
			// speeding up
			if(current_move.y_current_period > current_move.y_period){
				if((current_move.y_current_period - PERIOD_RAMP) >= current_move.y_period){
					current_move.y_next_period = current_move.y_current_period - PERIOD_RAMP;
				} else {
					current_move.y_next_period = current_move.y_period;
				}
			} else {
				current_move.y_next_period = current_move.y_period;
			}
		} else {
			// slowing down
			if(current_move.y_move_count <= ((START_SPEED - current_move.y_current_period)/PERIOD_RAMP)){
				if(current_move.y_current_period < START_SPEED){
					if((current_move.y_current_period + PERIOD_RAMP) <= START_SPEED){
						current_move.y_next_period = current_move.y_current_period + PERIOD_RAMP;
					} else {
						current_move.y_next_period = START_SPEED;
					}
				} else {
					current_move.y_next_period = START_SPEED;
				}
			} else {
				current_move.y_next_period = current_move.y_period;
			}
		}
	}
	if(current_move.z_act){
		if(current_move.z_move_count > (current_move.z_move - current_move.z_move_count)){
			// speeding up
			if(current_move.z_current_period > current_move.z_period){
				if((current_move.z_current_period - PERIOD_RAMP) >= current_move.z_period){
					current_move.z_next_period = current_move.z_current_period - PERIOD_RAMP;
				} else {
					current_move.z_next_period = current_move.z_period;
				}
			} else {
				current_move.z_next_period = current_move.z_period;
			}
		} else {
			// slowing down
			if(current_move.z_move_count <= ((START_SPEED - current_move.z_current_period)/PERIOD_RAMP)){
				if(current_move.z_current_period < START_SPEED){
					if((current_move.z_current_period + PERIOD_RAMP) <= START_SPEED){
						current_move.z_next_period = current_move.z_current_period + PERIOD_RAMP;
					} else {
						current_move.z_next_period = START_SPEED;
					}
				} else {
					current_move.z_next_period = START_SPEED;
				}
			} else {
				current_move.z_next_period = current_move.z_period;
			}
		}
	}*/
	current_move.x_current_period = current_move.x_period;
	current_move.x_next_period = current_move.x_period;
	current_move.y_current_period = current_move.y_period;
	current_move.y_next_period = current_move.y_period;
	current_move.z_current_period = current_move.z_period;
	current_move.z_next_period = current_move.z_period;
}

void process_motors(void){
	if(current_move.x_act){
		if(current_move.x_period_count == 0){
			system_status.get_new_move = 1;
			motor_x_high();
			current_move.x_high = 1;
			current_move.x_period_count = current_move.x_next_period;
			current_move.x_current_period = current_move.x_next_period;
			current_move.x_move_count = current_move.x_move_count - 1;
			if(current_move.x_dir == MOTOR_MOVE_R){
				current_position.x = current_position.x + 1;
			} else {
				current_position.x = current_position.x - 1;
			}
		} else {
			if(current_move.x_high){
				motor_x_low();
				if(current_move.x_dir == MOTOR_MOVE_R){
					motor_x_right();
				} else {
					motor_x_left();
				}
				current_move.x_high = 0;
			}
			current_move.x_period_count = current_move.x_period_count - 1;
		}
	}
	
	if(current_move.y_act){
		if(current_move.y_period_count == 0){
			motor_y_high();
			system_status.get_new_move = 1;
			current_move.y_high = 1;
			current_move.y_period_count = current_move.y_next_period;
			current_move.y_current_period = current_move.y_next_period;
			current_move.y_move_count = current_move.y_move_count - 1;
			if(current_move.y_dir == MOTOR_MOVE_A){
				current_position.y = current_position.y + 1;
			} else {
				current_position.y = current_position.y - 1;
			}
		} else {
			if(current_move.y_high){
				motor_y_low();
				if(current_move.y_dir == MOTOR_MOVE_A){
					motor_y_away();
				} else {
					motor_y_toward();
				}
				current_move.y_high = 0;
			}
			current_move.y_period_count = current_move.y_period_count - 1;
		}
	}
	
	if(current_move.z_act){
		if(current_move.z_period_count == 0){
			motor_zr_high();
			motor_zl_high();
			system_status.get_new_move = 1;
			current_move.z_high = 1;
			current_move.z_period_count = current_move.z_next_period;
			current_move.z_current_period = current_move.z_next_period;
			current_move.z_move_count = current_move.z_move_count - 1;
			if(current_move.z_dir == MOTOR_MOVE_U){
				current_position.z = current_position.z + 1;
			} else {
				current_position.z = current_position.z - 1;
			}
		} else {
			if(current_move.z_high){
				motor_zr_low();
				motor_zl_low();
				if(current_move.z_dir == MOTOR_MOVE_U){
					motor_z_up();
				} else {
					motor_z_down();
				}
				current_move.z_high = 0;
			}
			current_move.z_period_count = current_move.z_period_count - 1;
		}
	}
}

void check_x_endstop(void){
	if(digitalRead(L_END_STOP)){
		current_position.x_l_stop = ENDSTOP_HIT;
		if(current_move.x_dir == MOTOR_MOVE_L){
			if(current_move.x_act) printf("Hit Left Stop!\n");
			current_move.x_act = 0;
			current_move.x_move_count = 0;
		}
	} else {
		current_position.x_l_stop = ENDSTOP_OFF;
	}
	if(digitalRead(R_END_STOP)){
		current_position.x_r_stop = ENDSTOP_HIT;
		if(current_move.x_dir == MOTOR_MOVE_R){
			if(current_move.x_act) printf("Hit Right Stop!\n");
			current_move.x_act = 0;
			current_move.x_move_count = 0;
		}
	} else {
		current_position.x_r_stop = ENDSTOP_OFF;
	}
}

void check_y_endstop(void){
	if(digitalRead(F_END_STOP)){
		current_position.y_f_stop = ENDSTOP_HIT;
		if(current_move.y_dir == MOTOR_MOVE_A){
			if(current_move.y_act) printf("Hit Far Stop!\n");
			current_move.y_act = 0;
			current_move.y_move_count = 0;
		}
	} else {
		current_position.y_f_stop = ENDSTOP_OFF;
	}
	if(digitalRead(C_END_STOP)){
		current_position.y_c_stop = ENDSTOP_HIT;
		if(current_move.y_dir == MOTOR_MOVE_T){
			if(current_move.y_act) printf("Hit Close Stop!\n");
			current_move.y_act = 0;
			current_move.y_move_count = 0;
		}
	} else {
		current_position.y_c_stop = ENDSTOP_OFF;
	}
}

void check_z_endstop(void){
	current_position.z_t_stop = 0;
	current_position.z_b_stop = 0;
}

void init_control_gpio(void){
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

    pinMode(C_END_STOP, INPUT);
    pinMode(F_END_STOP, INPUT);
    pinMode(L_END_STOP, INPUT);
    pinMode(R_END_STOP, INPUT);

    digitalWrite(DRIVER_ENABLE, MOTOR_STANDBY);
    digitalWrite(DRIVER_SLEEP, MOTOR_RESET_ON);
    
    digitalWrite(L_MOTOR_STP, LOW);
    digitalWrite(L_MOTOR_DIR, MOTOR_MOVE_L);
    digitalWrite(L_MOTOR_MS2, LOW);
    digitalWrite(L_MOTOR_MS1, LOW);

    digitalWrite(F_MOTOR_STP, LOW);
    digitalWrite(F_MOTOR_DIR, MOTOR_MOVE_T);
    digitalWrite(F_MOTOR_MS2, LOW);
    digitalWrite(F_MOTOR_MS1, LOW);
	
	printf("GPIO Set To Safe Defaults\n");
	
    delay(2000);
    
}

void disable_control_gpio(void){
	printf("Setting GPIO To Safe Default For Shutdown\n");
	digitalWrite(DRIVER_POWER_RELAY, MOTOR_DRIVER_OFF);
	digitalWrite(DRIVER_ENABLE, MOTOR_STANDBY);
    digitalWrite(DRIVER_SLEEP, MOTOR_RESET_ON);
    
    digitalWrite(L_MOTOR_STP, LOW);
    digitalWrite(L_MOTOR_DIR, MOTOR_MOVE_R);
    digitalWrite(L_MOTOR_MS2, LOW);
    digitalWrite(L_MOTOR_MS1, LOW);

    digitalWrite(F_MOTOR_STP, LOW);
    digitalWrite(F_MOTOR_DIR, MOTOR_MOVE_A);
    digitalWrite(F_MOTOR_MS2, LOW);
    digitalWrite(F_MOTOR_MS1, LOW);
    printf("GPIO Safe... Quitting System\n");
}

void motor_x_left(void){
	digitalWrite(L_MOTOR_DIR, MOTOR_MOVE_L);
}
void motor_x_right(void){
	digitalWrite(L_MOTOR_DIR, MOTOR_MOVE_R);
}
void motor_y_toward(void){
	digitalWrite(F_MOTOR_DIR, MOTOR_MOVE_T);
}
void motor_y_away(void){
	digitalWrite(F_MOTOR_DIR, MOTOR_MOVE_A);
}
void motor_z_up(void){
	digitalWrite(UR_MOTOR_DIR, MOTOR_MOVE_U);
	digitalWrite(UL_MOTOR_DIR, MOTOR_MOVE_U);
}
void motor_z_down(void){
	digitalWrite(UR_MOTOR_DIR, MOTOR_MOVE_D);
	digitalWrite(UL_MOTOR_DIR, MOTOR_MOVE_D);
}

void motor_x_high(void){
	digitalWrite(L_MOTOR_STP, HIGH);
}
void motor_x_low(void){
	digitalWrite(L_MOTOR_STP, LOW);
}
void motor_y_high(void){
	digitalWrite(F_MOTOR_STP, HIGH);
}
void motor_y_low(void){
	digitalWrite(F_MOTOR_STP, LOW);
}
void motor_zr_high(void){
	digitalWrite(UR_MOTOR_STP, HIGH);
}
void motor_zr_low(void){
	digitalWrite(UR_MOTOR_STP, LOW);
}
void motor_zl_high(void){
	digitalWrite(UL_MOTOR_STP, HIGH);
}
void motor_zl_low(void){
	digitalWrite(UL_MOTOR_STP, LOW);
}
