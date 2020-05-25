#include "main.h"
#include "interface_functions.h"
#include "stdio.h"
#include "common_cnc.h"
#include "interface_serial.h"
#include "gcode.h"
#include "math.h"

uint8_t print_set = 0;
enum CNC_CONFIG_STATE configure_stage = CONFIG_ENABLE_POWER;
uint8_t configure_processing = 0;

#define DEFAULT_SPEED ((cnc_double) 10.0)
#define DEFAULT_DISTANCE ((cnc_double) 20.0)

#define SLOW_SPEED ((cnc_double) 1.0)
#define FAST_SPEED ((cnc_double) 15.0)

cnc_double move_speed = FAST_SPEED;
cnc_double move_distance = DEFAULT_DISTANCE;
cnc_double z_move_distance = DEFAULT_DISTANCE;
cnc_double new_position = 0.0;

void handle_cnc_state(struct interface_struct* interface){
   cnc_serial.process();
   if(cnc_serial.rx_queue_fullness > 0){
      // received spi transaction
      // process that instead of user input
      interface->cnc_read_length = cnc_serial.receive(interface->cnc_read_data);
      process_request(interface);
   }
	switch(interface->machine_state){
		case MACHINE_IDLE : {
         // if not processing spi, process user input
         socket_handler(&interface->user_command_set, interface->user_input);
         receive_user_input(interface);
         if(interface->user_command_finished){
            interface->machine_state = PROCESS_INPUT;
         } else {
            if(interface->instruction_requested){
               interface->machine_state = SEND_INSTRUCTION;
            }
         }
			break;
		}
		case PROCESS_INPUT : {
			handle_input(interface);
			break;
		}
		case POWER_MOTORS_ON : {
         #ifdef SILABS
         interface->cnc_write_data[0] = (char) DISABLE_ROUTE;
         if(cnc_serial.send(1, interface->cnc_write_data) > 0){
            enable_motors();
            interface->machine_state = MACHINE_IDLE;
         } else {
            interface->machine_state = POWER_MOTORS_ON;
         }
         #else
         enable_motors();
         interface->machine_state = MACHINE_IDLE;
         #endif
			break;
		}
		case POWER_MOTORS_OFF : {
         disable_motors();
         #ifdef SILABS
         interface->cnc_write_data[0] = (char) ENABLE_ROUTE;
         if(cnc_serial.send(1, interface->cnc_write_data) > 0){
            interface->machine_state = MACHINE_IDLE;
         } else {
            interface->machine_state = POWER_MOTORS_OFF;
         }
         #else
         interface->machine_state = MACHINE_IDLE;
         #endif
			break;
		}
		case GET_STATUS : {
         interface->cnc_write_data[0] = (uint8_t) GET_CNC_STATUS;
         if(cnc_serial.send(1, interface->cnc_write_data) > 0){
            interface->machine_state = MACHINE_IDLE;
         } else {
            interface->machine_state = GET_STATUS;
         }
			break;
		}
		case GET_VERSION : {
         interface->cnc_write_data[0] = (uint8_t) GET_CNC_VERSION;
         if(cnc_serial.send(1, interface->cnc_write_data) > 0){
            interface->machine_state = MACHINE_IDLE;
         } else {
            interface->machine_state = GET_VERSION;
         }
			break;
		}
		/*case SEND_CONFIG : {
         interface->cnc_write_data[0] = (char) SET_CNC_CONFIG;
         interface->cnc_write_length = config_to_string(&interface->machine_config, (char*) &interface->cnc_write_data[1]);
         interface->cnc_write_length = interface->cnc_write_length + 1; // add one for opcode at first byte
         if(cnc_serial.send(interface->cnc_write_length, interface->cnc_write_data) > 0){
            printf("Sent Config Data\n");
            save_config_file(&interface->machine_config);
            interface->machine_configured = 1;
            interface->machine_state = MACHINE_IDLE;
         } else {
            interface->machine_state = SEND_CONFIG;
         }
			break;
		}*/
		case USER_CONTROL : {
         if(interface->user_instruction.instruction_valid){
            interface->cnc_write_data[0] = (uint8_t) NEW_CNC_INSTRUCTION;
            interface->cnc_write_length = instruction_to_string(&interface->user_instruction, &interface->cnc_write_data[1]);
            interface->cnc_write_length = interface->cnc_write_length + 1;
            if(cnc_serial.send(interface->cnc_write_length, interface->cnc_write_data) > 0){
               interface->user_instruction.instruction_valid = 0;
               printf("Sent finally\n");
            } else {
               //printf("still can't send\n");
            }
         } else {
            clear_instruction(&interface->user_instruction);
            interface->user_instruction.instruction_valid = 0;
            socket_handler(&interface->user_command_set, interface->user_input);
            if(interface->user_command_set){
               if(interface->user_input[0] == 13 || interface->user_input[0] == 10 || interface->user_input[0] == 'q'){
                  // new line or CR so finish
                  interface->machine_state = MACHINE_IDLE;
                  interface->user_instruction.instruction_valid = 0;
                  print_set = 0;
               } else {
                  receive_user_control(interface);
               }
            } else {
               interface->machine_state = USER_CONTROL;
            }
            if(interface->user_instruction.instruction_valid){
               interface->cnc_write_data[0] = (uint8_t) NEW_CNC_INSTRUCTION;
               interface->cnc_write_length = instruction_to_string(&interface->user_instruction, &interface->cnc_write_data[1]);
               interface->cnc_write_length = interface->cnc_write_length + 1;
               if(cnc_serial.send(interface->cnc_write_length, interface->cnc_write_data) > 0){
                  interface->user_instruction.instruction_valid = 0;
                  //printf("Sent first time\n");
               } else {
                  printf("couldn't send first time\n");
               }
            }
         }
			break;
		}
		case CONFIGURE_INTERFACE : {
			configure_machine(interface);
			break;
		}
		case GET_PROGRAM : {
			socket_handler(&interface->user_command_set, interface->user_input);
			receive_user_input(interface);
			if(interface->user_command_finished){
				interface->machine_state = OPEN_PROGRAM;
				strcpy(interface->file_name, interface->user_command);
				clear_user_command(interface);
            printf("\n");
			}
			break;
		}
		case OPEN_PROGRAM : {
			if(open_gcode(interface) < 0){
				interface->machine_state = MACHINE_IDLE;
			} else {
				interface->machine_state = RUN_PROGRAM;
				interface->program.instruction_rp = 0;
			}
			break;
		}
		case RUN_PROGRAM : {
			//printf("starting program, length: %llu\n", interface->program.program_length);
			interface->machine_state = SEND_INSTRUCTION;
			break;
		}
      case SEND_INSTANT_INSTRUCTION : {
         interface->cnc_write_data[0] = (uint8_t) NEW_CNC_INSTRUCTION;
         interface->cnc_write_length = instruction_to_string(&interface->user_instruction,
                                                   &interface->cnc_write_data[1]);
         interface->cnc_write_length = interface->cnc_write_length + 1; // add one for 0 opcode
         if(cnc_serial.send(interface->cnc_write_length, interface->cnc_write_data) > 0){
            //printf("Sent user instruction!\n");
            clear_instruction(&interface->user_instruction);
            interface->machine_state = MACHINE_IDLE;
         } else {
            interface->machine_state = SEND_INSTANT_INSTRUCTION;
         }
         break;
      }
		case SEND_INSTRUCTION : {
         if(interface->program.instruction_rp < interface->program.program_length){
            if(interface->program.instruction[interface->program.instruction_rp].instruction_valid){
               interface->cnc_write_data[0] = (uint8_t) NEW_CNC_INSTRUCTION;
               interface->cnc_write_length = gcode_instruction_to_string(&interface->program.instruction[interface->program.instruction_rp],
                                                         &interface->cnc_write_data[1]);
               interface->cnc_write_length = interface->cnc_write_length + 1; // add one for 0 opcode
               if(cnc_serial.send(interface->cnc_write_length, interface->cnc_write_data) > 0){
                  interface->program.instruction_rp++;
                  //printf("Sent instruction!\n");
                  interface->instruction_requested = 0;
                  interface->machine_state = MACHINE_IDLE;
               } else {
                  interface->machine_state = SEND_INSTRUCTION;
               }
            } else {
               // skip instruction that somehow isn't valid...
               printf("SKIPPED INVALID INSTRUCTION?!\n");
               interface->program.instruction_rp++;
            }
         } else {
            printf("EXCEEDED PROGRAM LENGTH?!\n");
            interface->instruction_requested = 0;
            interface->machine_state = MACHINE_IDLE;
         }
			break;
		}
		case UPDATE_FIRMARE : {
			update_firmware(interface);
			interface->machine_state = MACHINE_IDLE;
			break;
		}
		default : {
			interface->machine_state = MACHINE_IDLE;
			break;
		}
	}
}

void receive_user_input(struct interface_struct* interface){
	if(print_set){
		if(interface->user_command_set && !interface->user_command_finished){
			if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
				// new line or CR so finish string
				printf("\n");
				interface->user_command[interface->command_counter] = '\0';
				interface->user_command_finished = 1;
				print_set = 0;
			} else {
				if(interface->user_input[0] == 8 || interface->user_input[0] == 127){
					// backspace
					interface->command_counter = (interface->command_counter > 0) ? interface->command_counter - 1 : 0;
					interface->user_command[interface->command_counter] = ' ';
					printf("\rCommand: %s", interface->user_command);
					fflush(stdout);
					interface->user_command[interface->command_counter] = '\0';
				} else {
					interface->user_command[interface->command_counter] = interface->user_input[0];
					interface->command_counter++;
					interface->user_command[interface->command_counter] = '\0';
				}
			}
			interface->user_input[0] = '\0';
			interface->user_command_set = 0;
			printf("\rCommand: %s", interface->user_command);
			fflush(stdout);
		}
	} else {
		print_set = 1;
		printf("\nEnter Command and Press Enter. Enter h For Help\n");
		printf("\rCommand: %s", interface->user_command);
		fflush(stdout);
	}
}

void receive_user_control(struct interface_struct* interface){
	if(interface->user_command_set){
		// convert letters to lowercase
		if((interface->user_input[0] > 64) && (interface->user_input[0] < 91)){
			interface->user_input[0] = interface->user_input[0] + 32;
		}
		//printf("Data: %d, %d, %d, %d, %d, %d, %d\n", interface->user_input[0], interface->user_input[1], interface->user_input[2], interface->user_input[3], interface->user_input[4], interface->user_input[5], interface->user_input[6]);
		switch(interface->user_input[0]){
			case 'a' : {
				// abort current instruction
            interface->user_instruction.instruction_type = AUX_INSTRUCTION;
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.instruction.aux.opcode = ABORT_INSTRUCTION;
				break;
			}
			case 'e' : {
            interface->user_instruction.instruction_type = AUX_INSTRUCTION;
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.instruction.aux.opcode = ENABLE_MOTORS;
            for(int i=0;i<NUM_MOTORS;i++){
               interface->user_instruction.instruction.aux.enable_motor[i] = 1;
            }
				break;
			}
			case 'd' : {
				interface->user_instruction.instruction_type = AUX_INSTRUCTION;
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.instruction.aux.opcode = DISABLE_MOTORS;
            for(int i=0;i<NUM_MOTORS;i++){
               interface->user_instruction.instruction.aux.disable_motor[i] = 1;
            }
				break;
			}
			case 27 : { // escape character 
				if(!strcmp(&interface->user_input[1], "[A")){
					// up arrow - forward
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].relative_move = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].end_position = move_distance;
					interface->user_instruction.instruction.motors.speed = move_speed;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[B")){
					// down arrow - backwarda
					interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].relative_move = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].end_position = -1*move_distance;
					interface->user_instruction.instruction.motors.speed = move_speed;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[C")){
					// right arrow - right
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].relative_move = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].end_position = move_distance;
					interface->user_instruction.instruction.motors.speed = move_speed;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[D")){
					// left arrow - left
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].relative_move = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].end_position = -1*move_distance;
					interface->user_instruction.instruction.motors.speed = move_speed;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[1;2A")){
					// shift and up arrow - up
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].relative_move = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].end_position = z_move_distance;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].relative_move = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].end_position = z_move_distance;
					interface->user_instruction.instruction.motors.speed = move_speed;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[1;2B")){
					// shift and down arrow - down
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].relative_move = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].end_position = -1*z_move_distance;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].relative_move = 1;
					interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].end_position = -1*z_move_distance;
					interface->user_instruction.instruction.motors.speed = move_speed;
					break;
				}
				break;
			}
			case 'c' : {
				// config which will measure all axis lengths
				/*interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.opcode = MEASURE_AXIS;
				interface->user_instruction.xl_axis.opcode = MEASURE_AXIS;
				interface->user_instruction.xl_axis.current_period = move_speed;
				interface->user_instruction.xl_axis.instruction_valid = 1;
				interface->user_instruction.yf_axis.opcode = MEASURE_AXIS;
				interface->user_instruction.yf_axis.current_period = move_speed;
				interface->user_instruction.yf_axis.instruction_valid = 1;
				interface->user_instruction.zl_axis.opcode = MEASURE_AXIS;
				interface->user_instruction.zl_axis.current_period = move_speed;
				interface->user_instruction.zl_axis.instruction_valid = 1;
				interface->user_instruction.zr_axis.opcode = MEASURE_AXIS;
				interface->user_instruction.zr_axis.current_period = move_speed;
				interface->user_instruction.zr_axis.instruction_valid = 1;*/
				break;
			}
			case 'z' : {
            interface->user_instruction.instruction_type = AUX_INSTRUCTION;
            interface->user_instruction.instruction_valid = 1;
            interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
            interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_XL] = 1;
            interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_YF] = 1;
            interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_ZL] = 1;
            interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_ZR] = 1;
            interface->user_instruction.instruction.aux.motor_speed = move_speed;
				break;
			}
			case 'm' : {
				interface->user_instruction.instruction_type = AUX_INSTRUCTION;
            interface->user_instruction.instruction_valid = 1;
            interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
            interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_XL] = 1;
            interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_YF] = 1;
            interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_ZL] = 1;
            interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_ZR] = 1;
            interface->user_instruction.instruction.aux.motor_speed = move_speed;
				break;
			}
			default : {
				interface->user_instruction.instruction_valid = 0;
				break;
			}
		}
		interface->user_instruction.instant_instruction = 1;
		interface->user_command_set = 0;
	}
}

void process_request(struct interface_struct* interface){
	// setting machine state to default...
	// it will change in switch if needed
	//interface->machine_state = MACHINE_IDLE;
	switch((CNC_OPCODES) interface->cnc_read_data[0]){
		case GET_CNC_VERSION : {
			printf("CNC Version: %s\n", &interface->cnc_read_data[1]);
			break;
		}
		case GET_CNC_STATUS : {
         interface->status_received = 1;
			string_to_status(&interface->machine_status, &interface->cnc_read_data[1]);
			printf("CNC XL Min: %d\n", interface->machine_status.endstop_status[X_L_MIN]);
			printf("CNC XL Max: %d\n", interface->machine_status.endstop_status[X_R_MAX]);
			printf("CNC YF Min: %d\n", interface->machine_status.endstop_status[Y_F_MIN]);
			printf("CNC YF Max: %d\n", interface->machine_status.endstop_status[Y_B_MAX]);
			printf("CNC ZL Min: %d\n", interface->machine_status.endstop_status[Z_L_MIN]);
			printf("CNC ZL Max: %d\n", interface->machine_status.endstop_status[Z_L_MAX]);
			printf("CNC ZR Min: %d\n", interface->machine_status.endstop_status[Z_R_MIN]);
			printf("CNC ZR Max: %d\n", interface->machine_status.endstop_status[Z_R_MAX]);
			printf("CNC EX0 Pos: %lf\n", interface->machine_status.position[MOTOR_AUX]);
			printf("CNC EX1 Pos: %lf\n", interface->machine_status.position[MOTOR_EXTRUDER_0]);
			printf("CNC AUX Pos: %lf\n", interface->machine_status.position[MOTOR_EXTRUDER_1]);
			printf("CNC XL Pos: %lf\n", interface->machine_status.position[MOTOR_AXIS_XL]);
			printf("CNC YF Pos: %lf\n", interface->machine_status.position[MOTOR_AXIS_YF]);
			printf("CNC ZL Pos: %lf\n", interface->machine_status.position[MOTOR_AXIS_ZL]);
			printf("CNC ZR Pos: %lf\n", interface->machine_status.position[MOTOR_AXIS_ZR]);
         printf("Program Length: %lld, Current Instruction: %lld\n", interface->program.program_length, interface->program.instruction_rp);
         
         #define IN_VOLT ((cnc_double) 3.3)
         #define BETA_VALUE ((cnc_double) 3950.0)
         #define BASE_RESISTANCE ((cnc_double) 22000.0)
         #define THERMISTOR_RESISTANCE ((cnc_double) 100000.0)
         #define KELVIN_CONV ((cnc_double) 273.15)
         #define BASE_TEMP ((cnc_double) 25.0) // degrees celsius
         #define BASE_TEMP_KELVIN (BASE_TEMP + KELVIN_CONV)
         #define ADC_MAX ((cnc_double) ((1<<10)-1))

         cnc_double current_voltage;
         cnc_double current_resistance;
         cnc_double log_val;
         cnc_double temp_calc;
         cnc_double temp_calc_f;
         cnc_double adc_calc; 

         current_voltage = ((cnc_double) IN_VOLT*interface->machine_status.temp[0])/ADC_MAX;
         if(current_voltage > 0.0){
            current_resistance = (((cnc_double) IN_VOLT*BASE_RESISTANCE)/current_voltage)-BASE_RESISTANCE;
            if(current_resistance > 0){
               log_val = ((cnc_double) log(current_resistance/THERMISTOR_RESISTANCE));
               temp_calc = (((cnc_double) 1.0/BASE_TEMP_KELVIN) + ((cnc_double) log_val/BETA_VALUE));
               temp_calc = (cnc_double) 1.0/temp_calc;
               temp_calc = temp_calc - KELVIN_CONV;
            }
         }
         temp_calc_f = temp_calc * 9/5 + 32;
			printf("CNC H0 Temp: %lf,%lf,%lf, %d\n", interface->machine_status.temp[0], temp_calc, temp_calc_f, interface->machine_status.temp_locked[0]);
			printf("CNC H1 Temp: %lf, %d\n", interface->machine_status.temp[1], interface->machine_status.temp_locked[1]);
			printf("CNC H2 Temp: %lf, %d\n", interface->machine_status.temp[2], interface->machine_status.temp_locked[2]);
			printf("CNC H3 Temp: %lf, %d\n", interface->machine_status.temp[3], interface->machine_status.temp_locked[3]);
			break;
		}
		case NEW_CNC_PRINT : {
			printf("CNC Printf: %s\n", &interface->cnc_read_data[1]);
			break;
		}
		case SET_CNC_CONFIG : {
			//interface->machine_state = SEND_CONFIG;
			//printf("Got Config Data Request\n");
			break;
		}
		case FLASH_FIRMWARE : {

			break;
		}
		case NEW_CNC_INSTRUCTION : {
         interface->instruction_requested = 1;
			//printf("Machine Requested Instruction\n");
			break;
		}
		case INSTANT_CNC_INSTRUCTION : {
			//interface->machine_state = USER_CONTROL;
			printf("Getting User Input To Control Machine:\n");
			break;
		}
		case DISABLE_ROUTE : {

			break;
		}
		case ENABLE_ROUTE : {

			break;
		}
		case PAUSE_PROGRAM : {

			break;
		}
		case RESUME_PROGRAM : {

			break;
		}
		case MARKER : {
			//printf("received machine marker!\n");
         string_to_status(&interface->machine_status, &interface->cnc_read_data[1]);
			// printf("CNC XL Min: %d\n", interface->machine_status.endstop_status[X_L_MIN]);
			// printf("CNC XL Max: %d\n", interface->machine_status.endstop_status[X_R_MAX]);
			// printf("CNC YF Min: %d\n", interface->machine_status.endstop_status[Y_F_MIN]);
			// printf("CNC YF Max: %d\n", interface->machine_status.endstop_status[Y_B_MAX]);
			// printf("CNC ZL Min: %d\n", interface->machine_status.endstop_status[Z_L_MIN]);
			// printf("CNC ZL Max: %d\n", interface->machine_status.endstop_status[Z_L_MAX]);
			// printf("CNC ZR Min: %d\n", interface->machine_status.endstop_status[Z_R_MIN]);
			// printf("CNC ZR Max: %d\n", interface->machine_status.endstop_status[Z_R_MAX]);
			// printf("CNC EX0 Pos: %lld\n", interface->machine_status.position[MOTOR_AUX]);
			// printf("CNC EX1 Pos: %lld\n", interface->machine_status.position[MOTOR_EXTRUDER_0]);
			// printf("CNC AUX Pos: %lld\n", interface->machine_status.position[MOTOR_EXTRUDER_1]);
			// printf("CNC XL Pos: %lld\n", interface->machine_status.position[MOTOR_AXIS_XL]);
			// printf("CNC YF Pos: %lld\n", interface->machine_status.position[MOTOR_AXIS_YF]);
			// printf("CNC ZL Pos: %lld\n", interface->machine_status.position[MOTOR_AXIS_ZL]);
			// printf("CNC ZR Pos: %lld\n", interface->machine_status.position[MOTOR_AXIS_ZR]);
			interface->machine_marker = 1;
			break;
		}
      case INSTANT_DONE : {
			//printf("instant instruction done!\n");
			interface->instant_done = 1;
			break;
		}
		case ERROR : {

			break;
		}
		default : {
			interface->machine_state = MACHINE_IDLE;
			break;
		}
	}
}

void handle_input(struct interface_struct* interface){
	// set states back to default states...
	// all tasks that need it to move to another state will set it
	interface->state = INTERFACE_RUNNING;
	interface->machine_state = MACHINE_IDLE;
	if(interface->user_command_finished){
		if((interface->user_command[0] > 64) && (interface->user_command[0] < 91)){
			interface->user_command[0] = interface->user_command[0] + 32;
		}
		switch(interface->user_command[0]){
			case 'q' : {
				interface->state = DISABLE_GPIO;
				break;
			}
			case 'h' : {
				print_interface_menu();
				break;
			}
         case 'i' : {
            process_instruction(interface);
            break;
         }
			case 'u' : {
				interface->machine_state = USER_CONTROL;
				printf("Getting User Instruction!\n");
				break;
			}
			case 'f' : {
				interface->machine_state = UPDATE_FIRMARE;
				break;
			}
			case 'r' : {
				//spi_reconnect();
				break;
			}
			case 'g' : {
				interface->machine_state = GET_STATUS;
				break;
			}
			case 'v' : {
				interface->machine_state = GET_VERSION;
				break;
			}
			case 'o' : {
				printf("\nEnter Name Of File to Open\n");
				interface->machine_state = GET_PROGRAM;
				break;
			}
			case 'e' : {
				interface->machine_state = POWER_MOTORS_ON;
				break;
			}
			case 'd' : {
				interface->machine_state = POWER_MOTORS_OFF;
				break;
			}
			case 'c' : {
				interface->machine_state = CONFIGURE_INTERFACE;
				configure_stage = CONFIG_ENABLE_POWER;
				configure_processing = 0;
				break;
			}
			case 'l' : {
				interface->machine_state = SEND_CONFIG;
				break;
			}
			case 's' : {
            sscanf(&interface->user_command[1], "%f", &move_speed);
            if(move_speed > 0.0){

            } else {
               move_speed = DEFAULT_SPEED;
            }
				break;
			}
         case 'p' : {
            if(interface->user_command[1] == 'x'){
               sscanf(&interface->user_command[2], "%f", &new_position);
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = SET_POSITION;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_XL] = new_position;
               interface->machine_state = SEND_INSTANT_INSTRUCTION;
            }
            if(interface->user_command[1] == 'y'){
               sscanf(&interface->user_command[2], "%f", &new_position);
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = SET_POSITION;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_YF] = 1;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_YF] = new_position;
               interface->machine_state = SEND_INSTANT_INSTRUCTION;
            }
            if(interface->user_command[1] == 'z'){
               sscanf(&interface->user_command[2], "%f", &new_position);
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = SET_POSITION;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_ZL] = 1;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_ZR] = 1;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_ZL] = new_position;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_ZR] = new_position;
               interface->machine_state = SEND_INSTANT_INSTRUCTION;
            }
            break;
         }
			default : {
				//spi_struct->pending_opcode = idle;
				interface->machine_state = MACHINE_IDLE;
				break;
			}
		}
		clear_user_command(interface);
	}
}

void process_instruction(struct interface_struct* interface){
   uint8_t valid_instruction = 0;
   cnc_double temp = 0.0;
   switch(interface->user_command[1]){
      case 'm' : {
         //printf("motor instruction\n");
         break;
      }
      case 'h' : {
         if(interface->user_command[2] == 'e'){
            sscanf(&interface->user_command[3], "%fC", &temp);
            if(temp > 1.0){
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instruction_type = HEATER_INSTRUCTION;
               interface->user_instruction.instruction.heaters.heater[0].instruction_valid = 1;
               interface->user_instruction.instruction.heaters.heater[0].enable_heater = 1;
               interface->user_instruction.instruction.heaters.heater[0].fan_duty = 100;
               interface->user_instruction.instruction.heaters.heater[0].enable_fan = 1;
               interface->user_instruction.instruction.heaters.heater[0].wait_for_temp = 1;
               interface->user_instruction.instruction.heaters.heater[0].target_temp = temp;
               valid_instruction = 1;
            } else {
            }
         } else {
            if(interface->user_command[2] == 'd'){
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instruction_type = HEATER_INSTRUCTION;
               interface->user_instruction.instruction.heaters.heater[0].instruction_valid = 1;
               interface->user_instruction.instruction.heaters.heater[0].disable_heater = 1;
               interface->user_instruction.instruction.heaters.heater[0].disable_fan = 1;
               interface->user_instruction.instruction.heaters.heater[0].wait_for_temp = 0;
               valid_instruction = 1;
            }
         }
         break;
      }
      default : {
         valid_instruction = 0;
         break;
      }
   }

   if(valid_instruction > 0){
      interface->machine_state = SEND_INSTANT_INSTRUCTION;
   } else {
      interface->machine_state = MACHINE_IDLE;
   }
}

void configure_machine(struct interface_struct* interface){
   //printf("Getting Size and Waiting for Status\n");
   socket_handler(&interface->user_command_set, interface->user_input);
   if(interface->user_command_set){
      if(interface->user_input[0] == 'a' || interface->user_input[0] == 'q'){
         // new line or CR so finish
         interface->machine_state = MACHINE_IDLE;
         interface->user_instruction.instruction_valid = 0;
         interface->user_command_set = 0;
         print_set = 0;
      }
   }
   if(cnc_serial.tx_pending){
      // wait for tx to be done
   } else {
      clear_instruction(&interface->user_instruction);
      interface->user_instruction.instruction_valid = 0;
      switch(configure_stage){
         case CONFIG_ENABLE_POWER : {
            enable_motors();
            configure_stage = ENABLE_ALL_MOTORS;
            configure_processing = 0;
            break;
         }
         case ENABLE_ALL_MOTORS : {
            // enable all axis motors
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = ENABLE_MOTORS;
               interface->user_instruction.instruction.aux.enable_motor[MOTOR_AUX] = 1;
               interface->user_instruction.instruction.aux.enable_motor[MOTOR_EXTRUDER_0] = 1;
               interface->user_instruction.instruction.aux.enable_motor[MOTOR_EXTRUDER_1] = 1;
               interface->user_instruction.instruction.aux.enable_motor[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.enable_motor[MOTOR_AXIS_YF] = 1;
               interface->user_instruction.instruction.aux.enable_motor[MOTOR_AXIS_ZL] = 1;
               interface->user_instruction.instruction.aux.enable_motor[MOTOR_AXIS_ZR] = 1;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = MAX_ZL_ZR_FAST;
               configure_processing = 0;
            }
            break;
         }
         case MAX_ZL_ZR_FAST : {
            // start by sending instruction to max the z axis on left and right
            if(!configure_processing){
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_ZL] = 1;
               interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_ZR] = 1;
               interface->user_instruction.instruction.aux.motor_speed = FAST_SPEED;
            }
            if(interface->instant_done){
               printf("Zero X and Y Motors...\n");
               interface->instant_done = 0;
               configure_stage = OFFSET_ZL_R_SLOW;
               configure_processing = 0;
            }
            break;
         }
         case OFFSET_ZL_R_SLOW : {
            if(!configure_processing){
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].relative_move = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].end_position = -5.0;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].relative_move = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].end_position = -5.0;
               interface->user_instruction.instruction.motors.speed = SLOW_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = MAX_ZL_ZR_SLOW;
               configure_processing = 0;
            }
            break;
         }
         case MAX_ZL_ZR_SLOW : {
            // start by sending instruction to max the z axis on left and right
            if(!configure_processing){
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_ZL] = 1;
               interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_ZR] = 1;
               interface->user_instruction.instruction.aux.motor_speed = SLOW_SPEED;
            }
            if(interface->instant_done){
               printf("Zero X and Y Motors...\n");
               interface->instant_done = 0;
               configure_stage = SET_ZERO_ZL_ZR;
               configure_processing = 0;
            }
            break;
         }
         case SET_ZERO_ZL_ZR : {
            if(!configure_processing){
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = SET_POSITION;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_ZL] = 1;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_ZR] = 1;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_ZL] = 0.0;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_ZR] = 0.0;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = MIN_X_Y_FAST;
               configure_processing = 0;
            }
            break;
         }
         case MIN_X_Y_FAST : {
            // send instruction to zero x and y
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_YF] = 1;
               interface->user_instruction.instruction.aux.motor_speed = FAST_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = OFFSET_X_Y;
               configure_processing = 0;
            }
            break;
         }
         case OFFSET_X_Y : {
            if(!configure_processing){
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].relative_move = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].end_position = 5.0;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].relative_move = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].end_position = 5.0;
               interface->user_instruction.instruction.motors.speed = SLOW_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = MIN_X_Y_SLOW;
               configure_processing = 0;
            }
            break;
         }
         case MIN_X_Y_SLOW : {
            // send instruction to zero x and y
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_YF] = 1;
               interface->user_instruction.instruction.aux.motor_speed = SLOW_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = SET_ZERO_X_Y;
               configure_processing = 0;
            }
            break;
         }
         case SET_ZERO_X_Y : {
            if(!configure_processing){
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = SET_POSITION;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_YF] = 1;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_XL] = 0.0;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_YF] = 0.0;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = MANUAL_ZL_ZR_SAFE;
               configure_processing = 0;
               printf("Manually adjust Z for new zero position...\n");
               printf("Press Enter When Finished\n");
            }
            break;
         }
         case MANUAL_ZL_ZR_SAFE : {
            // Manually move to safe z position
            if(interface->user_command_set){
               interface->user_command_set = 0;
               if(interface->user_input[0] == 27){
                  if(!strcmp(&interface->user_input[1], "[A")){
                     // up arrow - up
                     interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
                     interface->user_instruction.instruction_valid = 1;
                     interface->user_instruction.instant_instruction = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].instruction_valid = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].relative_move = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].end_position = 10.0;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].instruction_valid = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].relative_move = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].end_position = 10.0;
                     interface->user_instruction.instruction.motors.speed = FAST_SPEED;
                  }
                  if(!strcmp(&interface->user_input[1], "[B")){
                     // down arrow - down
                     interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
                     interface->user_instruction.instruction_valid = 1;
                     interface->user_instruction.instant_instruction = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].instruction_valid = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].relative_move = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].end_position = -10.0;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].instruction_valid = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].relative_move = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].end_position = -10.0;
                     interface->user_instruction.instruction.motors.speed = FAST_SPEED;
                  }
               } else {
                  if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
                     printf("Manually adjust Zl for new zero position...\n");
                     printf("Press Enter When Finished\n");
                     interface->instant_done = 0;
                     configure_stage = MANUAL_ZL_ZERO;
                     configure_processing = 0;
                  }
               }
            }
            break;
         }
         case MANUAL_ZL_ZERO : {
            // manually adjust zl and zr to create new safe position
            if(interface->user_command_set){
               interface->user_command_set = 0;
               if(interface->user_input[0] == 27){
                  if(!strcmp(&interface->user_input[1], "[A")){
                     // up arrow - up
                     interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
                     interface->user_instruction.instruction_valid = 1;
                     interface->user_instruction.instant_instruction = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].instruction_valid = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].relative_move = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].end_position = 0.1;
                     interface->user_instruction.instruction.motors.speed = SLOW_SPEED;
                  }
                  if(!strcmp(&interface->user_input[1], "[B")){
                     // down arrow - down
                     interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
                     interface->user_instruction.instruction_valid = 1;
                     interface->user_instruction.instant_instruction = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].instruction_valid = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].relative_move = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZL].end_position = -0.1;
                     interface->user_instruction.instruction.motors.speed = SLOW_SPEED;
                  }
               } else {
                  if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
                     interface->instant_done = 0;
                     configure_stage = MAX_X_FAST;
                     configure_processing = 0;
                  }
               }
            }
            break;
         }
         case MAX_X_FAST : {
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.motor_speed = FAST_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = OFFSET_X;
               configure_processing = 0;
            }
            break;
         }
         case OFFSET_X : {
            if(!configure_processing){
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].relative_move = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_XL].end_position = -5.0;
               interface->user_instruction.instruction.motors.speed = SLOW_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = MAX_X_SLOW;
               configure_processing = 0;
            }
            break;
         }
         case MAX_X_SLOW : {
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.motor_speed = SLOW_SPEED;
            }
            if(interface->instant_done){
               printf("Manually adjust ZR for new zero position...\n");
               printf("Press Enter When Finished\n");
               interface->instant_done = 0;
               configure_stage = MANUAL_ZR_ZERO;
               configure_processing = 0;
            }
            break;
         }
         case MANUAL_ZR_ZERO : {
            // manually adjust zl and zr to create new safe position
            if(interface->user_command_set){
               interface->user_command_set = 0;
               if(interface->user_input[0] == 27){
                  if(!strcmp(&interface->user_input[1], "[A")){
                     // up arrow - up
                     interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
                     interface->user_instruction.instruction_valid = 1;
                     interface->user_instruction.instant_instruction = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].instruction_valid = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].relative_move = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].end_position = 0.1;
                     interface->user_instruction.instruction.motors.speed = SLOW_SPEED;
                  }
                  if(!strcmp(&interface->user_input[1], "[B")){
                     // down arrow - down
                     interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
                     interface->user_instruction.instruction_valid = 1;
                     interface->user_instruction.instant_instruction = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].instruction_valid = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].relative_move = 1;
                     interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_ZR].end_position = -0.1;
                     interface->user_instruction.instruction.motors.speed = SLOW_SPEED;
                  }
               } else {
                  if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
                     interface->instant_done = 0;
                     configure_stage = SET_Z_ZERO;
                     configure_processing = 0;
                  }
               }
            }
            break;
         }
         case SET_Z_ZERO : {
            if(!configure_processing){
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = SET_POSITION;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_ZL] = 1;
               interface->user_instruction.instruction.aux.set_position[MOTOR_AXIS_ZR] = 1;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_ZL] = 0.0;
               interface->user_instruction.instruction.aux.motor_position[MOTOR_AXIS_ZR] = 0.0;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = MAX_Y_FAST;
               configure_processing = 0;
            }
            break;
         }
         case MAX_Y_FAST : {
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_YF] = 1;
               interface->user_instruction.instruction.aux.motor_speed = FAST_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = OFFSET_Y;
               configure_processing = 0;
            }
            break;
         }
         case OFFSET_Y : {
            if(!configure_processing){
               interface->user_instruction.instruction_type = MOTOR_INSTRUCTION;
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].instruction_valid = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].relative_move = 1;
               interface->user_instruction.instruction.motors.motor[MOTOR_AXIS_YF].end_position = -5.0;
               interface->user_instruction.instruction.motors.speed = SLOW_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = MAX_Y_SLOW;
               configure_processing = 0;
            }
            break;
         }
         case MAX_Y_SLOW : {
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.max_motor[MOTOR_AXIS_YF] = 1;
               interface->user_instruction.instruction.aux.motor_speed = SLOW_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = SAVE_AXIS_LENGTHS;
               configure_processing = 0;
            }
            break;
         }
         case SAVE_AXIS_LENGTHS : {
            interface->instant_done = 0;
            configure_stage = LEVEL_Y;
            break;
         }
         case LEVEL_Y : {
            interface->instant_done = 0;
            configure_stage = MIN_X;
            break;
         }
         case MIN_X : {
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instruction.aux.opcode = MOVE_TO_ENDSTOP;
               interface->user_instruction.instruction.aux.min_motor[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.motor_speed = FAST_SPEED;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = LEVEL_X;
               configure_processing = 0;
            }
            break;
         }
         case LEVEL_X : {
            interface->instant_done = 0;
            configure_stage = CENTER_ZL_ZR_X_Y;
            break;
         }
         case CENTER_ZL_ZR_X_Y : {
            interface->instant_done = 0;
            configure_stage = DISABLE_ALL_MOTORS;
            break;
         }
         case DISABLE_ALL_MOTORS : {
            if(!configure_processing){
               interface->user_instruction.instruction_valid = 1;
               interface->user_instruction.instruction_type = AUX_INSTRUCTION;
               interface->user_instruction.instant_instruction = 1;
               interface->user_instruction.instruction.aux.opcode = ENABLE_MOTORS;
               interface->user_instruction.instruction.aux.disable_motor[MOTOR_AUX] = 1;
               interface->user_instruction.instruction.aux.disable_motor[MOTOR_EXTRUDER_0] = 1;
               interface->user_instruction.instruction.aux.disable_motor[MOTOR_EXTRUDER_1] = 1;
               interface->user_instruction.instruction.aux.disable_motor[MOTOR_AXIS_XL] = 1;
               interface->user_instruction.instruction.aux.disable_motor[MOTOR_AXIS_YF] = 1;
               interface->user_instruction.instruction.aux.disable_motor[MOTOR_AXIS_ZL] = 1;
               interface->user_instruction.instruction.aux.disable_motor[MOTOR_AXIS_ZR] = 1;
            }
            if(interface->instant_done){
               interface->instant_done = 0;
               configure_stage = CONFIG_DISABLE_POWER;
               configure_processing = 0;
            }
            break;
         }
         case CONFIG_DISABLE_POWER : {
            disable_motors();
            interface->instant_done = 0;
            configure_stage = CONFIG_ERROR;
            break;
         }
         case CONFIG_ERROR : {
            configure_stage = CONFIGURED;
            break;
         }
         case CONFIGURED : {
            configure_stage = CONFIG_ENABLE_POWER;
            interface->machine_state = MACHINE_IDLE;
            break;
         }
         default : {
            interface->machine_state = MACHINE_IDLE;
            break;
         }
      }
      if(interface->user_instruction.instruction_valid){
         interface->cnc_write_data[0] = (char) NEW_CNC_INSTRUCTION;
         interface->cnc_write_length = instruction_to_string(&interface->user_instruction, &interface->cnc_write_data[1]);
         interface->cnc_write_length = interface->cnc_write_length + 1;
         if(cnc_serial.send(interface->cnc_write_length, interface->cnc_write_data) > 0){
            configure_processing = 1;
            printf("config instruction sent\n");
         }
      }
   }
}

void load_config_file(struct cnc_config_struct* config){
	FILE* config_file;
	uint8_t config_string[sizeof(cnc_config_struct)];
	config_file = fopen("./user_config.cnfg", "r+");
	uint16_t count = 0;
	uint8_t file_valid = 0;

	if(config_file < 0){
		printf("Error Loading Config File... Setting Default\n");
		file_valid = 0;
	} else {
		if(config_file == NULL){
			printf("Couldn't Open Config File... Setting Default\n");
			file_valid = 0;
		} else {
			printf("Reading Config Data From File\n");
			file_valid = 1;
		}
	}

	if(file_valid){
		// load_from_file
		while(count < (sizeof(cnc_config_struct) - 1)){
			config_string[count] = (uint8_t) fgetc(config_file);
			count = count + 1;
		}
		string_to_config(config, config_string);
	} else {
		// load from default
		*config = CONFIG_DEFAULT;
	}

	if(config_file){
		fclose(config_file);
	}
	if(config->valid_config){
		config->config_loaded = 1;
	}
}

void save_config_file(struct cnc_config_struct* config){
	FILE* config_file;
	uint8_t config_string[sizeof(cnc_config_struct)];
	config_file = fopen("./user_config.cnfg", "w+");
	uint16_t count = 0;
	uint8_t file_valid = 0;

	if(!config->config_loaded || config_file < 0){
		printf("Config Not Set Or Error Loading Config File... Not Writing\n");
		file_valid = 0;
	} else {
		if(config_file == NULL){
			printf("Couldn't Open Config File... Not Writing\n");
			file_valid = 0;
		} else {
			printf("Writing Config Data To File\n");
			file_valid = 1;
		}
	}

	if(file_valid){
		config_to_string(config, config_string);
		// load to file
		while(count < (sizeof(cnc_config_struct) - 1)){
			fputc((uint8_t) config_string[count], config_file);
			count = count + 1;
		}
	} else {
		// load from default
		*config = CONFIG_DEFAULT;
	}

	if(config_file){
		fclose(config_file);
	}
}

void clear_user_command(struct interface_struct* interface){
	while(interface->command_counter>0){
		//printf("\rCommand: %s", interface->user_command);
		//fflush(stdout);
		interface->user_command[interface->command_counter] = '\0';
		interface->command_counter--;
	};
	interface->user_command[0] = '\0';
	interface->command_counter = 0;
	interface->user_command_set = 0;
	interface->user_command_finished = 0;
}

void length_to_string(uint8_t byte_length, char* length_string){
	uint8_t count = 0;
	
	for(count=0;count<byte_length;count++){
		length_string[count] = ((byte_length >> (count*8)) & 0xFF);
	}
}

void update_firmware(struct interface_struct* interface){
	/*
   //enum spi_opcodes opcode;
	//char send_string[IDLE_LENGTH] = {0};
	//if(spi->updating_firmware == 1){
		//if(!get_s_ready_state(spi)){
			system("cp \"../machine/GNU ARM v7.2.1 - Debug/machine.hex\" /media/pi/STK3402/");
			//spi->updating_firmware = 0;
			//spi->state = spi_initialized;
		//}
	//} else {
		if(system("ls /media/pi/STK3402/") == 0){ // need to find better check/mask prints in system call
			//spi->updating_firmware = 1;
			//opcode = flash_firmware;
			//opcode_to_string(opcode, &send_string[0]);
			system("cp \"../machine/GNU ARM v7.2.1 - Debug/machine.hex\" /media/pi/STK3402/");
			//wiringPiSPIDataRW(SPI_CHANNEL, send_string, IDLE_LENGTH);
		} else {
			printf("Can't upgrade firmware - not connected\n");
			//spi->state = spi_idle;
		}
		spi_reconnect();
	//}
   */
	interface->machine_state = MACHINE_IDLE;
}

int open_gcode(struct interface_struct* interface){
	printf("openeing file: %s\n", interface->file_name);
	interface->gcode_fp = fopen(interface->file_name, "r");
	if(interface->gcode_fp == NULL){
		printf("Couln't Open File\n");
		return -1;
	} else {
		printf("Opened File!\n");
		
		parse_gcode_file(interface->gcode_fp, &interface->program);
		
		fclose(interface->gcode_fp);
		return 0;
	}
}


