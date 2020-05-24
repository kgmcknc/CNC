/*
 * To change this license header, choose License Headers in Project Properties.
 * To change this template file, choose Tools | Templates
 * and open the template in the editor.
 */

/* 
 * File:   gcode.h
 * Author: Kyle
 *
 * Created on December 22, 2018, 10:22 AM
 */

#ifndef SRC_GCODE_H
#define SRC_GCODE_H

#include "stdint.h"
#include "stdio.h"
#include "common_cnc.h"

#define MAX_GCODE_COMMENT 256
#define MAX_GCODE_MESSAGE 256

enum LINE_PARSE_STATE {
   BEGIN_LINE, CHECK_BLOCK_DELETE, GET_INSTRUCTION_NUMBER, GET_WORD, NEW_WORD, CONTINUE_WORD, START_COMMENT, HANDLE_COMMENT
};

enum NUMBER_PARSE_STATE {
   BEGIN_NUMBER, CHECK_SPACE, CHECK_SIGN, GET_INTEGER, GET_DECIMAL, SAVE_NUMBER
};

enum PROGRAM_UNITS {
   UNSET, INCHES, MILLIMETER
};

struct gcode_instruction_struct {
   enum INSTRUCTION_TYPES instruction_type;
   uint8_t instruction_valid;
   uint8_t instant_instruction;
   uint8_t instruction_set;
   uint64_t instruction_number;
   char comment[MAX_GCODE_COMMENT];
   char message[MAX_GCODE_MESSAGE];
   uint8_t comment_flag;
   uint8_t message_flag;
   struct cnc_motor_instructions motors;
   struct cnc_heater_instructions heaters;
   struct cnc_aux_instruction_struct aux;
};

struct gcode_program_struct {
   uint64_t program_length;
   uint64_t instruction_count;
   uint64_t instruction_wp;
   uint64_t instruction_rp;
   cnc_double feedrate;
   uint8_t movement_type;
   uint8_t units_in_inches;
   uint8_t absolute_move;
   uint8_t extruder_absolute_move;
   uint8_t extruder_select;
   uint8_t arc_move;
   uint8_t block_delete;
   uint8_t stop_switch;
   struct gcode_instruction_struct* instruction;
};

int parse_gcode_file(FILE* fp, struct gcode_program_struct* program);
int parse_gcode_line(char* line, struct gcode_program_struct* program);
int parse_number(char* line, uint16_t* line_count, cnc_double* value);
int parse_letter(char* line, uint16_t* line_count, char* value);
int parse_gcode_word(char* line, uint16_t* line_count, struct gcode_program_struct* program);
void clear_gcode_instruction(struct gcode_instruction_struct* instruction);
int check_gcode_instruction(struct gcode_instruction_struct* instruction);
int check_gcode_motor_instruction(struct cnc_motor_instruction_struct* instruction, cnc_double speed);
int check_gcode_extruder_instruction(struct cnc_motor_instruction_struct* instruction, cnc_double speed);
int check_gcode_heater_instruction(struct cnc_heater_instruction_struct* instruction);
uint16_t gcode_instruction_to_string(struct gcode_instruction_struct* instruction, uint8_t* string);

#endif /* SRC_GCODE_H */
