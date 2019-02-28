
#include "interface_spi.h"
#include "gcode.h"

enum CNC_INTERFACE_STATE {
	FORK_INPUT = 0, CREATE_SOCKET, CREATE_CHILD_SOCKET,
	INTERFACE_INIT, ENABLE_GPIO, CONNECT_MICRO,
	IDLE, HANDLE_SPI, PROCESS_INPUT, POWER_MOTORS_ON, POWER_MOTORS_OFF, 
	GET_PROGRAM, OPEN_PROGRAM, RUN_PROGRAM, SEND_TO_MICRO, UPDATE_FIRMARE, DISABLE_GPIO,
	CHILD_PROCESS, EXIT_INTERFACE
};

struct cnc_state {
	struct spi_struct* spi;
	enum CNC_INTERFACE_STATE state;
	struct gcode_program_struct program;
	uint8_t motors_enabled;
	FILE* gcode_fp;
};

void handle_input(struct cnc_state* cnc, char* system_command);
void interface_functions(uint8_t command_ready, char* system_command, struct spi_struct* spi, struct cnc_state* cnc);
void send_spi_string(char* spi_string, uint8_t string_length);
void receive_spi_string(uint8_t string_length);
void length_to_string(uint8_t byte_length, char *length_string);
void opcode_to_string(enum spi_opcodes opcode, char *opcode_string);
void update_si_firmware(struct spi_struct* spi);
int open_gcode(struct cnc_state* cnc, char gcode_file[200]);
