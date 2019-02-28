/*
 * main.h
 *
 *  Created on: Aug 21, 2018
 *      Author: Kyle
 */

#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include "stdint.h"
#include "cnc_spi.h"
#include "cnc_functions.h"
#include "cnc_motors.h"

#define SPI_SLAVE

#define MAJOR_REVISION 0x0000
#define MINOR_REVISION 0x0000

void system_init(struct spi_struct* spi, struct cnc_motor_list_struct* cnc_motors);
void variable_init(struct cnc_state_struct* cnc, struct spi_struct* spi, struct cnc_motor_list_struct* cnc_motors);
void init_clocks(void);

#endif /* SRC_MAIN_H_ */
