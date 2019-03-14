
#ifndef SRC_INTERFACE_FUNCTIONS_H_
#define SRC_INTERFACE_FUNCTIONS_H_

#include "interface_spi.h"
#include "common_spi.h"
#include "gcode.h"

enum CNC_INTERFACE_STATE {
	FORK_INPUT = 0, CREATE_SOCKET, CREATE_CHILD_SOCKET,
	INTERFACE_INIT, ENABLE_GPIO, CONNECT_MICRO,
	INTERFACE_RUNNING, DISABLE_GPIO, CHILD_PROCESS, EXIT_INTERFACE
};

enum CNC_MACHINE_STATE {
	MACHINE_IDLE = 0, PROCESS_SPI, PROCESS_INPUT, POWER_MOTORS_ON, POWER_MOTORS_OFF,
	GET_PROGRAM, OPEN_PROGRAM, RUN_PROGRAM, UPDATE_FIRMARE, GET_STATUS, GET_VERSION, USER_CONTROL
};

void handle_cnc_state(struct interface_struct* interface);
uint8_t check_spi(struct interface_struct* interface);
void process_spi_request(struct interface_struct* interface);
void receive_user_input(struct interface_struct* interface);
void receive_user_control(struct interface_struct* interface);
void handle_input(struct interface_struct* interface);
void interface_functions(uint8_t command_ready, char* system_command, struct spi_struct* spi, struct cnc_state* cnc);
void send_spi_string(char* spi_string, uint8_t string_length);
void receive_spi_string(uint8_t string_length);
void length_to_string(uint8_t byte_length, char *length_string);
void opcode_to_string(enum spi_opcodes opcode, char *opcode_string); 
void update_si_firmware(struct interface_struct* interface);
int open_gcode(struct interface_struct* cnc, char gcode_file[200]);

#endif /* SRC_INTERFACE_FUNCTIONS_H_ */
