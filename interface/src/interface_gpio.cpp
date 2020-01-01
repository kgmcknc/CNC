#include "main.h"

void init_interface_gpio(void){
    printf("Initializing System and Setting GPIO\n");
    wiringPiSetup();
    
    pinMode(CNC_POWER, OUTPUT);
    pinMode(MICRO_POWER, OUTPUT);
    pinMode(MOTOR_POWER, OUTPUT);
    pinMode(CNC_SPI_REQUEST, INPUT);
    pinMode(CNC_SPI_RESET, OUTPUT);
    
    digitalWrite(CNC_POWER, POWER_OFF);
    digitalWrite(MICRO_POWER, POWER_OFF);
    digitalWrite(MOTOR_POWER, POWER_OFF);
    usleep(50000);
    digitalWrite(CNC_SPI_RESET, LOW);
    digitalWrite(CNC_POWER, POWER_ON);
    digitalWrite(MICRO_POWER, POWER_ON);
    usleep(50000);
}

void disable_interface_gpio(void){
    printf("Setting GPIO To Safe Default For Shutdown\n");
    close(spi_fd);

    digitalWrite(MOTOR_POWER, POWER_OFF);
    sleep(1);
    digitalWrite(MICRO_POWER, POWER_OFF);
    digitalWrite(CNC_POWER, POWER_OFF);
    digitalWrite(CNC_SPI_RESET, LOW);
    
    pinMode(CNC_POWER, INPUT);
    pinMode(MOTOR_POWER, INPUT);
    pinMode(MICRO_POWER, INPUT);
    pinMode(CNC_SPI_REQUEST, INPUT);
    pinMode(CNC_SPI_RESET, INPUT);

    printf("GPIO Safe... Quitting System\n");
}

void enable_motors(void){
	digitalWrite(MOTOR_POWER, POWER_ON);
}

void disable_motors(void){
	digitalWrite(MOTOR_POWER, POWER_OFF);
}
