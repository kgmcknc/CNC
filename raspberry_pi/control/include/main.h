
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "socket.h"
#include "control_gpio.h"
#include "control_functions.h"

void system_control(int control_socket);
void print_control_menu(void);
	
extern char system_command[MAX_FUNCTION_STRING];
