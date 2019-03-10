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

enum LINE_PARSE_STATE {
    BEGIN_LINE, CHECK_BLOCK_DELETE, GET_INSTRUCTION_NUMBER, GET_WORD, NEW_WORD, CONTINUE_WORD, START_COMMENT, HANDLE_COMMENT
};

enum NUMBER_PARSE_STATE {
    BEGIN_NUMBER, CHECK_SPACE, CHECK_SIGN, GET_INTEGER, GET_DECIMAL, SAVE_NUMBER
};

struct axis_movement_struct{
    char axis_active;
    char force_position;
    char linear_move;
    char arc_move;
    char infinite_move;
    double move_position;
    double current_position;
    double feed_rate;
};

struct heater_struct{
    char heater_active;
    char wait_for_temp;
    char fan_on;
    char fan_duty;
    double target_temp;
};

struct gcode_program_struct {
    uint64_t program_length;
    uint64_t instruction_count;
    char units_in_inches;
    char units_in_metric;
    char absolute_positions;
    char delta_positions;
    char arc_move_active;
    char linear_move_active;
    char block_delete;
    char stop_switch;
    struct cnc_instruction_struct* instruction;
};

int parse_gcode_file(FILE* fp, struct gcode_program_struct* program);
int parse_gcode_line(char* line, struct gcode_program_struct* program);
int parse_number(char* line, uint16_t* line_count, double* value);
int parse_letter(char* line, uint16_t* line_count, char* value);
int parse_gcode_word(char* line, uint16_t* line_count, struct gcode_program_struct* program);
int check_instruction(struct cnc_instruction_struct* instruction);

#endif /* SRC_GCODE_H */
