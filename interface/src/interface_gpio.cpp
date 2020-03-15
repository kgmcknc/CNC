#include "main.h"

void init_interface_gpio(void){
    printf("Initializing System and Setting GPIO\n");
    wiringPiSetup();
    
    pinMode(MAIN_POWER_PIN, OUTPUT);
    pinMode(MOTOR_POWER_PIN, OUTPUT);
    pinMode(MICRO_RESET_PIN, OUTPUT);

    digitalWrite(MAIN_POWER_PIN, MAIN_POWER_OFF);
    digitalWrite(MOTOR_POWER_PIN, MOTOR_POWER_OFF);
    digitalWrite(MICRO_RESET_PIN, MICRO_RESET_ON);
    usleep(50000);
    digitalWrite(MAIN_POWER_PIN, MAIN_POWER_ON);
    digitalWrite(MICRO_RESET_PIN, MICRO_RESET_OFF);
    usleep(50000);
}

void disable_interface_gpio(void){
    printf("Setting GPIO To Safe Default For Shutdown\n");
    close(serial_fd);

    digitalWrite(MOTOR_POWER_PIN, MOTOR_POWER_OFF);
    sleep(4);
    digitalWrite(MAIN_POWER_PIN, MAIN_POWER_OFF);
    //digitalWrite(MICRO_RESET_PIN, MICRO_RESET_ON);
    
    pinMode(MAIN_POWER_PIN, INPUT);
    pinMode(MOTOR_POWER_PIN, INPUT);
    //pinMode(MICRO_RESET_PIN, INPUT);

    printf("GPIO Safe... Quitting System\n");
}

void enable_motors(void){
	digitalWrite(MOTOR_POWER_PIN, MOTOR_POWER_ON);
}

void disable_motors(void){
	digitalWrite(MOTOR_POWER_PIN, MOTOR_POWER_OFF);
}
