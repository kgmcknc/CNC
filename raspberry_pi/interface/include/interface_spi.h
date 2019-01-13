
#ifndef SRC_CNC_SPI_H_
#define SRC_CNC_SPI_H_

#include "stdint.h"

#define SPI_CHANNEL 0
#define SPI_SPEED 1000000

#define MAX_SPI_LENGTH 80

#define MASTER_INIT_LENGTH 6
#define SLAVE_INIT_LENGTH 6
#define SPI_MASTER_INIT_STRING "PiHere"
#define SPI_SLAVE_INIT_STRING "UcHere"
#define IDLE_LENGTH 6
#define PRINT_LENGTH 60
#define OPCODE_LENGTH 2
#define GENERAL_LENGTH 8
#define INSTRUCTION_LENGTH 48
#define STATUS_LENGTH 48

enum cnc_spi_state {
	spi_inactive,
	spi_initialized,
	spi_connected,
	spi_idle,
	spi_read_print,
	spi_get_status,
	spi_send_instruction,
	spi_send_opcode,
	spi_read_version,
	spi_request_reconnect,
	firmware_update
};

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

struct cnc_spi_struct {
	enum cnc_spi_state state;
	uint8_t connected;
	uint8_t write_pending;
	uint8_t read_pending;
	uint8_t write_finished;
	uint8_t read_finished;
	uint8_t updating_firmware;
	uint8_t pending_length;
	enum spi_opcodes pending_opcode;
	char write_data[MAX_SPI_LENGTH];
	char read_data[MAX_SPI_LENGTH];
};

void check_spi(void);
uint8_t init_spi(struct cnc_spi_struct* spi_struct);
void handle_spi(struct cnc_spi_struct* spi_struct);
void wait_for_connect(struct cnc_spi_struct* spi_struct);
void send_connected(struct cnc_spi_struct* spi_struct);
void set_m_ready(struct cnc_spi_struct* spi_struct);
void clear_m_ready(struct cnc_spi_struct* spi_struct);
uint8_t get_s_ready_state(struct cnc_spi_struct* spi_struct);
void check_idle(struct cnc_spi_struct* spi_struct);
int32_t string_to_int32(char length_string[MAX_SPI_LENGTH], uint8_t byte_length);
enum spi_opcodes parse_opcode(char opcode_string[MAX_SPI_LENGTH]);
void handle_opcode(struct cnc_spi_struct* spi_struct, enum spi_opcodes new_opcode);
void handle_request(struct cnc_spi_struct* spi_struct);
void display_cnc_print(struct cnc_spi_struct* spi_struct);
void send_cnc_instruction(struct cnc_spi_struct* spi_struct);
void send_cnc_opcode(struct cnc_spi_struct* spi_struct);
void request_reconnect(struct cnc_spi_struct* spi_struct);
void request_cnc_status(struct cnc_spi_struct* spi_struct);
void set_write_opcode(struct cnc_spi_struct* spi_struct, enum spi_opcodes instruction, uint8_t byte_length);
/*void set_read_request(struct cnc_spi_struct* spi_struct);
void clear_read_request(struct cnc_spi_struct* spi_struct);
void set_write_request(struct cnc_spi_struct* spi_struct);
void clear_write_request(struct cnc_spi_struct* spi_struct);
*/

#endif /* SRC_CNC_SPI_H_ */
