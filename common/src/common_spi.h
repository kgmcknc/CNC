/*
 * common_spi.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#ifndef SRC_COMMON_SPI_H_
#define SRC_COMMON_SPI_H_

#define MAX_SPI_LENGTH 256

#define MASTER_INIT_LENGTH 6
#define SLAVE_INIT_LENGTH 6
#define SPI_MASTER_INIT_STRING "PiHere"
#define SPI_SLAVE_INIT_STRING "UcHere"
#define OPCODE_LENGTH 2
#define SIZE_LENGTH 4
#define IDLE_LENGTH (OPCODE_LENGTH + SIZE_LENGTH)
#define PRINT_LENGTH (MAX_SPI_LENGTH - IDLE_LENGTH)
#define GENERAL_LENGTH 8
#define INSTRUCTION_LENGTH (MAX_SPI_LENGTH - IDLE_LENGTH)
#define STATUS_LENGTH (MAX_SPI_LENGTH - IDLE_LENGTH)

enum spi_opcodes {
	idle,
	init,
	reconnect_spi,
	loopback_test,
	read_version,
	flash_firmware,
	start_cnc_program,
	end_cnc_program,
	new_cnc_instruction,
	get_cnc_status,
	new_cnc_print,
	disable_route,
	enable_route
};


#endif /* SRC_COMMON_SPI_H_ */
