#include <wiringPi.h>
#include <wiringPiSPI.h>
#include "cnc_pin_map.h"

#define SPI_CHANNEL 0
#define SPI_SPEED 10000000

void init_interface_gpio(void);
void disable_interface_gpio(void);
