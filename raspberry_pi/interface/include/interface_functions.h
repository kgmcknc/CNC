
#include "interface_spi.h"

struct cnc_state {
	FILE* gcode_fp;
};

void interface_functions(uint8_t command_ready, uint8_t system_command[MAX_FUNCTION_STRING], struct cnc_spi_struct* spi_struct, struct cnc_state* cnc);
void send_spi_string(char spi_string[MAX_FUNCTION_STRING], uint8_t string_length);
void receive_spi_string(uint8_t string_length);
void length_to_string(uint8_t byte_length, char *length_string);
void opcode_to_string(enum spi_opcodes opcode, char *opcode_string);
void update_si_firmware(struct cnc_spi_struct* spi_struct);
void open_gcode(struct cnc_state* cnc, char gcode_file[200]);
