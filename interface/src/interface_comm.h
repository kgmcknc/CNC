
#ifndef SRC_INTERFACE_COMM_H_
#define SRC_INTERFACE_COMM_H_

#include "stdint.h"
#include "common_serial.h"
#include "wiringSerial.h"

extern int serial_fd;

//void handle_interface_spi(struct spi_struct* spi);
//void handle_spi_reset(struct spi_struct* spi);
//void wait_for_connect(struct spi_struct* spi);
//void send_connected(struct spi_struct* spi);
//void set_spi_reset();
//void clear_spi_reset();
//uint8_t check_spi_request();
//void check_idle(struct spi_struct* spi);
//int32_t string_to_int32(char length_string[MAX_SPI_LENGTH], uint8_t byte_length);
//enum spi_opcodes parse_opcode(char opcode_string[MAX_SPI_LENGTH]);
void handle_opcode(struct spi_struct* spi, enum spi_opcodes new_opcode);
void handle_request(struct spi_struct* spi);
void display_cnc_print(uint8_t* print_string, uint32_t length);
void send_cnc_instruction(struct spi_struct* spi);
void send_cnc_opcode(struct spi_struct* spi);
void request_reconnect(struct spi_struct* spi);
void request_cnc_status(struct spi_struct* spi);
//void set_write_opcode(struct spi_struct* spi, enum spi_opcodes instruction, uint8_t byte_length);
void set_read_request(struct spi_struct* spi);
void clear_read_request(struct spi_struct* spi);
void set_write_request(struct spi_struct* spi);
void clear_write_request(struct spi_struct* spi);


#endif /* SRC_INTERFACE_COMM_H_ */