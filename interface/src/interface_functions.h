
#ifndef SRC_INTERFACE_FUNCTIONS_H_
#define SRC_INTERFACE_FUNCTIONS_H_

#include "interface_serial.h"
#include "common_serial.h"
#include "gcode.h"

enum CNC_INTERFACE_STATE {
   FORK_INPUT = 0, CREATE_SOCKET, CREATE_CHILD_SOCKET,
   INTERFACE_INIT, ENABLE_GPIO, CONNECT_MICRO,
   INTERFACE_RUNNING, DISABLE_GPIO, CHILD_PROCESS, EXIT_INTERFACE
};

enum CNC_MACHINE_STATE {
   MACHINE_IDLE = 0, PROCESS_INPUT, POWER_MOTORS_ON, POWER_MOTORS_OFF, CONFIGURE_INTERFACE, SEND_CONFIG,
   GET_PROGRAM, OPEN_PROGRAM, RUN_PROGRAM, SEND_INSTRUCTION, SEND_INSTANT_INSTRUCTION, UPDATE_FIRMARE, GET_STATUS, GET_VERSION, USER_CONTROL
};

enum CNC_CONFIG_STATE { 
   CONFIG_ENABLE_POWER = 0, ENABLE_ALL_MOTORS, MAX_ZL_ZR_FAST, OFFSET_ZL_R_SLOW, MAX_ZL_ZR_SLOW, SET_ZERO_ZL_ZR,
   MIN_X_Y_FAST, OFFSET_X_Y, MIN_X_Y_SLOW, SET_ZERO_X_Y, MANUAL_ZL_ZR_SAFE, MANUAL_ZL_ZERO, MAX_X_FAST, OFFSET_X, MAX_X_SLOW,
   MANUAL_ZR_ZERO, SET_Z_ZERO, MAX_Y_FAST, OFFSET_Y, MAX_Y_SLOW, SAVE_AXIS_LENGTHS, LEVEL_Y, MIN_X, LEVEL_X,
   CENTER_ZL_ZR_X_Y, DISABLE_ALL_MOTORS, CONFIG_DISABLE_POWER, CONFIG_ERROR, CONFIGURED
};

enum CNC_LEVEL_BED {
   LEVEL_ENABLE_ALL_MOTORS = 0
};

void handle_cnc_state(struct interface_struct* interface);
void process_request(struct interface_struct* interface);
void configure_machine(struct interface_struct* interface);
void receive_user_input(struct interface_struct* interface);
void receive_user_control(struct interface_struct* interface);
void handle_input(struct interface_struct* interface);
void clear_user_command(struct interface_struct* interface);
void receive_spi_string(uint8_t string_length);
void length_to_string(uint8_t byte_length, char *length_string);
void update_firmware(struct interface_struct* interface);
int open_gcode(struct interface_struct* interface);
void load_config_file(struct cnc_config_struct* config);
void save_config_file(struct cnc_config_struct* config);
void process_instruction(struct interface_struct* interface);
void display_cnc_print(uint8_t* print_string, uint32_t length);

#endif /* SRC_INTERFACE_FUNCTIONS_H_ */
