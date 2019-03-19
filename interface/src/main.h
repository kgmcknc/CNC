
#ifndef SRC_MAIN_H_
#define SRC_MAIN_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "socket.h"
#include "common_spi.h"
#include "interface_gpio.h"
#include "interface_functions.h"
#include "instructions.h"

#define MAX_USER_COMMAND 4096

struct interface_struct {
    enum CNC_INTERFACE_STATE state;
    enum CNC_MACHINE_STATE machine_state;
    struct gcode_program_struct program;
	uint8_t motors_enabled;
	FILE* gcode_fp;
    char file_name[MAX_USER_COMMAND];
    char user_command[MAX_USER_COMMAND];
    char user_input[MAX_USER_COMMAND];
    char cnc_read_data[MAX_SPI_TRANSFER];
    int32_t cnc_read_length;
    uint8_t read_in_progress;
    uint8_t read_complete;
    char cnc_write_data[MAX_SPI_TRANSFER];
    int32_t cnc_write_length;
    uint8_t write_in_progress;
    uint8_t write_complete;
    uint16_t command_counter = 0;
    uint8_t user_command_set = 0;
    uint8_t user_command_finished = 0;
    struct cnc_instruction_struct user_instruction;
};

void init_interface_struct(struct interface_struct* interface);
uint8_t interface_main(struct interface_struct* interface);
void system_control(int control_socket);
void system_shutdown(int sig);
void system_control_shutdown(int sig);
void print_interface_menu(void);

extern uint8_t quit_system;
extern int32_t spi_fd;

#endif /* SRC_MAIN_H_ */