#include "main.h"

int spi_fd;

void init_interface_gpio(void){
    printf("Initializing System and Setting GPIO\n");
    wiringPiSetup();
    
    spi_fd = wiringPiSPISetup(SPI_CHANNEL, SPI_SPEED);
    
    if(spi_fd < 0){
		printf("Spi error...\n");
	}
    
    delay(2000);
    
}

void disable_interface_gpio(void){
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
