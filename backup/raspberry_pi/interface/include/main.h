
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "socket.h"
#include "interface_gpio.h"
#include "interface_functions.h"

void system_control(int control_socket);
void print_interface_menu(void);

extern char quit_system;
extern int32_t spi_fd;