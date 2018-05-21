#include "main.h"

struct m_position beg_pos, cur_pos, end_pos;

struct m_dynamics beg_dyn, cur_dyn, end_dyn;

struct w_area w_size;

void reset_motor_state(void){
	cur_pos.x = 0;
	cur_pos.x_l_stop = 0;
	cur_pos.x_r_stop = 0;
	cur_pos.y = 0;
	cur_pos.y_c_stop = 0;
	cur_pos.y_f_stop = 0;
	cur_pos.z = 0;
	cur_pos.z_b_stop = 0;
	cur_pos.z_t_stop = 0;
}

void reset_area_state(void){
	w_size.x = 0;
	w_size.x_calibrated = 0;
	w_size.y = 0;
	w_size.y_calibrated = 0;
	w_size.z = 0;
	w_size.z_calibrated = 0;
}

void process_motors(void){
	check_x_endstop();
	check_y_endstop();
	check_z_endstop();
	
	if(cur_dyn.x_total > 0){
		if(cur_dyn.x == 0){
			motor_x_low();
			cur_dyn.x = cur_dyn.x + 1;
		} else {
			if(cur_dyn.x >= cur_dyn.x_total){
				if(cur_dyn.x_dir == MOTOR_MOVE_L){
					cur_pos.x = cur_pos.x - 1;
					motor_x_left();
				} else {
					cur_pos.x = cur_pos.x + 1;
					motor_x_right();
				}
				motor_x_high();
				cur_dyn.x = 0;
			} else {
				cur_dyn.x = cur_dyn.x + 1;
			}
		}
	}
	
	if(cur_dyn.y_total > 0){
		if(cur_dyn.y == 0){
			motor_y_low();
			cur_dyn.y = cur_dyn.y + 1;
		} else {
			if(cur_dyn.y >= cur_dyn.y_total){
				if(cur_dyn.y_dir == MOTOR_MOVE_T){
					motor_y_toward();
					cur_pos.y = cur_pos.y - 1;
				} else {
					motor_y_away();
					cur_pos.y = cur_pos.y + 1;
				}
				motor_y_high();
				cur_dyn.y = 0;
			} else {
				cur_dyn.y = cur_dyn.y + 1;
			}
		}
	}
	
}

void check_x_endstop(void){
	if(digitalRead(L_END_STOP)){
		cur_pos.x_l_stop = ENDSTOP_HIT;
		if(cur_dyn.x_dir == MOTOR_MOVE_L){
			if(cur_dyn.x_total) printf("Hit Left Stop!\n");
			cur_dyn.x_total = 0;
		}
	} else {
		cur_pos.x_l_stop = ENDSTOP_OFF;
	}
	if(digitalRead(R_END_STOP)){
		cur_pos.x_r_stop = ENDSTOP_HIT;
		if(cur_dyn.x_dir == MOTOR_MOVE_R){
			if(cur_dyn.x_total) printf("Hit Right Stop!\n");
			cur_dyn.x_total = 0;
		}
	} else {
		cur_pos.x_r_stop = ENDSTOP_OFF;
	}
}

void check_y_endstop(void){
	if(digitalRead(F_END_STOP)){
		cur_pos.y_f_stop = ENDSTOP_HIT;
		if(cur_dyn.y_dir == MOTOR_MOVE_A){
			if(cur_dyn.y_total) printf("Hit Far Stop!\n");
			cur_dyn.y_total = 0;
		}
	} else {
		cur_pos.y_f_stop = ENDSTOP_OFF;
	}
	if(digitalRead(C_END_STOP)){
		cur_pos.y_c_stop = ENDSTOP_HIT;
		if(cur_dyn.y_dir == MOTOR_MOVE_T){
			if(cur_dyn.y_total) printf("Hit Close Stop!\n");
			cur_dyn.y_total = 0;
		}
	} else {
		cur_pos.y_c_stop = ENDSTOP_OFF;
	}
}

void check_z_endstop(void){
	cur_pos.z_t_stop = 1;
	cur_pos.z_b_stop = 1;
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
    digitalWrite(L_MOTOR_DIR, MOTOR_MOVE_R);
    digitalWrite(L_MOTOR_MS2, LOW);
    digitalWrite(L_MOTOR_MS1, LOW);

    digitalWrite(F_MOTOR_STP, LOW);
    digitalWrite(F_MOTOR_DIR, MOTOR_MOVE_A);
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
