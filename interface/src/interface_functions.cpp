#include "main.h"
#include "stdio.h"
#include "common_spi.h"
#include "common_cnc.h"
#include "interface_spi.h"
#include "gcode.h"

uint8_t print_set = 0;
uint8_t configure_stage = 0;
uint8_t configure_processing = 0;

void handle_cnc_state(struct interface_struct* interface){
	//if(check_spi(interface)){
		// received spi transaction
		// process that instead of user input
		//interface->machine_state = PROCESS_SPI;
		//process_spi_request(interface);
	//}
	switch(interface->machine_state){
		case MACHINE_IDLE : {
			/*if(check_spi(interface)){
				// received spi transaction
				// process that instead of user input
				//interface->machine_state = PROCESS_SPI;
				process_spi_request(interface);
			} else {*/
				// if not processing spi, process user input
				socket_handler(&interface->user_command_set, interface->user_input);
				receive_user_input(interface);
				if(interface->user_command_finished){
					interface->machine_state = PROCESS_INPUT;
				} else {
					/*if(interface->spi_connected && !interface->machine_configured){
						interface->machine_state = SEND_CONFIG;
					}*/
				}
			//}
			break;
		}
		/*case PROCESS_SPI : {
			process_spi_request(interface);
			break;
		}*/
		case PROCESS_INPUT : {
			handle_input(interface);
			break;
		}
		case POWER_MOTORS_ON : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					enable_motors();
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				interface->cnc_write_data[0] = (char) DISABLE_ROUTE;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = POWER_MOTORS_ON;
			}
			break;
		}
		case POWER_MOTORS_OFF : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				disable_motors();
				interface->cnc_write_data[0] = (char) ENABLE_ROUTE;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = POWER_MOTORS_OFF;
			}
			break;
		}
		case GET_STATUS : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				interface->cnc_write_data[0] = (char) GET_CNC_STATUS;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = GET_STATUS;
			}
			break;
		}
		case GET_VERSION : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				interface->cnc_write_data[0] = (char) GET_CNC_VERSION;
				if(spi_set_write(interface->cnc_write_data, 1) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = GET_VERSION;
			}
			break;
		}
		case SEND_CONFIG : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					printf("Sent Config Data\n");
					save_config_file(&interface->machine_config);
					interface->machine_configured = 1;
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				interface->cnc_write_data[0] = (char) SET_CNC_CONFIG;
				interface->cnc_write_length = config_to_string(&interface->machine_config, &interface->cnc_write_data[1]);
				interface->cnc_write_length = interface->cnc_write_length + 1;
				if(spi_set_write(interface->cnc_write_data, interface->cnc_write_length) > 0){
					interface->write_in_progress = 1;
				} else {
					interface->write_in_progress = 0;
				}
				interface->machine_state = SEND_CONFIG;
			}
			break;
		}
		case USER_CONTROL : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					clear_instruction(&interface->user_instruction);
					interface->user_instruction.instruction_valid = 0;
				}
			} else {
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
					interface->cnc_write_data[0] = (char) NEW_CNC_INSTRUCTION;
					interface->cnc_write_length = instruction_to_string(&interface->user_instruction, &interface->cnc_write_data[1]);
					interface->cnc_write_length = interface->cnc_write_length + 1;
					if(spi_set_write(interface->cnc_write_data, interface->cnc_write_length) > 0){
						interface->write_in_progress = 1;
					} else {
						interface->write_in_progress = 0;
					}
				}
			}
			break;
		}
		case CONFIGURE_INTERFACE : {
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
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					clear_instruction(&interface->user_instruction);
					interface->user_instruction.instruction_valid = 0;
				}
			} else {
				switch(configure_stage){
					case 0 : {
						// enable all axis motors
						if(!configure_processing){
							printf("Enabled Motors for Config...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.opcode = ENABLE_MOTORS;
							interface->user_instruction.xl_axis.pending_enable = 1;
							interface->user_instruction.xl_axis.opcode = ENABLE_MOTORS;
							interface->user_instruction.yf_axis.pending_enable = 1;
							interface->user_instruction.yf_axis.opcode = ENABLE_MOTORS;
							interface->user_instruction.zl_axis.pending_enable = 1;
							interface->user_instruction.zl_axis.opcode = ENABLE_MOTORS;
							interface->user_instruction.zr_axis.pending_enable = 1;
							interface->user_instruction.zr_axis.opcode = ENABLE_MOTORS;
							interface->user_instruction.extruder_0.pending_enable = 1;
							interface->user_instruction.extruder_0.opcode = ENABLE_MOTORS;
							interface->user_instruction.extruder_1.pending_enable = 1;
							interface->user_instruction.extruder_1.opcode = ENABLE_MOTORS;
							interface->user_instruction.aux.pending_enable = 1;
							interface->user_instruction.aux.opcode = ENABLE_MOTORS;
						}
						if(interface->machine_marker){
							configure_stage = 1;
							configure_processing = 0;
							interface->machine_marker = 0;
						}
						break;
					}
					case 1 : {
						// start by sending instruction to max the z axis on left and right
						if(!configure_processing){
							printf("Max Z Motors...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.zl_axis.opcode = MAX_MOTOR;
							interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
							interface->user_instruction.zl_axis.instruction_valid = 1;
							interface->user_instruction.zr_axis.opcode = MAX_MOTOR;
							interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
							interface->user_instruction.zr_axis.instruction_valid = 1;
						}
						if(interface->machine_marker){
							configure_stage = 2;
							configure_processing = 0;
							interface->machine_marker = 0;
						}
						break;
					}
					case 2 : {
						// send instruction to zero x and y
						if(!configure_processing){
							printf("Zero X and Y Motors...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.xl_axis.opcode = ZERO_MOTOR;
							interface->user_instruction.xl_axis.current_period = MOVE_PERIOD;
							interface->user_instruction.xl_axis.instruction_valid = 1;
							interface->user_instruction.yf_axis.opcode = ZERO_MOTOR;
							interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
							interface->user_instruction.yf_axis.instruction_valid = 1;
						}
						if(interface->machine_marker){
							configure_stage = 3;
							configure_processing = 0;
							interface->machine_marker = 0;
						}
						break;
					}
					case 3 : {
						// Set position of Zl and Zr to 0 to count to new home position
						if(!configure_processing){
							printf("Setting Zl and Zr to 0...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.opcode = POSITION_AXIS;
							interface->user_instruction.zl_axis.opcode = POSITION_AXIS;
							interface->user_instruction.zl_axis.move_count = 0;
							interface->user_instruction.zl_axis.instruction_valid = 1;
							interface->user_instruction.zr_axis.opcode = POSITION_AXIS;
							interface->user_instruction.zr_axis.move_count = 0;
							interface->user_instruction.zr_axis.instruction_valid = 1;
						}
						if(interface->machine_marker){
							printf("Manually adjust Zl and Zr for new safe position...\n");
							printf("Press Enter When Finished\n");
							configure_stage = 4;
							configure_processing = 0;
							interface->machine_marker = 0;
						}
						break;
					}
					case 4 : {
						// manually adjust zl and zr to create new safe position
						if(interface->user_command_set){
							interface->user_command_set = 0;
							if(interface->user_input[0] == 27){
								if(!strcmp(&interface->user_input[1], "[A")){
									// up arrow - up
									interface->user_instruction.instruction_valid = 1;
									interface->user_instruction.instant_instruction = 1;
									interface->user_instruction.zl_axis.instruction_valid = 1;
									interface->user_instruction.zl_axis.move_count = 200;
									interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
									interface->user_instruction.zr_axis.instruction_valid = 1;
									interface->user_instruction.zr_axis.move_count = 200;
									interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
								}
								if(!strcmp(&interface->user_input[1], "[B")){
									// down arrow - down
									interface->user_instruction.instruction_valid = 1;
									interface->user_instruction.instant_instruction = 1;
									interface->user_instruction.zl_axis.instruction_valid = 1;
									interface->user_instruction.zl_axis.move_count = -200;
									interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
									interface->user_instruction.zr_axis.instruction_valid = 1;
									interface->user_instruction.zr_axis.move_count = -200;
									interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
								}
							} else {
								if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
									printf("Manually adjust Zl for new zero position...\n");
									printf("Press Enter When Finished\n");
									configure_stage = 5;
									configure_processing = 0;
									interface->machine_marker = 0;
								}
							}
						}
						break;
					}
					case 5 : {
						// get status and update safe position for xl
						if(configure_processing){
							if(interface->machine_marker){
								interface->machine_config.zl_min_safe_pos = interface->machine_status.zl_position;
								interface->machine_config.zr_min_safe_pos = interface->machine_status.zr_position;
								interface->machine_marker = 0;
								configure_processing = 0;
								configure_stage = 6;
							}
						} else {
							if(!interface->write_in_progress){
								interface->cnc_write_data[0] = (char) GET_CNC_STATUS;
								if(spi_set_write(interface->cnc_write_data, 1) > 0){
									configure_processing = 1;
									interface->write_in_progress = 1;
								} else {
									interface->write_in_progress = 0;
								}
							}
						}
						break;
					}
					case 6 : {
						// manually adjust zl to zero position
						if(interface->user_command_set){
							interface->user_command_set = 0;
							if(interface->user_input[0] == 27){
								if(!strcmp(&interface->user_input[1], "[A")){
									// up arrow - up
									interface->user_instruction.instruction_valid = 1;
									interface->user_instruction.instant_instruction = 1;
									interface->user_instruction.zl_axis.instruction_valid = 1;
									interface->user_instruction.zl_axis.move_count = 1;
									interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
								}
								if(!strcmp(&interface->user_input[1], "[B")){
									// down arrow - down
									interface->user_instruction.instruction_valid = 1;
									interface->user_instruction.instant_instruction = 1;
									interface->user_instruction.zl_axis.instruction_valid = 1;
									interface->user_instruction.zl_axis.move_count = -1;
									interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
								}
							} else {
								if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
									configure_stage = 7;
									configure_processing = 0;
									interface->machine_marker = 0;
								}
							}
						}
						break;
					}
					case 7 : {
						// measure x and y axis as we go from min to max
						if(!configure_processing){
							printf("Moving X and Y to max to measure bed and level...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.opcode = MEASURE_AXIS;
							interface->user_instruction.xl_axis.instruction_valid = 1;
							interface->user_instruction.xl_axis.opcode = MEASURE_AXIS;
							interface->user_instruction.xl_axis.current_period = MOVE_PERIOD;
							interface->user_instruction.yf_axis.instruction_valid = 1;
							interface->user_instruction.yf_axis.opcode = MEASURE_AXIS;
							interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
						}
						if(interface->machine_marker){
							configure_stage = 8;
							configure_processing = 0;
							interface->machine_marker = 0;
							printf("Manually adjust ZR for new zero position...\n");
							printf("Press Enter When Finished\n");
						}
						break;
					}
					case 8 : {
						// manually adjust zr to zero position
						if(interface->user_command_set){
							interface->user_command_set = 0;
							if(interface->user_input[0] == 27){
								if(!strcmp(&interface->user_input[1], "[A")){
									// up arrow - up
									interface->user_instruction.instant_instruction = 1;
									interface->user_instruction.instruction_valid = 1;
									interface->user_instruction.zr_axis.instruction_valid = 1;
									interface->user_instruction.zr_axis.move_count = 1;
									interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
								}
								if(!strcmp(&interface->user_input[1], "[B")){
									// down arrow - down
									interface->user_instruction.instant_instruction = 1;
									interface->user_instruction.instruction_valid = 1;
									interface->user_instruction.zr_axis.instruction_valid = 1;
									interface->user_instruction.zr_axis.move_count = -1;
									interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
								}
							} else {
								if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
									configure_stage = 9;
									configure_processing = 0;
									interface->machine_marker = 0;
								}
							}
						}
						break;
					}
					case 9 : {
						// read cnc status to get zero positions and axis lengths
						if(configure_processing){
							if(interface->machine_marker){
								interface->machine_config.ramp_period = RAMP_PERIOD;
								interface->machine_config.xl_min_home_pos = 0;
								interface->machine_config.yf_min_home_pos = 0;
								interface->machine_config.zl_min_home_pos = 0;
								interface->machine_config.zr_min_home_pos = 0;
								interface->machine_config.zl_axis_size = -1*interface->machine_status.zl_position;
								interface->machine_config.zr_axis_size = -1*interface->machine_status.zr_position;
								interface->machine_config.xl_min_safe_pos = 0;
								interface->machine_config.yf_min_safe_pos = 0;
								interface->machine_config.zl_min_safe_pos = interface->machine_config.zl_min_safe_pos + interface->machine_config.zl_axis_size;
								interface->machine_config.zr_min_safe_pos = interface->machine_config.zr_min_safe_pos + interface->machine_config.zr_axis_size;
								interface->machine_config.x_axis_size = interface->machine_status.xl_position;
								interface->machine_config.y_axis_size = interface->machine_status.yf_position;
								interface->machine_marker = 0;
								interface->machine_config.config_loaded = 1;
								configure_processing = 0;
								configure_stage = 10;
							}
						} else {
							if(!interface->write_in_progress){
								interface->cnc_write_data[0] = (char) GET_CNC_STATUS;
								if(spi_set_write(interface->cnc_write_data, 1) > 0){
									configure_processing = 1;
									interface->write_in_progress = 1;
								} else {
									interface->write_in_progress = 0;
								}
							}
						}
						break;
					}
					case 10 : {
						// Set position of Zl and Zr to 0 to count to new home position
						if(!configure_processing){
							printf("Setting Zl and Zr to New 0...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.opcode = POSITION_AXIS;
							interface->user_instruction.zl_axis.opcode = POSITION_AXIS;
							interface->user_instruction.zl_axis.move_count = 0;
							interface->user_instruction.zl_axis.instruction_valid = 1;
							interface->user_instruction.zr_axis.opcode = POSITION_AXIS;
							interface->user_instruction.zr_axis.move_count = 0;
							interface->user_instruction.zr_axis.instruction_valid = 1;
						}
						if(interface->machine_marker){
							configure_stage = 11;
							configure_processing = 0;
							interface->machine_marker = 0;
						}
						break;
					}
					case 11 : {
						// Move Y to front to level plate
						if(!configure_processing){
							printf("Moving Y to front to level bed...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.yf_axis.instruction_valid = 1;
							interface->user_instruction.yf_axis.opcode = ZERO_MOTOR;
							interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
						}
						if(interface->machine_marker){
							printf("Press Enter When Finished Leveling\n");
							configure_stage = 12;
							configure_processing = 0;
							interface->machine_marker = 0;
						}
						break;
					}
					case 12 : {
						// wait for enter to move to other axis
						if(interface->user_command_set){
							interface->user_command_set = 0;
							if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
								configure_stage = 13;
								configure_processing = 0;
								interface->machine_marker = 0;
							}
						}
						break;
					}
					case 13 : {
						// measure x and y axis as we go from min to max
						if(!configure_processing){
							printf("Moving X to Left and Y to Front to Level Bed...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.xl_axis.instruction_valid = 1;
							interface->user_instruction.xl_axis.opcode = ZERO_MOTOR;
							interface->user_instruction.xl_axis.current_period = MOVE_PERIOD;
							interface->user_instruction.yf_axis.instruction_valid = 1;
							interface->user_instruction.yf_axis.opcode = MAX_MOTOR;
							interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
						}
						if(interface->machine_marker){
							printf("Press Enter When Finished Leveling\n");
							configure_stage = 14;
							configure_processing = 0;
							interface->machine_marker = 0;
						}
						break;
					}
					case 14 : {
						// wait for enter to move to other axis
						if(interface->user_command_set){
							interface->user_command_set = 0;
							if(interface->user_input[0] == 13 || interface->user_input[0] == 10){
								configure_stage = 15;
								configure_processing = 0;
								interface->machine_marker = 0;
							}
						}
						break;
					}
					case 15 : {
						// enable all axis motors
						if(!configure_processing){
							printf("Disabled Motors for Config...\n");
							interface->user_instruction.instruction_valid = 1;
							interface->user_instruction.instant_instruction = 1;
							interface->user_instruction.opcode = DISABLE_MOTORS;
							interface->user_instruction.xl_axis.pending_disable = 1;
							interface->user_instruction.xl_axis.opcode = DISABLE_MOTORS;
							interface->user_instruction.yf_axis.pending_disable = 1;
							interface->user_instruction.yf_axis.opcode = DISABLE_MOTORS;
							interface->user_instruction.zl_axis.pending_disable = 1;
							interface->user_instruction.zl_axis.opcode = DISABLE_MOTORS;
							interface->user_instruction.zr_axis.pending_disable = 1;
							interface->user_instruction.zr_axis.opcode = DISABLE_MOTORS;
							interface->user_instruction.extruder_0.pending_disable = 1;
							interface->user_instruction.extruder_0.opcode = DISABLE_MOTORS;
							interface->user_instruction.extruder_1.pending_disable = 1;
							interface->user_instruction.extruder_1.opcode = DISABLE_MOTORS;
							interface->user_instruction.aux.pending_disable = 1;
							interface->user_instruction.aux.opcode = DISABLE_MOTORS;
						}
						if(interface->machine_marker){
							configure_stage = 0;
							configure_processing = 0;
							interface->machine_marker = 0;
							interface->machine_state = SEND_CONFIG;
						}
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
					if(spi_set_write(interface->cnc_write_data, interface->cnc_write_length) > 0){
						configure_processing = 1;
						interface->write_in_progress = 1;
					} else {
						interface->write_in_progress = 0;
					}
				}
			}
			break;
		}
		case GET_PROGRAM : {
			socket_handler(&interface->user_command_set, interface->user_input);
			receive_user_input(interface);
			if(interface->user_command_finished){
				interface->machine_state = OPEN_PROGRAM;
				strcpy(interface->file_name, interface->user_command);
				clear_user_command(interface);
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
			printf("starting program, length: %llu\n", interface->program.program_length);
			interface->machine_state = SEND_INSTRUCTION;
			break;
		}
		case SEND_INSTRUCTION : {
			if(interface->write_in_progress){
				if(spi_check_write() > 0){
					interface->write_in_progress = 0;
					interface->machine_state = MACHINE_IDLE;
				}
			} else {
				// set with instruction
				if(interface->program.instruction_rp <= interface->program.program_length){
					if(interface->program.instruction[interface->program.instruction_rp].instruction_valid){
						interface->cnc_write_data[0] = (char) NEW_CNC_INSTRUCTION;
						interface->cnc_write_length = instruction_to_string(&interface->program.instruction[interface->program.instruction_rp],
																				&interface->cnc_write_data[1]);
						interface->cnc_write_length = interface->cnc_write_length + 1; // add one for 0 opcode
						interface->program.instruction_rp++;
						if(spi_set_write(interface->cnc_write_data, interface->cnc_write_length) > 0){
							interface->write_in_progress = 1;
							printf("Sent instruction!\n");
						} else {
							interface->write_in_progress = 0;
						}
					} else {
						// skip instruction that somehow isn't valid...
						interface->program.instruction_rp++;
					}
					interface->machine_state = SEND_INSTRUCTION;
				} else {
					printf("EXCEEDED PROGRAM LENGTH?!\n");
					interface->machine_state = MACHINE_IDLE;
				}
			}
			break;
		}
		case UPDATE_FIRMARE : {
			update_si_firmware(interface);
			interface->machine_state = MACHINE_IDLE;
			break;
		}
		default : {
			interface->machine_state = MACHINE_IDLE;
			break;
		}
	}
}

uint8_t check_spi(struct interface_struct* interface){
	// always check write to complete transfer if done
	//spi_check_write();
	// check if read is finished
	interface->cnc_read_length = spi_check_read(interface->cnc_read_data);
	if(interface->cnc_read_length == 0){
		// not reading, so set read ready
		spi_set_read();
		return 0;
	} else {
		if(interface->cnc_read_length > 0){
			// read finished, so process
			return 1;
		} else {
			return 0;
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
			interface->user_instruction.opcode = EMPTY_OPCODE;
			case 'a' : {
				// abort current instruction
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.opcode = ABORT_INSTRUCTION;
				break;
			}
			case 'e' : {
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.opcode = ENABLE_MOTORS;
				interface->user_instruction.xl_axis.pending_enable = 1;
				interface->user_instruction.xl_axis.opcode = ENABLE_MOTORS;
				interface->user_instruction.yf_axis.pending_enable = 1;
				interface->user_instruction.yf_axis.opcode = ENABLE_MOTORS;
				interface->user_instruction.zl_axis.pending_enable = 1;
				interface->user_instruction.zl_axis.opcode = ENABLE_MOTORS;
				interface->user_instruction.zr_axis.pending_enable = 1;
				interface->user_instruction.zr_axis.opcode = ENABLE_MOTORS;
				interface->user_instruction.extruder_0.pending_enable = 1;
				interface->user_instruction.extruder_0.opcode = ENABLE_MOTORS;
				interface->user_instruction.extruder_1.pending_enable = 1;
				interface->user_instruction.extruder_1.opcode = ENABLE_MOTORS;
				interface->user_instruction.aux.pending_enable = 1;
				interface->user_instruction.aux.opcode = ENABLE_MOTORS;
				break;
			}
			case 'd' : {
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.opcode = DISABLE_MOTORS;
				interface->user_instruction.xl_axis.pending_disable = 1;
				interface->user_instruction.xl_axis.opcode = DISABLE_MOTORS;
				interface->user_instruction.yf_axis.pending_disable = 1;
				interface->user_instruction.yf_axis.opcode = DISABLE_MOTORS;
				interface->user_instruction.zl_axis.pending_disable = 1;
				interface->user_instruction.zl_axis.opcode = DISABLE_MOTORS;
				interface->user_instruction.zr_axis.pending_disable = 1;
				interface->user_instruction.zr_axis.opcode = DISABLE_MOTORS;
				interface->user_instruction.extruder_0.pending_disable = 1;
				interface->user_instruction.extruder_0.opcode = DISABLE_MOTORS;
				interface->user_instruction.extruder_1.pending_disable = 1;
				interface->user_instruction.extruder_1.opcode = DISABLE_MOTORS;
				interface->user_instruction.aux.pending_disable = 1;
				interface->user_instruction.aux.opcode = DISABLE_MOTORS;
				break;
			}
			case 27 : { // escape character 
				if(!strcmp(&interface->user_input[1], "[A")){
					// up arrow - forward
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.yf_axis.instruction_valid = 1;
					interface->user_instruction.yf_axis.move_count = 10;
					interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[B")){
					// down arrow - backward
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.yf_axis.instruction_valid = 1;
					interface->user_instruction.yf_axis.move_count = -10;
					interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[C")){
					// right arrow - right
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.xl_axis.instruction_valid = 1;
					interface->user_instruction.xl_axis.move_count = 10;
					interface->user_instruction.xl_axis.current_period = MOVE_PERIOD;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[D")){
					// left arrow - left
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.xl_axis.instruction_valid = 1;
					interface->user_instruction.xl_axis.move_count = -10;
					interface->user_instruction.xl_axis.current_period = MOVE_PERIOD;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[1;2A")){
					// shift and up arrow - up
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.zl_axis.instruction_valid = 1;
					interface->user_instruction.zl_axis.move_count = 10;
					interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
					interface->user_instruction.zr_axis.instruction_valid = 1;
					interface->user_instruction.zr_axis.move_count = 10;
					interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
					break;
				}
				if(!strcmp(&interface->user_input[1], "[1;2B")){
					// shift and down arrow - down
					interface->user_instruction.instruction_valid = 1;
					interface->user_instruction.zl_axis.move_count = -10;
					interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
					interface->user_instruction.zl_axis.instruction_valid = 1;
					interface->user_instruction.zr_axis.move_count = -10;
					interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
					interface->user_instruction.zr_axis.instruction_valid = 1;
					break;
				}
				break;
			}
			case 'c' : {
				// config which will measure all axis lengths
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.opcode = MEASURE_AXIS;
				interface->user_instruction.xl_axis.opcode = MEASURE_AXIS;
				interface->user_instruction.xl_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.xl_axis.instruction_valid = 1;
				interface->user_instruction.yf_axis.opcode = MEASURE_AXIS;
				interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.yf_axis.instruction_valid = 1;
				interface->user_instruction.zl_axis.opcode = MEASURE_AXIS;
				interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.zl_axis.instruction_valid = 1;
				interface->user_instruction.zr_axis.opcode = MEASURE_AXIS;
				interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.zr_axis.instruction_valid = 1;
				break;
			}
			case 'z' : {
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.xl_axis.opcode = ZERO_MOTOR;
				interface->user_instruction.xl_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.xl_axis.instruction_valid = 1;
				interface->user_instruction.yf_axis.opcode = ZERO_MOTOR;
				interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.yf_axis.instruction_valid = 1;
				interface->user_instruction.zl_axis.opcode = ZERO_MOTOR;
				interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.zl_axis.instruction_valid = 1;
				interface->user_instruction.zr_axis.opcode = ZERO_MOTOR;
				interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.zr_axis.instruction_valid = 1;
				break;
			}
			case 'm' : {
				interface->user_instruction.instruction_valid = 1;
				interface->user_instruction.xl_axis.opcode = MAX_MOTOR;
				interface->user_instruction.xl_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.xl_axis.instruction_valid = 1;
				interface->user_instruction.yf_axis.opcode = MAX_MOTOR;
				interface->user_instruction.yf_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.yf_axis.instruction_valid = 1;
				interface->user_instruction.zl_axis.opcode = MAX_MOTOR;
				interface->user_instruction.zl_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.zl_axis.instruction_valid = 1;
				interface->user_instruction.zr_axis.opcode = MAX_MOTOR;
				interface->user_instruction.zr_axis.current_period = MOVE_PERIOD;
				interface->user_instruction.zr_axis.instruction_valid = 1;
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

void process_spi_request(struct interface_struct* interface){
	// setting machine state to default...
	// it will change in switch if needed
	//interface->machine_state = MACHINE_IDLE;
	switch((CNC_OPCODES) interface->cnc_read_data[0]){
		case GET_CNC_VERSION : {
			printf("CNC Version: %s\n", &interface->cnc_read_data[1]);
			break;
		}
		case GET_CNC_STATUS : {
			string_to_status(&interface->machine_status, &interface->cnc_read_data[1]);
			interface->machine_marker = 1;
			printf("CNC XL Min: %d\n", interface->machine_status.xl_min_flag);
			printf("CNC XL Max: %d\n", interface->machine_status.xl_max_flag);
			printf("CNC YF Min: %d\n", interface->machine_status.yf_min_flag);
			printf("CNC YF Max: %d\n", interface->machine_status.yf_max_flag);
			printf("CNC ZL Min: %d\n", interface->machine_status.zl_min_flag);
			printf("CNC ZL Max: %d\n", interface->machine_status.zl_max_flag);
			printf("CNC ZR Min: %d\n", interface->machine_status.zr_min_flag);
			printf("CNC ZR Max: %d\n", interface->machine_status.zr_max_flag);
			printf("CNC EX0 Pos: %lld\n", interface->machine_status.ex0_position);
			printf("CNC EX1 Pos: %lld\n", interface->machine_status.ex1_position);
			printf("CNC AUX Pos: %lld\n", interface->machine_status.aux_position);
			printf("CNC XL Pos: %lld\n", interface->machine_status.xl_position);
			printf("CNC YF Pos: %lld\n", interface->machine_status.yf_position);
			printf("CNC ZL Pos: %lld\n", interface->machine_status.zl_position);
			printf("CNC ZR Pos: %lld\n", interface->machine_status.zr_position);
			printf("CNC H0 Temp: %lf\n", interface->machine_status.heater_0_temp);
			printf("CNC H1 Temp: %lf\n", interface->machine_status.heater_1_temp);
			printf("CNC H2 Temp: %lf\n", interface->machine_status.heater_2_temp);
			printf("CNC H3 Temp: %lf\n", interface->machine_status.heater_3_temp);
			break;
		}
		case NEW_CNC_PRINT : {
			printf("CNC Printf: %s\n", &interface->cnc_read_data[1]);
			break;
		}
		case SET_CNC_CONFIG : {
			interface->machine_state = SEND_CONFIG;
			printf("Got Config Data Request\n");
			break;
		}
		case FLASH_FIRMWARE : {

			break;
		}
		case NEW_CNC_INSTRUCTION : {
			interface->machine_state = SEND_INSTRUCTION;
			//printf("Machine Requested Instruction\n");
			break;
		}
		case INSTANT_CNC_INSTRUCTION : {
			interface->machine_state = USER_CONTROL;
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
			printf("received machine marker!\n");
			interface->machine_marker = 1;
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
				interface->machine_state = USER_CONTROL;
				printf("Getting User Instruction!\n");
				break;
			}
			case 'u' : {
				interface->machine_state = UPDATE_FIRMARE;
				break;
			}
			case 'r' : {
				spi_reconnect();
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
				configure_stage = 0;
				configure_processing = 0;
				break;
			}
			case 'l' : {
				interface->machine_state = SEND_CONFIG;
				break;
			}
			case 's' : {
				printf("Config: X Min Home: %lld\n", interface->machine_config.xl_min_home_pos);
				printf("Config: X Min Safe: %lld\n", interface->machine_config.xl_min_safe_pos);
				printf("Config: Y Min Home: %lld\n", interface->machine_config.yf_min_home_pos);
				printf("Config: Y Min Safe: %lld\n", interface->machine_config.yf_min_safe_pos);
				printf("Config: Zl Min Home: %lld\n", interface->machine_config.zl_min_home_pos);
				printf("Config: Zl Min Safe: %lld\n", interface->machine_config.zl_min_safe_pos);
				printf("Config: Zr Min Home: %lld\n", interface->machine_config.zr_min_home_pos);
				printf("Config: Zr Min Safe: %lld\n", interface->machine_config.zr_min_safe_pos);
				printf("Config: X  Axis Length: %lld\n", interface->machine_config.x_axis_size);
				printf("Config: Y  Axis Length: %lld\n", interface->machine_config.y_axis_size);
				printf("Config: ZL  Axis Length: %lld\n", interface->machine_config.zl_axis_size);
				printf("Config: ZR  Axis Length: %lld\n", interface->machine_config.zr_axis_size);
				printf("Config: Ramp: %d\n", interface->machine_config.ramp_period);
				break;
			}
			default : {
				//spi_struct->pending_opcode = idle;
				interface->state = INTERFACE_RUNNING;
				break;
			}
		}
		clear_user_command(interface);
	}
}

void load_config_file(struct cnc_config_struct* config){
	FILE* config_file;
	char config_string[sizeof(cnc_config_struct)];
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
			config_string[count] = fgetc(config_file);
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
	char config_string[sizeof(cnc_config_struct)];
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
			fputc(config_string[count], config_file);
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

void opcode_to_string(enum spi_opcodes opcode, char* opcode_string){
	uint8_t count = 0;
	
	for(count=0;count<OPCODE_LENGTH;count++){
		opcode_string[count] = ((opcode >> (count*8)) & 0xFF);
	}
}

void update_si_firmware(struct interface_struct* interface){
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


// case HANDLE_SPI : {
// 				/*if(cnc.spi->state == spi_idle){
// 					cnc.state = IDLE;
// 				} else {
// 					//handle_interface_spi(cnc.spi);
// 				}*/
// 				break;
// 			}
// 			case PROCESS_INPUT : {
// 				/*if(cnc.spi->pending_opcode == idle){
// 					cnc.state = IDLE;
// 				} else {
// 					//handle_interface_spi(cnc.spi);
// 					cnc.state = PROCESS_INPUT;
// 				}*/
// 				break;
// 			}
// 			case POWER_MOTORS_ON : {
// 				//if(cnc.spi->pending_opcode == idle){
// 					enable_motors();
// 					cnc.motors_enabled = 1;
// 					cnc.state = IDLE;
// 				//} else {
// 					//handle_interface_spi(cnc.spi);
// 				//}
// 				break;
// 			}
// 			case POWER_MOTORS_OFF : {
// 				//if(cnc.spi->pending_opcode == idle){
// 					disable_motors();
// 					cnc.motors_enabled = 0;
// 					cnc.state = IDLE;
// 				//} else {
// 					//handle_interface_spi(cnc.spi);
// 				//}
// 				break;
// 			}
// 			case GET_PROGRAM : {
// 				if(command_set){
// 					socket_handler(&command_ready, system_command);
// 					if(command_ready){
// 						if(system_command[0] == 13 || system_command[0] == 10){
// 							// new line or CR so finish string
// 							control_string[string_counter] = '\0';
// 							string_counter = 0;
// 							command_set = 0;
// 							cnc.state = OPEN_PROGRAM;
// 						} else {
// 							if(system_command[0] == 8 || system_command[0] == 127){
// 								// backspace
// 								string_counter = (string_counter > 0) ? string_counter - 1 : 0;
// 								control_string[string_counter] = '\0';
// 							} else {
// 								control_string[string_counter] = system_command[0];
// 								string_counter++;
// 							}
// 						}
// 						command_ready = 0;
// 						printf("\rFile Name: %s", control_string);
// 					}
// 				} else {
// 					printf("Type Full Path to File and Press Enter:\n");
// 					printf("Typing q and enter will cancel\n");
// 					command_set = 1;
// 				}
// 				break;
// 			}
// 			case OPEN_PROGRAM : {
// 				if((control_string[0] == 'q' || control_string[0] == 'Q') && (control_string[1] == '\0')){
// 					printf("Going back to idle\n");
// 					cnc.state = IDLE;
// 				} else {
// 					if(open_gcode(&cnc, control_string) == 0){
// 						printf("File Opened\n");
// 						cnc.state = RUN_PROGRAM;
// 					} else {
// 						cnc.state = IDLE;
// 					}
// 				}
// 				break;
// 			}
// 			case RUN_PROGRAM : {
// 				printf("We would start the program here\n");
// 				cnc.state = IDLE;
// 				break;
// 			}
// 			case SEND_TO_MICRO : {
// 				if(command_set){
// 					socket_handler(&command_ready, system_command);
// 					if(command_ready){
// 						if(system_command[0] == 13 || system_command[0] == 10){
// 							// new line or CR so finish string
// 							control_string[string_counter] = '\0';
// 							string_counter = 0;
// 							command_set = 0;
// 							cnc.state = OPEN_PROGRAM;
// 						} else {
// 							if(system_command[0] == 8 || system_command[0] == 127){
// 								// backspace
// 								string_counter = (string_counter > 0) ? string_counter - 1 : 0;
// 								control_string[string_counter] = '\0';
// 							} else {
// 								control_string[string_counter] = system_command[0];
// 								string_counter++;
// 							}
// 						}
// 						command_ready = 0;
// 						printf("\rCommand: %s", control_string);
// 					}
// 				} else {
// 					printf("Enter Commands for Machine and press enter\n");
// 					printf("Typing q and enter will cancel\n");
// 					command_set = 1;
// 				}
// 				break;
// 			}
// 			case UPDATE_FIRMARE : {
// 				//update_si_firmware(spi);
// 				if(cnc.spi->state == spi_initialized){
// 					cnc.state = CONNECT_MICRO;
// 				} else {
// 					cnc.spi->connected = 0;
// 					//handle_interface_spi(cnc.spi);
// 				}
// 				break;
// 			}