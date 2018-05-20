
#include <stdio.h>
#include <stdlib.h>

#include "socket.h"
#include "interface_gpio.h"

void system_control(int control_socket);

extern char system_command[MAX_FUNCTION_STRING];
