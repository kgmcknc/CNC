/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

#include "stdio.h"
#include "stdint.h"
#include "string.h"
#include "stdlib.h"

#include "gcode.h"
#include "common_cnc.h"

int parse_gcode_file(FILE* gcode_fp, struct gcode_program_struct* program){
   int error = 0;
    uint64_t file_counter;
    uint64_t line_counter;
    uint64_t line_count;
    uint64_t file_size;
    int data;
    char line[4096];
    char* file_data;
    printf("starting parser\n");
    program->block_delete = 1;
    program->stop_switch = 0;
    program->program_length = 0;
    program->arc_move = 0;
    program->absolute_move = 0;
    program->extruder_absolute_move = 0;
    program->extruder_select = 0;
    program->instruction = 0;
    program->instruction_wp = 0;
    program->instruction_rp = 0;
    program->units_in_inches = 0;
    program->feedrate = 0.0;
    
    data = 0;
    file_counter = 0;
    while(data != EOF){
        file_counter++;
        data = getc(gcode_fp);
    }
    
    file_size = file_counter;
    fseek(gcode_fp, 0, SEEK_SET);
    
    file_data = (char*) malloc(file_size);
    if(file_data == NULL){
       printf("couldn't allocate data memory\n");
       exit(0);
    }
    
    file_counter = 0;
    while(file_counter < file_size){
        data = getc(gcode_fp);
        if((data >= 97) && (data <= 122)){
            data = data - 32; // convert to upper case
        }
        file_data[file_counter] = data;
        file_counter++;
    }
    
    line_counter = 0;
    line_count = 0;
    printf("file size: %lld\n", file_size);
    for(file_counter=0;file_counter < file_size; file_counter++){
         if((file_data[file_counter] >= 10) && (file_data[file_counter] <= 13)){ // look for end of line marker
            // end of line character - process line into command
            if(line_counter > 0){
                // process line
                line_count++;
            } else {
                // there were more than one character for end of line
            }
            line_counter = 0;
        } else {
            // not end of line character - store into line
            line_counter++;
        }
    }
    if(line_counter > 0){
       // finished program without new line... add a line for that instruction
       line_count++;
       line_counter = 0;
    }
    // add two instructions for start and end of program
    line_count = line_count + 2;
    printf("allocating %lld instructions\n", line_count);
    
    program->instruction = (struct gcode_instruction_struct*) malloc(line_count*sizeof(struct gcode_instruction_struct));
    if(program->instruction == NULL){
       printf("couldn't allocate program memory\n");
       exit(0);
    }

    line_counter = 0;
    line_count = 0;
    // first instruction is start program that we will set below
    clear_gcode_instruction(&program->instruction[0]);
    program->instruction_wp = program->instruction_wp + 1;
    program->program_length = 1;
    for(file_counter=0;file_counter < file_size; file_counter++){
        // read data into line
        if((file_data[file_counter] >= 10) && (file_data[file_counter] <= 13)){ // look for end of line marker
            // end of line character - process line into command
            if(line_counter > 0){
                // process line
                line[line_counter] = '\0';
                error = parse_gcode_line(line, program);
                if(error){
                   printf("error parsing line\n");
                   break;
                }
                line_count++;
            } else {
                // there were more than one character for end of line
            }
            line_counter = 0;
        } else {
            // not end of line character - store into line
            if(line_counter == 0){
                if(file_data[file_counter] == 0x20){ // space
                    // don't copy initial space to line data
                } else {
                    line[line_counter] = file_data[file_counter];
                }
            } else {
                line[line_counter] = file_data[file_counter];
            }
            line_counter++;
        }
    }
    // set start of program instruction
    program->instruction[0].instruction_type = AUX_INSTRUCTION;
    program->instruction[0].aux.opcode = PROGRAM_START;
    program->instruction[0].instruction_valid = 1;
    // set end of program instruction
    clear_gcode_instruction(&program->instruction[program->instruction_wp]);
    program->instruction[program->instruction_wp].instruction_type = AUX_INSTRUCTION;
    program->instruction[program->instruction_wp].aux.opcode = PROGRAM_END;
    program->instruction[program->instruction_wp].instruction_valid = 1;
    program->instruction_wp++;
    program->program_length++;
    printf("Finished Parsing Instructions\n");
    free(file_data);
    printf("File was %llu long\n", file_counter);
     return error;
}

void clear_gcode_instruction(struct gcode_instruction_struct* instruction){
   instruction->instruction_valid = 0;
   instruction->instant_instruction = 0;
   instruction->instruction_set = 0;
   instruction->instruction_number = 0;
   instruction->comment[0] = '\0';
   instruction->message[0] = '\0';
   instruction->comment_flag = 0;
   instruction->message_flag = 0;
   instruction->instruction_type = AUX_INSTRUCTION;
   instruction->aux.opcode = EMPTY_OPCODE;

   memset(instruction, 0, sizeof(gcode_instruction_struct));
}

int parse_gcode_line(char* line, struct gcode_program_struct* program){
   int error = 0;
    cnc_double temp_number = 0;
    uint16_t line_count = 0;
    uint16_t string_count = 0;
    char comment_brace_opened = 0;
    char comment_line_opened = 0;
    char message_flag = 0;
    char count_set = 0;
    char skip = 0;
    
    enum LINE_PARSE_STATE line_state = BEGIN_LINE;
    
    // all ascii letter input already converted to uppercase
    
    while((line[line_count] != '\0') && (error == 0) && (skip == 0)){
        
        if(!(comment_brace_opened || comment_line_opened) && line[line_count] == ')'){
            error = 1;
            break;
        }
        
        switch(line_state){
            case(BEGIN_LINE) : {
                // initialize new instruction
                string_count = 0;
                count_set = 0;
                clear_gcode_instruction(&program->instruction[program->instruction_wp]);
                program->instruction[program->instruction_wp].instruction_number = 0;
                program->instruction[program->instruction_wp].comment[0] = '\0';
                program->instruction[program->instruction_wp].message[0] = '\0';
                program->instruction[program->instruction_wp].comment_flag = 0;
                program->instruction[program->instruction_wp].message_flag = 0;
                program->instruction[program->instruction_wp].instruction_valid = 0;
                program->instruction[program->instruction_wp].motors.speed = program->feedrate;
                line_state = CHECK_BLOCK_DELETE;
                break;
            }
            case(CHECK_BLOCK_DELETE) : {
                if(line[line_count] == '/'){
                    if(program->block_delete){
                        // block delete high so skip this line
                        skip = 1;
                    } else {
                        // block delete low so process this line
                        skip = 0;
                    }
                    // skip over block delete character
                    printf("skipping block delete line\n");
                    line_count++;
                } else {
                    // no block delete character so process this line
                    skip = 0;
                }
                line_state = GET_INSTRUCTION_NUMBER;
                break;
            }
            case(GET_INSTRUCTION_NUMBER) : {
                if((line[line_count] == '(') || (line[line_count] == ';')){
                    // detected start of comment
                    line_state = START_COMMENT;
                } else {
                    // check for space or tab
                    if((line[line_count] == 32) || (line[line_count] == 9)){
                        // skip space
                        line_count++;
                    } else {
                        // check for instruction number
                        if(line[line_count] == 'N'){
                            line_count++;
                            error = parse_number(line, &line_count, &temp_number);
                            if(!error){
                              program->instruction[program->instruction_wp].instruction_number = (uint64_t) ((cnc_double) temp_number + 0.5);
                            } else {
                              count_set = 0;
                            }
                        } else {
                            // force a number
                            program->instruction[program->instruction_wp].instruction_number = program->instruction_wp;
                            count_set = 1;
                        }
                        line_state = GET_WORD;
                    }
                }
                break;
            }
            case(GET_WORD) : {
                if((line[line_count] == '(') || (line[line_count] == ';')){
                    // detected start of comment
                    line_state = START_COMMENT;
                } else {
                    // check for space or tab
                    if((line[line_count] == 32) || (line[line_count] == 9)){
                        // skip space
                        line_count++;
                    } else {
                        // call handle function
                        error = parse_gcode_word(line, &line_count, program);
                    }
                }
                break;
            }
            case(START_COMMENT) : {
                if(line[line_count] == '('){
                    comment_brace_opened = 1;
                    if(!strncmp(&line[line_count+1], "MSG", 3)){
                        message_flag = 1;
                        line_count = line_count + 3; // skip the MSG tag
                        program->instruction[program->instruction_wp].message_flag = 1;
                    } else {
                        comment_line_opened = 1;
                        line_count++;
                        program->instruction[program->instruction_wp].comment_flag = 1;
                    }
                }
                line_state = HANDLE_COMMENT;
                break;
            }
            case(HANDLE_COMMENT) : {
                if(line[line_count] == ')'){
                    comment_brace_opened = 0;
                    if(message_flag){
                        program->instruction[program->instruction_wp].message[string_count] = '\0';
                    } else {
                        program->instruction[program->instruction_wp].comment[string_count] = '\0';
                    }
                    // if count set, go onto finding new word
                    if(count_set){
                        // already set instruction number, so get next word
                        line_state = GET_WORD;
                    } else {
                        // No instruction number set, so try to find instruction number
                        line_state = GET_INSTRUCTION_NUMBER;
                    }
                } else {
                    if(message_flag){
                        program->instruction[program->instruction_wp].message[string_count] = line[line_count];
                    } else {
                        program->instruction[program->instruction_wp].comment[string_count] = line[line_count];
                    }
                    string_count++;
                }
                line_count++;
                break;
            }
            default : {
                line_state = BEGIN_LINE;
                break;
            }
        }
    }
    
    program->instruction[program->instruction_wp].message[string_count] = '\0';
    program->instruction[program->instruction_wp].comment[string_count] = '\0';
    
    check_gcode_instruction(&program->instruction[program->instruction_wp]);
    
    if(program->instruction[program->instruction_wp].instruction_valid){
        program->instruction_wp++;
        program->program_length++;
    }
    
    if(comment_brace_opened){
        error = 1;
    }
    
    return error;
}

int parse_number(char* line, uint16_t* line_count, cnc_double* value){
   int error;
    cnc_double number;
    cnc_double decimal;
    char negative;
    uint8_t temp;
    char number_set;
    char decimal_set;
    cnc_double decimal_divider = 10;
    char end_of_number = 0;
    
    enum NUMBER_PARSE_STATE number_state = BEGIN_NUMBER;
    
    while(!end_of_number){
        switch(number_state){
            case(BEGIN_NUMBER) : {
                end_of_number = 0;
                number_set = 0;
                temp = 0;
                decimal_divider = 10;
                negative = 0;
                decimal = 0;
                number = 0;
                error = 0;
                number_state = CHECK_SPACE;
                break;
            }
            case(CHECK_SPACE) : {
                // check for space or tab
                if((line[*line_count] == 32) || (line[*line_count] == 9)){
                    // if so skip past
                    *line_count = *line_count + 1;
                } else {
                    number_state = CHECK_SIGN;
                }
                break;
            }
            case(CHECK_SIGN) : {
                // check if positive or negative sign before number
                // positive by default if no negative sign set
                if((line[*line_count] == 43) || (line[*line_count] == 45)){
                    if(line[*line_count] == 45){
                       negative = 1; 
                    } else {
                        negative = 0;
                    }
                    *line_count = *line_count + 1;
                }
                number_state = GET_INTEGER;
                break;
            }
            case(GET_INTEGER) : {
                // check if outside ascii character range...
                if((line[*line_count] < 48) || (line[*line_count] > 57)){
                    if((line[*line_count] == 32) || (line[*line_count] == 9)){
                        // skip spaces
                        *line_count = *line_count + 1;
                    } else {
                        if(line[*line_count] == '.'){
                            *line_count = *line_count + 1;
                            if(number_set){
                                number_state = GET_DECIMAL;
                            } else {
                                error = 1;
                                number_state = SAVE_NUMBER;
                            }
                        } else {
                            number_state = SAVE_NUMBER;
                        }
                    }
                } else {
                    // handle integer part of number
                    number_set = 1;
                    temp = line[*line_count] - 48; // convert character to decimal
                    number = number*10 + temp;
                    *line_count = *line_count + 1;
                }
                break;
            }
            case(GET_DECIMAL) : {
                if((line[*line_count] < 48) || (line[*line_count] > 57)){
                    if((line[*line_count] == 32) || (line[*line_count] == 9)){
                        // skip spaces
                        *line_count = *line_count + 1;
                    } else {
                        if(line[*line_count] == '.'){
                            // can't get second decimal...
                            error = 1;
                        } else {
                            if(!decimal_set){
                                error = 1;
                            }
                        }
                        number_state = SAVE_NUMBER;
                    }
                } else {
                    // get decimal part of number
                    decimal_set = 1;
                    temp = line[*line_count] - 48; // convert character to decimal
                    decimal = decimal + temp/decimal_divider;
                    decimal_divider = decimal_divider*10;
                    *line_count = *line_count + 1;
                }
                break;
            }
            case(SAVE_NUMBER) : {
                if(error){
                    *value = 0;
                } else {
                    number = number + decimal;
                    if(negative){
                        number = -1*number;
                    }
                    *value = number;
                }
                end_of_number = 1;
                break;
            }
            default : {
                number_state = BEGIN_NUMBER;
                break;
            }
        }
    }
    
    return error;
}

int parse_letter(char* line, uint16_t* line_count, char* value){
    int error = 0;
    char found_letter = 0;
    uint8_t temp;
    
    while(!found_letter && !error){
        // check for space or tab
        if((line[*line_count] == 32) || (line[*line_count] == 9)){
            // if so skip past
            *line_count = *line_count + 1;
        } else {
            // check for letter
            if((line[*line_count] >= 65) || (line[*line_count] <= 90)){
                // found value inside ascii letter range
                *value = line[*line_count];
                found_letter = 1;
            } else {
                *value = 0;
                error = 1;
            }
            *line_count = *line_count + 1;
        }
    }
    
    return error;
}

int check_gcode_instruction(struct gcode_instruction_struct* instruction){
   int error = 0;
   char valid_instruction = 0;

    if(instruction->instruction_type == AUX_INSTRUCTION){
       switch(instruction->aux.opcode){
          case EMPTY_OPCODE : {
            valid_instruction = 0;
             break;
          }
          case SET_POSITION : {
             valid_instruction = 1;
             break;
          }
          case MOVE_TO_ENDSTOP : {
             instruction->aux.motor_speed = instruction->motors.speed;
               if(instruction->aux.motor_speed > 0.0){
                  valid_instruction = 1;
               } else {
                  valid_instruction = 0;
               }
             break;
          }
          default : {
             valid_instruction = 0;
             break;
          }
       }
    }

   if(instruction->instruction_type == MOTOR_INSTRUCTION){
      for(int i=0;i<NUM_MOTORS;i++){
         check_gcode_motor_instruction(&instruction->motors.motor[i], instruction->motors.speed);
         valid_instruction = valid_instruction | instruction->motors.motor[i].instruction_valid;
      }
      if(valid_instruction){
         //printf("valid motor instruction!\n");
      }
   }
   
   if(instruction->instruction_type == HEATER_INSTRUCTION){
      for(int i=0;i<NUM_HEATERS;i++){
         check_gcode_heater_instruction(&instruction->heaters.heater[i]);
         valid_instruction = valid_instruction | instruction->heaters.heater[i].instruction_valid;
      }
   }
   
   instruction->instruction_valid = valid_instruction;
   instruction->instruction_set = instruction->instruction_valid || instruction->message_flag;

   return error;
}

int check_gcode_motor_instruction(struct cnc_motor_instruction_struct* instruction, cnc_double speed){
   if(speed <= 0.0){
      if(instruction->instruction_valid){
         //printf("motor instruction not valid\n");
      }
      instruction->instruction_valid = 0;
   }
   return instruction->instruction_valid;
}

// don't remember why I wanted separate extrude check function...
int check_gcode_extruder_instruction(struct cnc_motor_instruction_struct* instruction, cnc_double speed){
   //  if(instruction->opcode != EMPTY_OPCODE){
   //      instruction->instruction_valid = 1;
   //      if(speed > 0.0){
   //          instruction->current_period = speed;
   //      }
   //  } else {
   //      if(instruction->move_count && (speed > 0.0)){
   //          instruction->instruction_valid = 1;
   //          if(speed > 0.0){
   //              instruction->current_period = speed;
   //          }
   //      } else {
   //          instruction->instruction_valid = 0;
   //      }
   //  }
    return instruction->instruction_valid;
}

int check_gcode_heater_instruction(struct cnc_heater_instruction_struct* instruction){
   if(instruction->target_temp){
      instruction->instruction_valid = 1;
   } else {
      instruction->instruction_valid = 0;
   }
   return instruction->instruction_valid;
}

int parse_gcode_word(char* line, uint16_t* line_count, struct gcode_program_struct* program){
   int error = 0;
    cnc_double code_number = 0;
    cnc_double temp_val = 0;
    char marker_letter = 0;
    
    // check for new word marker
    switch(line[*line_count]){
        case('A') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('B') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('C') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('D') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('E') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(program->units_in_inches){
                code_number = code_number * IN_TO_MM;
            }
            if(!error){
               if(program->extruder_select == 0){
                  if(program->instruction[program->instruction_wp].instruction_type == MOTOR_INSTRUCTION){
                     program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_0].instruction_valid = 1;
                     program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_0].arc_move = 0;
                     program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_0].end_position = code_number;
                     if(program->extruder_absolute_move){
                           program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_0].relative_move = 0;
                     } else {
                        program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_0].relative_move = 1;
                     }
                  } else {
                     if(program->instruction[program->instruction_wp].aux.opcode == SET_POSITION){
                        program->instruction[program->instruction_wp].aux.motor_position[MOTOR_EXTRUDER_0] = code_number;
                        program->instruction[program->instruction_wp].aux.set_position[MOTOR_EXTRUDER_0] = 1;
                     } else {
                        if(program->instruction[program->instruction_wp].aux.opcode == MOVE_TO_ENDSTOP){
                           //program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_0].end_position = 0.0;
                        }
                     }
                  }
               } else {
                  if(program->instruction[program->instruction_wp].instruction_type == MOTOR_INSTRUCTION){
                     program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_1].instruction_valid = 1;
                     program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_1].arc_move = 0;
                     program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_1].end_position = code_number;
                     if(program->extruder_absolute_move){
                           program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_1].relative_move = 0;
                     } else {
                        program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_1].relative_move = 1;
                     }
                  } else {
                     if(program->instruction[program->instruction_wp].aux.opcode == SET_POSITION){
                        program->instruction[program->instruction_wp].aux.motor_position[MOTOR_EXTRUDER_1] = code_number;
                        program->instruction[program->instruction_wp].aux.set_position[MOTOR_EXTRUDER_1] = 1;
                     } else {
                        if(program->instruction[program->instruction_wp].aux.opcode == MOVE_TO_ENDSTOP){
                           //program->instruction[program->instruction_wp].motors.motor[MOTOR_EXTRUDER_1].end_position = 0.0;
                        }
                     }
                  }
               }
            }
            // had else setting error if we weren't homing...
            break;
        }
        case('F') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
               if(program->units_in_inches){
                  // inches per minute
                  program->feedrate = code_number*IN_TO_MM/MINUTE_TO_SECOND;
                  program->instruction[program->instruction_wp].motors.speed = program->feedrate;
                  /*if(program->instruction[program->instruction_wp].motors.speed == 0){
                        program->instruction[program->instruction_wp].motors.speed = 1.0;
                  }*/
               } else {
                  // mm per minute
                  program->feedrate = code_number/MINUTE_TO_SECOND;
                  program->instruction[program->instruction_wp].motors.speed = program->feedrate;
                  /*if(program->instruction[program->instruction_wp].motors.speed == 0){
                     program->instruction[program->instruction_wp].motors.speed = 1;
                  }*/
               }
            }
            break;
        }
        case('G') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
                switch((int) code_number){
                    case(0) : {
                        // rapid move
                        program->arc_move = 0;
                        program->instruction[program->instruction_wp].instruction_type = MOTOR_INSTRUCTION;
                        program->instruction[program->instruction_wp].motors.speed = RAPID_MOVE_SPEED;
                        //printf("set rapid move\n");
                        //program->instruction[program->instruction_wp].xl_axis.move_count = 1000;
                        //program->instruction[program->instruction_wp].yf_axis.move_count = 1000;
                        //program->instruction[program->instruction_wp].zl_axis.move_count = 1000;
                        //program->instruction[program->instruction_wp].zr_axis.move_count = 1000;
                        //program->instruction[program->instruction_wp].xl_axis.current_period = MOVE_PERIOD;
                        //program->instruction[program->instruction_wp].yf_axis.current_period = MOVE_PERIOD;
                        //program->instruction[program->instruction_wp].zl_axis.current_period = MOVE_PERIOD;
                        //program->instruction[program->instruction_wp].zr_axis.current_period = MOVE_PERIOD;
                        break;
                    }
                    case(1) : {
                        // controlled move
                        program->instruction[program->instruction_wp].instruction_type = MOTOR_INSTRUCTION;
                        program->arc_move = 0;
                        //program->instruction[program->instruction_wp].xl_axis.move_count = 2000;
                        //program->instruction[program->instruction_wp].yf_axis.move_count = 2000;
                        //program->instruction[program->instruction_wp].zl_axis.move_count = 2000;
                        //program->instruction[program->instruction_wp].zr_axis.move_count = 2000;
                        //program->instruction[program->instruction_wp].xl_axis.current_period = 10;
                        //program->instruction[program->instruction_wp].yf_axis.current_period = 10;
                        //program->instruction[program->instruction_wp].zl_axis.current_period = 10;
                        //program->instruction[program->instruction_wp].zr_axis.current_period = 10;
                        break;
                    }
                    case(2) : {
                        program->arc_move = 1;
                        break;
                    }
                    case(3) : {
                        program->arc_move = 1;
                        break;
                    }
                    case(4) : {
                        // dwell
                        break;
                    }
                    case(6) : {
                        // direct stepper move
                        break;
                    }
                    case(10) : {
                        
                        break;
                    }
                    case(11) : {
                        
                        break;
                    }
                    case(12) : {
                        
                        break;
                    }
                    case(17) : {
                        
                        break;
                    }
                    case(18) : {
                        
                        break;
                    }
                    case(19) : {
                        
                        break;
                    }
                    case(20) : {
                        // inch system selection
                        printf("set units to inches\n");
                        program->units_in_inches = 1;
                        break;
                    }
                    case(21) : {
                        // metric system selection
                        printf("set units to mm\n");
                        program->units_in_inches = 0;
                        break;
                    }
                    case(22) : {
                        
                        break;
                    }
                    case(23) : {
                        
                        break;
                    }
                    case(26) : {
                        
                        break;
                    }
                    case(28) : {
                        // return to home
                        program->instruction[program->instruction_wp].instruction_type = AUX_INSTRUCTION;
                        program->instruction[program->instruction_wp].aux.opcode = MOVE_TO_ENDSTOP;
                        program->instruction[program->instruction_wp].motors.speed = RAPID_MOVE_SPEED;
                        break;
                    }
                    case(29) : {
                        if(code_number == 29.0){
                            
                        } else {
                            if(code_number == 29.1){
                                
                            } else {
                                if(code_number == 29.2){
                                    
                                } else {
                                    error = 1;
                                }
                            }
                        }
                        break;
                    }
                    case(30) : {
                        
                        break;
                    }
                    case(31) : {
                        
                        break;
                    }
                    case(32) : {
                        
                        break;
                    }
                    case(33) : {
                        
                        break;
                    }
                    case(34) : {
                        
                        break;
                    }
                    case(38) : {
                        if(code_number == 38.2){
                            
                        } else {
                            if(code_number == 38.3){
                                
                            } else {
                                if(code_number == 38.4){
                                    
                                } else {
                                    if(code_number == 38.5){
                                        
                                    } else {
                                        error = 1;
                                    }
                                }
                            }
                        }
                        break;
                    }
                    case(40) : {
                        
                        break;
                    }
                    case(42) : {
                        
                        break;
                    }
                    case(49) : {
                        
                        break;
                    }
                    case(53) : {
                        
                        break;
                    }
                    case(54) : {
                        
                        break;
                    }
                    case(55) : {
                        
                        break;
                    }
                    case(56) : {
                        
                        break;
                    }
                    case(57) : {
                        
                        break;
                    }
                    case(58) : {
                        
                        break;
                    }
                    case(59) : {
                        
                        break;
                    }
                    case(60) : {
                        
                        break;
                    }
                    case(61) : {
                        
                        break;
                    }
                    case(80) : {
                        
                        break;
                    }
                    case(90) : {
                        // absolute positioning mode
                        printf("set absolute move\n");
                        program->absolute_move = 1;
                        break;
                    }
                    case(91) : {
                        // relative positioning mode
                        printf("set relative move\n");
                        program->absolute_move = 0;
                        break;
                    }
                    case(92) : {
                        if(code_number == 92.0){
                            program->instruction[program->instruction_wp].instruction_type = AUX_INSTRUCTION;
                            program->instruction[program->instruction_wp].aux.opcode = SET_POSITION;
                        } else {
                            if(code_number == 92.1){
                                
                            } else {
                                if(code_number == 92.2){
                                    
                                } else {
                                    error = 1;
                                }
                            }
                        }
                        break;
                    }
                    case(93) : {
                        
                        break;
                    }
                    case(94) : {
                        
                        break;
                    }
                    case(100) : {
                        
                        break;
                    }
                    case(130) : {
                        
                        break;
                    }
                    case(131) : {
                        
                        break;
                    }
                    case(132) : {
                        
                        break;
                    }
                    case(133) : {
                        
                        break;
                    }
                    case(161) : {
                        
                        break;
                    }
                    case(162) : {
                        
                        break;
                    }
                    default : {
                        error = 1;
                        break;
                    }
                }
            }
            break;
        }
        case('H') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('I') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('J') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('K') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('L') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('M') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                switch((int) code_number){
                    case(0) : {
                        
                        break;
                    }
                    case(1) : {
                        
                        break;
                    }
                    case(2) : {
                        
                        break;
                    }
                    case(3) : {
                        
                        break;
                    }
                    case(4) : {
                        
                        break;
                    }
                    case(5) : {
                        
                        break;
                    }
                    case(6) : {
                        
                        break;
                    }
                    case(7) : {
                        
                        break;
                    }
                    case(8) : {
                        
                        break;
                    }
                    case(9) : {
                        
                        break;
                    }
                    case(10) : {
                        
                        break;
                    }
                    case(11) : {
                        
                        break;
                    }
                    case(17) : {
                        
                        break;
                    }
                    case(18) : {
                        
                        break;
                    }
                    case(20) :
                    case(21) :
                    case(22) :
                    case(23) :
                    case(24) :
                    case(25) :
                    case(26) :
                    case(27) :
                    case(28) :
                    case(29) :
                    case(30) :
                    case(31) :
                    case(32) :
                    case(33) :
                    case(34) :
                    case(35) :
                    case(36) :
                    case(37) :
                    case(38) :
                    case(39) :
                    case(40) :
                    case(41) :
                    case(42) :
                    case(43) : {
                        // can't handle sd card
                        error = 1;
                        break;
                    }
                    case(48) : {
                        
                        break;
                    }
                    case(70) : {
                        
                        break;
                    }
                    case(72) : {
                        
                        break;
                    }
                    case(73) : {
                        
                        break;
                    }
                    case(80) : {
                        
                        break;
                    }
                    case(81) : {
                        
                        break;
                    }
                    case(82) : {
                        // absolute positioning mode
                        program->extruder_absolute_move = 1;
                        break;
                    }
                    case(83) : {
                        // relative positioning mode
                        program->extruder_absolute_move = 0;
                        break;
                    }
                    case(84) : {
                        
                        break;
                    }
                    case(85) : {
                        
                        break;
                    }
                    case(92) : {
                        
                        break;
                    }
                    case(93) : {
                        
                        break;
                    }
                    case(98) : {
                        
                        break;
                    }
                    case(99) : {
                        
                        break;
                    }
                    case(101) : {
                        
                        break;
                    }
                    case(102) : {
                        
                        break;
                    }
                    case(103) : {
                        
                        break;
                    }
                    case(104) : {
                        // set extruder temp
                        // get snnn.nnn
                        error = parse_letter(line, line_count, &marker_letter);
                        if(!error){
                            error = parse_number(line, line_count, &temp_val);
                            if(!error){
                               program->instruction[program->instruction_wp].instruction_type = HEATER_INSTRUCTION;
                                program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].enable_heater = 1;
                                program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].target_temp = temp_val;
                                program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].wait_for_temp = 0;
                            }
                        }
                        break;
                    }
                    case(105) : {
                        // get extruder temp
                        
                        break;
                    }
                    case(106) : {
                        // fan on
                        // LOTS of settings
                        // error = parse_letter(line, line_count, &marker_letter);
                        // if(!error){
                        //     error = parse_number(line, line_count, &temp_val);
                        //     if(!error){
                        //        program->instruction[program->instruction_wp].instruction_type = HEATER_INSTRUCTION;
                        //         program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].enable_fan = 1;
                        //         program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].fan_duty = 100;
                        //     }
                        // }
                        break;
                    }
                    case(107) : {
                        // fan off
                        // error = parse_letter(line, line_count, &marker_letter);
                        // if(!error){
                        //     error = parse_number(line, line_count, &temp_val);
                        //     if(!error){
                        //        program->instruction[program->instruction_wp].instruction_type = HEATER_INSTRUCTION;
                        //         program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].disable_heater = 0;
                        //         program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].fan_duty = 100;
                        //     }
                        // }
                        break;
                    }
                    case(108) : {
                        // set extruder speed bfb
                        // cancel heating marlin
                        break;
                    }
                    case(109) : {
                        // set extruder temp and wait
                        error = parse_letter(line, line_count, &marker_letter);
                        if(!error){
                            error = parse_number(line, line_count, &temp_val);
                            if(!error){
                               program->instruction[program->instruction_wp].instruction_type = HEATER_INSTRUCTION;
                                program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].enable_heater = 1;
                                program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].target_temp = temp_val;
                                program->instruction[program->instruction_wp].heaters.heater[program->extruder_select].wait_for_temp = 1;
                            }
                        }
                        break;
                    }
                    case(110) : {
                        
                        break;
                    }
                    case(111) : {
                        
                        break;
                    }
                    case(112) : {
                        // emergency stop
                        break;
                    }
                    case(113) : {
                        
                        break;
                    }
                    case(114) : {
                        
                        break;
                    }
                    case(115) : {
                        
                        break;
                    }
                    case(116) : {
                        
                        break;
                    }
                    case(117) : {
                        
                        break;
                    }
                    case(118) : {
                        
                        break;
                    }
                    case(119) : {
                        
                        break;
                    }
                    case(120) : {
                        
                        break;
                    }
                    case(121) : {
                        
                        break;
                    }
                    case(122) : {
                        
                        break;
                    }
                    case(123) : {
                        
                        break;
                    }
                    case(124) : {
                        
                        break;
                    }
                    case(126) : {
                        
                        break;
                    }
                    case(127) : {
                        
                        break;
                    }
                    case(128) : {
                        
                        break;
                    }
                    case(129) : {
                        
                        break;
                    }
                    case(130) : {
                        
                        break;
                    }
                    case(131) : {
                        
                        break;
                    }
                    case(132) : {
                        
                        break;
                    }
                    case(133) : {
                        
                        break;
                    }
                    case(134) : {
                        
                        break;
                    }
                    case(135) : {
                        
                        break;
                    }
                    case(136) : {
                        
                        break;
                    }
                    case(140) : {
                        
                        break;
                    }
                    case(141) : {
                        
                        break;
                    }
                    case(142) : {
                        
                        break;
                    }
                    case(143) : {
                        
                        break;
                    }
                    case(144) : {
                        
                        break;
                    }
                    case(146) : {
                        
                        break;
                    }
                    case(149) : {
                        
                        break;
                    }
                    case(150) : {
                        
                        break;
                    }
                    case(155) : {
                        
                        break;
                    }
                    case(160) : {
                        
                        break;
                    }
                    case(163) : {
                        
                        break;
                    }
                    case(164) : {
                        
                        break;
                    }
                    case(165) : {
                        
                        break;
                    }
                    case(190) : {
                        
                        break;
                    }
                    case(191) : {
                        
                        break;
                    }
                    case(200) : {
                        
                        break;
                    }
                    case(201) : {
                        
                        break;
                    }
                    case(202) : {
                        
                        break;
                    }
                    case(203) : {
                        
                        break;
                    }
                    case(204) : {
                        
                        break;
                    }
                    case(205) : {
                        
                        break;
                    }
                    case(206) : {
                        
                        break;
                    }
                    case(207) : {
                        
                        break;
                    }
                    case(208) : {
                        
                        break;
                    }
                    case(209) : {
                        
                        break;
                    }
                    case(210) : {
                        
                        break;
                    }
                    case(211) : {
                        
                        break;
                    }
                    default : {
                        error = 1;
                        break;
                    }
                }
            }
            break;
        }
        // case('N') : shouldn't get second N on line!! error
        case('P') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('Q') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('R') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('S') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('T') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(!error){
                // process command
            }
            break;
        }
        case('X') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            //printf("Got X: %f\n", code_number);
            if(program->units_in_inches){
                code_number = code_number * IN_TO_MM;
            }
            if(program->instruction[program->instruction_wp].instruction_type == MOTOR_INSTRUCTION){
                if(!error){
                   program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_XL].instruction_valid = 1;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_XL].arc_move = 0;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_XL].end_position = code_number;
                    if(program->absolute_move){
                       //printf("absolute x move\n");
                       program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_XL].relative_move = 0;
                    } else {
                       //printf("relative x move\n");
                       program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_XL].relative_move = 1;
                    }
                }
            } else {
               if(program->instruction[program->instruction_wp].aux.opcode == SET_POSITION){
                  program->instruction[program->instruction_wp].aux.motor_position[MOTOR_AXIS_XL] = code_number;
                  program->instruction[program->instruction_wp].aux.set_position[MOTOR_AXIS_XL] = 1;
               } else {
                     if(program->instruction[program->instruction_wp].aux.opcode == MOVE_TO_ENDSTOP){
                        program->instruction[program->instruction_wp].aux.min_motor[MOTOR_AXIS_XL] = 1;
                     }
                }
            }
            break;
        }
        case('Y') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(program->units_in_inches){
                code_number = code_number * IN_TO_MM;
            }
            if(program->instruction[program->instruction_wp].instruction_type == MOTOR_INSTRUCTION){
                if(!error){
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_YF].instruction_valid = 1;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_YF].arc_move = 0;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_YF].end_position = code_number;
                    if(program->absolute_move){
                       program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_YF].relative_move = 0;
                    } else {
                       program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_YF].relative_move = 1;
                    }
                }
            } else {
               if(program->instruction[program->instruction_wp].aux.opcode == SET_POSITION){
                  program->instruction[program->instruction_wp].aux.motor_position[MOTOR_AXIS_YF] = code_number;
                  program->instruction[program->instruction_wp].aux.set_position[MOTOR_AXIS_YF] = 1;
               } else {
                     if(program->instruction[program->instruction_wp].aux.opcode == MOVE_TO_ENDSTOP){
                        program->instruction[program->instruction_wp].aux.min_motor[MOTOR_AXIS_YF] = 1;
                     }
                }
            }
            break;
        }
        case('Z') : {
            *line_count = *line_count + 1;
            error = parse_number(line, line_count, &code_number);
            if(program->units_in_inches){
                code_number = code_number * IN_TO_MM;
            }
            if(program->instruction[program->instruction_wp].instruction_type == MOTOR_INSTRUCTION){
                if(!error){
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZL].instruction_valid = 1;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZR].instruction_valid = 1;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZL].arc_move = 0;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZR].arc_move = 0;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZL].end_position = code_number;
                    program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZR].end_position = code_number;
                    if(program->absolute_move){
                       program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZL].relative_move = 0;
                       program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZR].relative_move = 0;
                    } else {
                       program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZL].relative_move = 1;
                       program->instruction[program->instruction_wp].motors.motor[MOTOR_AXIS_ZR].relative_move = 1;
                    }
                }
            } else {
               if(program->instruction[program->instruction_wp].aux.opcode == SET_POSITION){
                  program->instruction[program->instruction_wp].aux.motor_position[MOTOR_AXIS_ZL] = code_number;
                  program->instruction[program->instruction_wp].aux.set_position[MOTOR_AXIS_ZL] = 1;
                  program->instruction[program->instruction_wp].aux.motor_position[MOTOR_AXIS_ZR] = code_number;
                  program->instruction[program->instruction_wp].aux.set_position[MOTOR_AXIS_ZR] = 1;
               } else {
                     if(program->instruction[program->instruction_wp].aux.opcode == MOVE_TO_ENDSTOP){
                        program->instruction[program->instruction_wp].aux.min_motor[MOTOR_AXIS_ZL] = 1;
                        program->instruction[program->instruction_wp].aux.min_motor[MOTOR_AXIS_ZR] = 1;
                     }
                }
            }
            break;
        }
        default : {
            error = 1;
            break;
        }
    }
    
    return error;
}

uint16_t gcode_instruction_to_string(struct gcode_instruction_struct* instruction, uint8_t* string){
   uint16_t offset = 0;
   
   // copy generic instruction stuff
   string[offset] = instruction->instruction_valid;
   offset++;
	string[offset] = instruction->instant_instruction;
   offset++;
   string[offset] = instruction->instruction_type;
   offset++;

   if(instruction->instruction_type == MOTOR_INSTRUCTION){
      memcpy(&string[offset], &instruction->motors.rotations, sizeof(instruction->motors.rotations));
      offset = offset + sizeof(instruction->motors.rotations);
      memcpy(&string[offset], &instruction->motors.speed, sizeof(instruction->motors.speed));
      offset = offset + sizeof(instruction->motors.speed);
      memcpy(&string[offset], &instruction->motors.ramp_speed, sizeof(instruction->motors.ramp_speed));
      offset = offset + sizeof(instruction->motors.ramp_speed);
      for(int i=0;i<NUM_MOTORS;i++){
         motor_instruction_to_string(&instruction->motors.motor[i], string, &offset);
      }
   }
   if(instruction->instruction_type == HEATER_INSTRUCTION){
      for(int i=0;i<NUM_HEATERS;i++){
         heater_instruction_to_string(&instruction->heaters.heater[i], string, &offset);
      }
   }
   if(instruction->instruction_type == AUX_INSTRUCTION){
      string[offset] = (uint8_t) instruction->aux.opcode;
      offset++;
      for(int i=0;i<NUM_MOTORS;i++){
         string[offset] = instruction->aux.enable_motor[i];
         offset++;
         string[offset] = instruction->aux.disable_motor[i];
         offset++;
         string[offset] = instruction->aux.set_position[i];
         offset++;
         string[offset] = instruction->aux.min_motor[i];
         offset++;
         string[offset] = instruction->aux.max_motor[i];
         offset++;
         memcpy(&string[offset], &instruction->aux.motor_position[i], sizeof(instruction->aux.motor_position[0]));
         offset = offset + sizeof(instruction->aux.motor_position[0]);
      }
      memcpy(&string[offset], &instruction->aux.motor_speed, sizeof(instruction->aux.motor_speed));
      offset = offset + sizeof(instruction->aux.motor_speed);
   }
   
   return offset;
}