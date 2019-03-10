/*
 * cnc_instructions.h
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifndef CNC_INSTRUCTIONS_H_
#define CNC_INSTRUCTIONS_H_

#include "stdint.h"
#include "common_spi.h"
#include "cnc_functions.h"
#include "cnc_heaters.h"
#include "cnc_pid.h"
#include "common_cnc.h"

#define INSTRUCTION_FIFO_DEPTH 500
#define PRINT_DEPTH 16
#define MAX_PRINT_LENGTH 2048

extern struct cnc_instruction_struct cnc_instruction, cnc_instruction_next;
extern struct cnc_instruction_struct instruction_array[INSTRUCTION_FIFO_DEPTH];
extern uint32_t instruction_array_fullness, instruction_array_wp, instruction_array_rp;

void clear_instruction(struct cnc_instruction_struct* instruction);
void copy_instruction(struct cnc_instruction_struct* new_instruction, struct cnc_instruction_struct* current_instruction);
void check_motor_instruction(struct cnc_motor_instruction_struct* current_instruction, struct cnc_motor_struct* motor);
void check_heater_instruction(struct cnc_heater_instruction_struct* current_instruction, struct cnc_heater_struct* heater);
void check_instruction(struct cnc_instruction_struct* current_instruction);
void copy_motor_instruction(struct cnc_motor_instruction_struct* new_instruction, struct cnc_motor_instruction_struct* current_instruction);
void copy_heater_instruction(struct cnc_heater_instruction_struct* new_instruction, struct cnc_heater_instruction_struct* current_instruction);

#endif /* CNC_INSTRUCTIONS_H_ */
