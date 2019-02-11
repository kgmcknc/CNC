/*
 * cnc_spi.h
 *
 *  Created on: Aug 17, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_SPI_H_
#define SRC_CNC_SPI_H_

#include "spidrv.h"
#include "cnc_functions.h"
#include "revision.h"
#include "common_spi.h"
#include "util.h"

#define SPI_CLK_LOC       10
#define SPI_CS_LOC        29
#define SPI_MOSI_LOC      10
#define SPI_MISO_LOC      10

typedef enum {
	spi_inactive,
	spi_initialized,
	spi_connected,
	spi_idle,
	spi_loopback_active,
	spi_handle_request,
	get_cnc_instruction,
	send_cnc_status,
	send_cnc_print,
	firmware_update
} cnc_spi_state;

struct cnc_spi_struct {
	cnc_spi_state state = spi_inactive;
	uint8_t write_pending = 0;
	uint8_t read_pending = 0;
	uint8_t read_ready = 0;
	uint8_t read_requested = 0;
	uint8_t write_finished = 0;
	uint8_t read_finished = 0;
	uint8_t opcode_sent = 0;
	uint32_t pending_length = 0;
	spi_opcodes pending_opcode = idle;
	char write_data[MAX_SPI_LENGTH] = {0};
	char read_data[MAX_SPI_LENGTH] = {0};
};

uint8_t init_spi(struct cnc_spi_struct* spi_struct);
void handle_cnc_spi(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
void wait_for_connect(struct cnc_spi_struct* spi_struct);
void send_connected(struct cnc_spi_struct* spi_struct);
void set_idle_read(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
void read_idle(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
spi_opcodes parse_opcode(char opcode_string[MAX_STRING_LENGTH]);
void handle_opcode(struct cnc_spi_struct* spi_struct, spi_opcodes new_opcode, struct cnc_state_struct* cnc);
void handle_request(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
void loop_back_data(struct cnc_spi_struct* spi_struct);
void wait_for_read(struct cnc_spi_struct* spi_struct);
void wait_for_write(struct cnc_spi_struct* spi_struct);
void set_read_request(struct cnc_spi_struct* spi_struct);
void check_read_request(struct cnc_spi_struct* spi_struct);
void clear_read_request(struct cnc_spi_struct* spi_struct);
void set_idle_read_request(struct cnc_spi_struct* spi_struct);
void set_write_request(struct cnc_spi_struct* spi_struct);
void clear_write_request(struct cnc_spi_struct* spi_struct);
void set_write_opcode(struct cnc_spi_struct* spi_struct, spi_opcodes instruction, uint8_t byte_length);
void receive_instruction(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
void parse_instruction(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
void parse_status(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
void parse_print(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
void send_status(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);
void send_print(struct cnc_spi_struct* spi_struct, struct cnc_state_struct* cnc);

#endif /* SRC_CNC_SPI_H_ */
