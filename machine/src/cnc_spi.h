/*
 * cnc_spi.h
 *
 *  Created on: Aug 17, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_SPI_H_
#define SRC_CNC_SPI_H_

#define SPI_SLAVE

#include "spidrv.h"
#include "cnc_functions.h"
#include "revision.h"
#include "common_spi.h"
#include "util.h"

#define SPI_CLK_LOC       10
#define SPI_CS_LOC        29
#define SPI_MOSI_LOC      10
#define SPI_MISO_LOC      10

void init_spi_driver(void);

void init_spi(struct spi_struct* spi);
uint8_t check_spi(struct cnc_state_struct* cnc);
//void handle_spi_reset(struct spi_struct*spi);
void handle_cnc_spi(struct spi_struct* spi, struct cnc_state_struct* cnc);
//void send_connected(struct spi_struct* spi);
//void set_idle_read(struct spi_struct* spi, struct cnc_state_struct* cnc);
//void read_idle(struct spi_struct* spi, struct cnc_state_struct* cnc);
//spi_opcodes parse_opcode(char opcode_string[MAX_STRING_LENGTH]);
//void handle_opcode(struct spi_struct* spi, spi_opcodes new_opcode, struct cnc_state_struct* cnc);
//void handle_request(struct spi_struct* spi, struct cnc_state_struct* cnc);
//void loop_back_data(struct spi_struct* spi);
//void wait_for_read(struct spi_struct* spi);
//void wait_for_write(struct spi_struct* spi);
//void set_pending_transfer(struct spi_struct* spi);
//void clear_pending_transfer(struct spi_struct* spi);
//void set_request_ready(struct spi_struct* spi);
//void clear_request_ready(struct spi_struct* spi);
//void get_spi_reset(struct spi_struct* spi);
//void set_idle_read_request(struct spi_struct* spi);
//void clear_write_request(struct spi_struct* spi);
//void set_write_opcode(struct spi_struct* spi, spi_opcodes instruction, uint8_t byte_length);
void receive_instruction(struct spi_struct* spi, struct cnc_state_struct* cnc);
void parse_instruction(struct spi_struct* spi, struct cnc_state_struct* cnc);
void send_status(struct spi_struct* spi, struct cnc_state_struct* cnc);
void send_print(struct spi_struct* spi, struct cnc_state_struct* cnc);

#endif /* SRC_CNC_SPI_H_ */
