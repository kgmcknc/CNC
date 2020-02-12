
#ifndef SRC_INTERFACE_GPIO_H_
#define SRC_INTERFACE_GPIO_H_

#include <wiringPi.h>
#include <wiringSerial.h>
#include "cnc_pin_map.h"

void init_interface_gpio(void);
void disable_interface_gpio(void);
void enable_motors(void);
void disable_motors(void);

#endif /* SRC_INTERFACE_GPIO_H_ */
