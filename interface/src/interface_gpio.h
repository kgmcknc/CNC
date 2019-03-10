
#ifndef SRC_INTERFACE_GPIO_H_
#define SRC_INTERFACE_GPIO_H_

#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "cnc_pin_map.h"

#define SPI_M_SCLK  14
#define SPI_M_MOSI  12
#define SPI_M_MISO  13
#define SPI_M_CS    10

#define CS_0_ACTIVE 0
#define CS_0_INACTIVE 1

#define SILABS_POWER 29
#define MOTOR_POWER 28
#define CNC_SPI_REQUEST 5
#define CNC_SPI_RESET 6

#define POWER_OFF 1
#define POWER_ON 0

void init_interface_gpio(void);
void disable_interface_gpio(void);
void enable_motors(void);
void disable_motors(void);

#endif /* SRC_INTERFACE_GPIO_H_ */
