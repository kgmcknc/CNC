#include "main.h"

void init_interface_gpio(void){
    printf("Initializing System and Setting GPIO\n");
    wiringPiSetup();
    
    pinMode(SILABS_POWER, OUTPUT);
    pinMode(MOTOR_POWER, OUTPUT);
    pinMode(CNC_SPI_REQUEST, INPUT);
    pinMode(CNC_SPI_RESET, OUTPUT);
    
    digitalWrite(SILABS_POWER, POWER_OFF);
    digitalWrite(MOTOR_POWER, POWER_OFF);
    usleep(50000);
    digitalWrite(CNC_SPI_RESET, LOW);
    digitalWrite(SILABS_POWER, POWER_ON);
    usleep(50000);
}

void disable_interface_gpio(void){
	printf("Setting GPIO To Safe Default For Shutdown\n");
	close(spi_fd);
	
	digitalWrite(MOTOR_POWER, POWER_OFF);
    sleep(1);
    digitalWrite(SILABS_POWER, POWER_OFF);
    digitalWrite(CNC_SPI_RESET, LOW);
    
    pinMode(MOTOR_POWER, INPUT);
    pinMode(SILABS_POWER, INPUT);
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
