/*
 * common_cnc.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "common_cnc.h"

uint16_t instruction_to_string(struct cnc_instruction_struct* instruction, char* string){
    uint16_t offset = 0;

    string[offset] = instruction->instruction_valid;
    offset++;
	string[offset] = instruction->instant_instruction;
    offset++;
	string[offset] = (char) instruction->opcode; // maps to instruction opcode enum
    offset++;
    string[offset] = instruction->set_position_flag;
    offset++;

    motor_instruction_to_string(&instruction->xl_axis, string, &offset);
    motor_instruction_to_string(&instruction->yf_axis, string, &offset);
    motor_instruction_to_string(&instruction->zl_axis, string, &offset);
    motor_instruction_to_string(&instruction->zr_axis, string, &offset);
    motor_instruction_to_string(&instruction->aux, string, &offset);
    motor_instruction_to_string(&instruction->extruder_0, string, &offset);
    motor_instruction_to_string(&instruction->extruder_1, string, &offset);
	
    heater_instruction_to_string(&instruction->heater_0, string, &offset);
    heater_instruction_to_string(&instruction->heater_1, string, &offset);
    heater_instruction_to_string(&instruction->heater_2, string, &offset);
    heater_instruction_to_string(&instruction->heater_3, string, &offset);
    
    return offset;
}
void string_to_instruction(struct cnc_instruction_struct* instruction, char* string){
    uint16_t offset = 0;

    instruction->instruction_valid = string[offset];
    offset++;
	instruction->instant_instruction = string[offset];
    offset++;
	instruction->opcode = (INSTRUCTION_OPCODE) string[offset]; // maps to instruction opcode enum
    offset++;
    instruction->set_position_flag = string[offset];
    offset++;

    string_to_motor_instruction(&instruction->xl_axis, string, &offset);
    string_to_motor_instruction(&instruction->yf_axis, string, &offset);
    string_to_motor_instruction(&instruction->zl_axis, string, &offset);
    string_to_motor_instruction(&instruction->zr_axis, string, &offset);
    string_to_motor_instruction(&instruction->aux, string, &offset);
    string_to_motor_instruction(&instruction->extruder_0, string, &offset);
    string_to_motor_instruction(&instruction->extruder_1, string, &offset);
	
    string_to_heater_instruction(&instruction->heater_0, string, &offset);
    string_to_heater_instruction(&instruction->heater_1, string, &offset);
    string_to_heater_instruction(&instruction->heater_2, string, &offset);
    string_to_heater_instruction(&instruction->heater_3, string, &offset);
}

void motor_instruction_to_string(struct cnc_motor_instruction_struct* instruction, char* string, uint16_t* offset){
    uint16_t local_offset;
    local_offset = *offset;
    string[local_offset] = instruction->instruction_valid;
    local_offset++;
    memcpy(&string[local_offset], &instruction->current_position, 4);
    local_offset = local_offset + 4;
    memcpy(&string[local_offset], &instruction->move_count, 4);
    local_offset = local_offset + 4;
    memcpy(&string[local_offset], &instruction->current_period, 4);
    local_offset = local_offset + 4;
    string[local_offset] = instruction->opcode;
    local_offset++;
    string[local_offset] = instruction->pending_enable;
    local_offset++;
    string[local_offset] = instruction->pending_disable;
    local_offset++;
    string[local_offset] = instruction->ramp_up_speed;
    local_offset++;
    string[local_offset] = instruction->ramp_down_speed;
    local_offset++;
    string[local_offset] = instruction->find_zero;
    local_offset++;
    string[local_offset] = instruction->find_max;
    local_offset++;
    string[local_offset] = instruction->linear_move;
    local_offset++;
    string[local_offset] = instruction->arc_move;
    local_offset++;
    *offset = local_offset;
}
void heater_instruction_to_string(struct cnc_heater_instruction_struct* instruction, char* string, uint16_t* offset){
    uint16_t local_offset;
    local_offset = *offset;
    string[local_offset] = instruction->instruction_valid;
    local_offset++;
    string[local_offset] = instruction->opcode;
    local_offset++;
    string[local_offset] = instruction->enabled;
    local_offset++;
    string[local_offset] = instruction->heater_on;
    local_offset++;
    string[local_offset] = instruction->heater_active;
    local_offset++;
    string[local_offset] = instruction->wait_for_temp;
    local_offset++;
    string[local_offset] = instruction->fan_duty;
    local_offset++;
    string[local_offset] = instruction->fan_on;
    local_offset++;
    string[local_offset] = instruction->temp_locked;
    local_offset++;
    memcpy(&string[local_offset], &instruction->target_temp, 8);
    local_offset = local_offset + 8;
    memcpy(&string[local_offset], &instruction->current_temp, 8);
    local_offset = local_offset + 8;
    *offset = local_offset;
}

void string_to_motor_instruction(struct cnc_motor_instruction_struct* instruction, char* string, uint16_t* offset){
    uint16_t local_offset;
    local_offset = *offset;
    instruction->instruction_valid = string[local_offset];
    local_offset++;
    memcpy(&instruction->current_position, &string[local_offset], 4);
    local_offset = local_offset + 4;
    memcpy(&instruction->move_count, &string[local_offset], 4);
    local_offset = local_offset + 4;
    memcpy(&instruction->current_period, &string[local_offset], 4);
    local_offset = local_offset + 4;
    instruction->opcode = (INSTRUCTION_OPCODE) string[local_offset];
    local_offset++;
    instruction->pending_enable = string[local_offset];
    local_offset++;
    instruction->pending_disable = string[local_offset];
    local_offset++;
    instruction->ramp_up_speed = string[local_offset];
    local_offset++;
    instruction->ramp_down_speed = string[local_offset];
    local_offset++;
    instruction->find_zero = string[local_offset];
    local_offset++;
    instruction->find_max = string[local_offset];
    local_offset++;
    instruction->linear_move = string[local_offset];
    local_offset++;
    instruction->arc_move = string[local_offset];
    local_offset++;
    *offset = local_offset;
}
void string_to_heater_instruction(struct cnc_heater_instruction_struct* instruction, char* string, uint16_t* offset){
    uint16_t local_offset;
    local_offset = *offset;
    instruction->instruction_valid = string[local_offset];
    local_offset++;
    instruction->opcode = (INSTRUCTION_OPCODE) string[local_offset];
    local_offset++;
    instruction->enabled = string[local_offset];
    local_offset++;
    instruction->heater_on = string[local_offset];
    local_offset++;
    instruction->heater_active = string[local_offset];
    local_offset++;
    instruction->wait_for_temp = string[local_offset];
    local_offset++;
    instruction->fan_duty = string[local_offset];
    local_offset++;
    instruction->fan_on = string[local_offset];
    local_offset++;
    instruction->temp_locked = string[local_offset];
    local_offset++;
    memcpy(&instruction->target_temp, &string[local_offset], 8);
    local_offset = local_offset + 8;
    memcpy(&instruction->current_temp, &string[local_offset], 8);
    local_offset = local_offset + 8;
    *offset = local_offset;
}

uint16_t config_to_string(struct cnc_config_struct* config, char* string){
    uint16_t offset = 0;
    string[offset] = config->valid_config;
    offset = offset + 1;
    memcpy(&string[offset], &config->max_speed, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->min_speed, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->xl_min_safe_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->xr_max_safe_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->yf_min_safe_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->yb_max_safe_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zl_min_safe_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zl_max_safe_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zr_min_safe_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zr_max_safe_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->xl_min_home_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->xr_max_home_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->yf_min_home_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->yb_max_home_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zl_min_home_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zl_max_home_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zr_min_home_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zr_max_home_pos, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->x_axis_size, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->y_axis_size, 4);
    offset = offset + 4;
	memcpy(&string[offset], &config->zr_axis_size, 4);
    offset = offset + 4;
    memcpy(&string[offset], &config->zl_axis_size, 4);
    return offset;
}
void string_to_config(struct cnc_config_struct* config, char* string){
    uint16_t offset = 0;
    config->valid_config = string[offset];
    offset = offset + 1;
    memcpy(&config->max_speed, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->min_speed, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->xl_min_safe_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->xr_max_safe_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->yf_min_safe_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->yb_max_safe_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zl_min_safe_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zl_max_safe_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zr_min_safe_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zr_max_safe_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->xl_min_home_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->xr_max_home_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->yf_min_home_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->yb_max_home_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zl_min_home_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zl_max_home_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zr_min_home_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zr_max_home_pos, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->x_axis_size, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->y_axis_size, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zl_axis_size, &string[offset], 4);
    offset = offset + 4;
	memcpy(&config->zr_axis_size, &string[offset], 4);
}

uint16_t status_to_string(struct cnc_status_struct* status, char* string){
    uint16_t offset = 0;

    string[offset] = status->xl_min_flag;
    offset = offset + 1;
	string[offset] = status->xl_max_flag;
    offset = offset + 1;
	string[offset] = status->yf_min_flag;
    offset = offset + 1;
	string[offset] = status->yf_max_flag;
    offset = offset + 1;
	string[offset] = status->zl_min_flag;
    offset = offset + 1;
	string[offset] = status->zl_max_flag;
    offset = offset + 1;
	string[offset] = status->zr_min_flag;
    offset = offset + 1;
	string[offset] = status->zr_max_flag;
    offset = offset + 1;
    memcpy(&string[offset], &status->ex0_position, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->ex1_position, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->aux_position, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->xl_position, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->yf_position, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->zl_position, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->zr_position, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->heater_0_temp, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->heater_1_temp, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->heater_2_temp, 4);
    offset = offset + 4;
	memcpy(&string[offset], &status->heater_3_temp, 4);
    offset = offset + 4;
    
    return offset;
}
void string_to_status(struct cnc_status_struct* status, char* string){
    uint16_t offset = 0;
    status->xl_min_flag = string[offset];
    offset = offset + 1;
	status->xl_max_flag = string[offset];
    offset = offset + 1;
	status->yf_min_flag = string[offset];
    offset = offset + 1;
	status->yf_max_flag = string[offset];
    offset = offset + 1;
	status->zl_min_flag = string[offset];
    offset = offset + 1;
	status->zl_max_flag = string[offset];
    offset = offset + 1;
	status->zr_min_flag = string[offset];
    offset = offset + 1;
	status->zr_max_flag = string[offset];
    offset = offset + 1;
    memcpy(&status->ex0_position, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->ex1_position, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->aux_position, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->xl_position, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->yf_position, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->zl_position, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->zr_position, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->heater_0_temp, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->heater_1_temp, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->heater_2_temp, &string[offset], 4);
    offset = offset + 4;
	memcpy(&status->heater_3_temp, &string[offset], 4);
    offset = offset + 4;
}

void clear_instruction(struct cnc_instruction_struct* instruction){
	instruction->instruction_valid = 0;
    instruction->instant_instruction = 0;
	instruction->opcode = EMPTY_OPCODE;
    instruction->set_position_flag = 0;
    instruction->instruction_set = 0;
	
    clear_motor_instruction(&instruction->xl_axis);
    clear_motor_instruction(&instruction->yf_axis);
    clear_motor_instruction(&instruction->zl_axis);
    clear_motor_instruction(&instruction->zr_axis);
    clear_motor_instruction(&instruction->extruder_0);
    clear_motor_instruction(&instruction->extruder_1);
    clear_motor_instruction(&instruction->aux);
    clear_heater_instruction(&instruction->heater_0);
    clear_heater_instruction(&instruction->heater_1);
    clear_heater_instruction(&instruction->heater_2);
    clear_heater_instruction(&instruction->heater_3);
}

void clear_motor_instruction(struct cnc_motor_instruction_struct* instruction){
    instruction->pending_enable = 0;
    instruction->pending_disable = 0;
    instruction->opcode = EMPTY_OPCODE;
    instruction->instruction_valid = 0; // flag saying whether instruction is valid or stale
	instruction->current_position = 0;
	instruction->move_count = 0;
	instruction->current_period = 0;
	instruction->ramp_up_speed = 0;
	instruction->ramp_down_speed = 0;
	instruction->find_zero = 0;
	instruction->find_max = 0;
	instruction->linear_move = 0;
	instruction->arc_move = 0;

}

void clear_heater_instruction(struct cnc_heater_instruction_struct* instruction){
    instruction->instruction_valid = 0;
	instruction->opcode = EMPTY_OPCODE;
	instruction->pending_enable = 0;
	instruction->pending_disable = 0;
	instruction->enabled = 0;
	instruction->heater_on = 0;
	instruction->heater_active = 0;
	instruction->wait_for_temp = 0;
	instruction->fan_duty = 0;
	instruction->fan_on = 0;
	instruction->temp_locked = 0;
	instruction->target_temp = 0;
	instruction->current_temp = 0;
}

