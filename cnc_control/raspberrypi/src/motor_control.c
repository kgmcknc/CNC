#include "main.h"


void init_cnc_gpio(void){
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

void disable_cnc_gpio(void){
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
