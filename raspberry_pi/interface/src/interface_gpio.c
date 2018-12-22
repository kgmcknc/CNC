#include "main.h"

void init_interface_gpio(void){
    printf("Initializing System and Setting GPIO\n");
    wiringPiSetup();
    
    pinMode(SILABS_POWER, OUTPUT);
    pinMode(MOTOR_POWER, OUTPUT);
    pinMode(SPI_RX_READY, OUTPUT);
    pinMode(SPI_TX_READY, INPUT);
    
    digitalWrite(SILABS_POWER, POWER_OFF);
    digitalWrite(MOTOR_POWER, POWER_OFF);
    delay(50);
    digitalWrite(SILABS_POWER, POWER_ON);
    delay(50);
}

void disable_interface_gpio(void){
	printf("Setting GPIO To Safe Default For Shutdown\n");
	close(spi_fd);
	
	digitalWrite(MOTOR_POWER, POWER_OFF);
    digitalWrite(SILABS_POWER, POWER_OFF);
    
    pinMode(MOTOR_POWER, INPUT);
    pinMode(SILABS_POWER, INPUT);
    pinMode(SPI_RX_READY, INPUT);
    pinMode(SPI_TX_READY, INPUT);

    printf("GPIO Safe... Quitting System\n");
}

void enable_motors(void){
	digitalWrite(MOTOR_POWER, POWER_ON);
}

void disable_motors(void){
	digitalWrite(MOTOR_POWER, POWER_OFF);
}
