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

uint16_t instruction_to_string(struct cnc_instruction_struct* instruction, uint8_t* string){
   uint16_t offset = 0;
   
   // copy generic instruction stuff
   string[offset] = instruction->instruction_valid;
   offset++;
	string[offset] = instruction->instant_instruction;
   offset++;
   string[offset] = instruction->instruction_type;
   offset++;

   if(instruction->instruction_type == MOTOR_INSTRUCTION){
      memcpy(&string[offset], &instruction->instruction.motors.rotations, sizeof(instruction->instruction.motors.rotations));
      offset = offset + sizeof(instruction->instruction.motors.rotations);
      memcpy(&string[offset], &instruction->instruction.motors.speed, sizeof(instruction->instruction.motors.speed));
      offset = offset + sizeof(instruction->instruction.motors.speed);
      memcpy(&string[offset], &instruction->instruction.motors.ramp_speed, sizeof(instruction->instruction.motors.ramp_speed));
      offset = offset + sizeof(instruction->instruction.motors.ramp_speed);
      for(int i=0;i<NUM_MOTORS;i++){
         motor_instruction_to_string(&instruction->instruction.motors.motor[i], string, &offset);
      }
   }
   if(instruction->instruction_type == HEATER_INSTRUCTION){
      for(int i=0;i<NUM_HEATERS;i++){
         heater_instruction_to_string(&instruction->instruction.heaters.heater[i], string, &offset);
      }
   }
   if(instruction->instruction_type == AUX_INSTRUCTION){
      string[offset] = (uint8_t) instruction->instruction.aux.opcode;
      offset++;
      for(int i=0;i<NUM_MOTORS;i++){
         string[offset] = instruction->instruction.aux.enable_motor[i];
         offset++;
         string[offset] = instruction->instruction.aux.disable_motor[i];
         offset++;
         string[offset] = instruction->instruction.aux.set_position[i];
         offset++;
         string[offset] = instruction->instruction.aux.min_motor[i];
         offset++;
         string[offset] = instruction->instruction.aux.max_motor[i];
         offset++;
         memcpy(&string[offset], &instruction->instruction.aux.motor_position[i], sizeof(instruction->instruction.aux.motor_position[0]));
         offset = offset + sizeof(instruction->instruction.aux.motor_position[0]);
      }
      memcpy(&string[offset], &instruction->instruction.aux.motor_speed, sizeof(instruction->instruction.aux.motor_speed));
      offset = offset + sizeof(instruction->instruction.aux.motor_speed);
   }
   
   return offset;
}
void string_to_instruction(struct cnc_instruction_struct* instruction, uint8_t* string){
   uint16_t offset = 0;

   // copy generic instruction stuff
   instruction->instruction_valid = string[offset];
   offset++;
	instruction->instant_instruction = string[offset];
   offset++;
   instruction->instruction_type = (INSTRUCTION_TYPES) string[offset];
   offset++;

   if(instruction->instruction_type == MOTOR_INSTRUCTION){
      memcpy(&instruction->instruction.motors.rotations, &string[offset], sizeof(instruction->instruction.motors.rotations));
      offset = offset + sizeof(instruction->instruction.motors.rotations);
      memcpy(&instruction->instruction.motors.speed, &string[offset], sizeof(instruction->instruction.motors.speed));
      offset = offset + sizeof(instruction->instruction.motors.speed);
      memcpy(&instruction->instruction.motors.ramp_speed, &string[offset], sizeof(instruction->instruction.motors.ramp_speed));
      offset = offset + sizeof(instruction->instruction.motors.ramp_speed);
      for(int i=0;i<NUM_MOTORS;i++){
         string_to_motor_instruction(&instruction->instruction.motors.motor[i], string, &offset);
      }
   }
   if(instruction->instruction_type == HEATER_INSTRUCTION){
      for(int i=0;i<NUM_HEATERS;i++){
         string_to_heater_instruction(&instruction->instruction.heaters.heater[i], string, &offset);
      }
   }
   if(instruction->instruction_type == AUX_INSTRUCTION){
      instruction->instruction.aux.opcode = (INSTRUCTION_OPCODE) string[offset];
      offset++;
      for(int i=0;i<NUM_MOTORS;i++){
         instruction->instruction.aux.enable_motor[i] = string[offset];
         offset++;
         instruction->instruction.aux.disable_motor[i] = string[offset];
         offset++;
         instruction->instruction.aux.set_position[i] = string[offset];
         offset++;
         instruction->instruction.aux.min_motor[i] = string[offset];
         offset++;
         instruction->instruction.aux.max_motor[i] = string[offset];
         offset++;
         memcpy(&instruction->instruction.aux.motor_position[i], &string[offset], sizeof(instruction->instruction.aux.motor_position[0]));
         offset = offset + sizeof(instruction->instruction.aux.motor_position[0]);
      }
      memcpy(&instruction->instruction.aux.motor_speed, &string[offset], sizeof(instruction->instruction.aux.motor_speed));
      offset = offset + sizeof(instruction->instruction.aux.motor_speed);
   }
   
}

void motor_instruction_to_string(struct cnc_motor_instruction_struct* instruction, uint8_t* string, uint16_t* offset){
   uint16_t local_offset;
   local_offset = *offset;

   string[local_offset] = instruction->instruction_valid;
   local_offset++;
   string[local_offset] = instruction->relative_move;
   local_offset++;
   string[local_offset] = instruction->arc_move;
   local_offset++;
   memcpy(&string[local_offset], &instruction->end_position, sizeof(instruction->end_position));
   local_offset = local_offset + sizeof(instruction->end_position);
   string[local_offset] = instruction->ramp_down;
   local_offset++;
   string[local_offset] = instruction->ramp_up;
   local_offset++;

   *offset = local_offset;
}
void heater_instruction_to_string(struct cnc_heater_instruction_struct* instruction, uint8_t* string, uint16_t* offset){
   uint16_t local_offset;
   local_offset = *offset;
   
   string[local_offset] = instruction->disable_fan;
   local_offset++;
   string[local_offset] = instruction->disable_heater;
   local_offset++;
   string[local_offset] = instruction->enable_fan;
   local_offset++;
   string[local_offset] = instruction->enable_heater;
   local_offset++;
   string[local_offset] = instruction->fan_duty;
   local_offset++;
   string[local_offset] = instruction->instruction_valid;
   local_offset++;
   memcpy(&string[local_offset], &instruction->target_temp, sizeof(instruction->target_temp));
   local_offset = local_offset + sizeof(instruction->target_temp);
   string[local_offset] = instruction->wait_for_temp;
   local_offset++;

   *offset = local_offset;
}

void string_to_motor_instruction(struct cnc_motor_instruction_struct* instruction, uint8_t* string, uint16_t* offset){
   uint16_t local_offset;
   local_offset = *offset;
    
   instruction->instruction_valid = string[local_offset];
   local_offset++;
   instruction->relative_move = string[local_offset];
   local_offset++;
   instruction->arc_move = string[local_offset];
   local_offset++;
   memcpy(&instruction->end_position, &string[local_offset], sizeof(instruction->end_position));
   local_offset = local_offset + sizeof(instruction->end_position);
   instruction->ramp_down = string[local_offset];
   local_offset++;
   instruction->ramp_up = string[local_offset];
   local_offset++;

   *offset = local_offset;
}
void string_to_heater_instruction(struct cnc_heater_instruction_struct* instruction, uint8_t* string, uint16_t* offset){
   uint16_t local_offset;
   local_offset = *offset;
   
   instruction->disable_fan = string[local_offset];
   local_offset++;
   instruction->disable_heater = string[local_offset];
   local_offset++;
   instruction->enable_fan = string[local_offset];
   local_offset++;
   instruction->enable_heater = string[local_offset];
   local_offset++;
   instruction->fan_duty = string[local_offset];
   local_offset++;
   instruction->instruction_valid = string[local_offset];
   local_offset++;
   memcpy(&instruction->target_temp, &string[local_offset], sizeof(instruction->target_temp));
   local_offset = local_offset + sizeof(instruction->target_temp);
   instruction->wait_for_temp = string[local_offset];
   local_offset++;

   *offset = local_offset;
}

uint16_t config_to_string(struct cnc_config_struct* config, uint8_t* string){
   uint16_t offset = 0;

   string[offset] = config->config_loaded;
   offset = offset + 1;
	string[offset] = config->valid_config;
   offset = offset + 1;
	string[offset] = config->max_speed;
   offset = offset + 1;
	string[offset] = config->min_speed;
   offset = offset + 1;
	string[offset] = config->ramp_speed;
   offset = offset + 1;
   for(int i=0;i<NUM_MOTORS;i++){
      memcpy(&string[offset], &config->safe_position[i], sizeof(config->safe_position[0]));
      offset = offset + sizeof(config->safe_position[0]);
   }
   for(int i=0;i<NUM_MOTORS;i++){
      memcpy(&string[offset], &config->home_position[i], sizeof(config->home_position[0]));
      offset = offset + sizeof(config->home_position[0]);
   }
   for(int i=0;i<NUM_MOTORS;i++){
      memcpy(&string[offset], &config->axis_size[i], sizeof(config->axis_size[0]));
      offset = offset + sizeof(config->axis_size[0]);
   }

   return offset;
}
void string_to_config(struct cnc_config_struct* config, uint8_t* string){
   uint16_t offset = 0;

   config->config_loaded = string[offset];
   offset = offset + 1;
	config->valid_config = string[offset];
   offset = offset + 1;
	config->max_speed = string[offset];
   offset = offset + 1;
	config->min_speed = string[offset];
   offset = offset + 1;
	config->ramp_speed = string[offset];
   offset = offset + 1;
   for(int i=0;i<NUM_MOTORS;i++){
      memcpy(&config->safe_position[i], &string[offset], sizeof(config->safe_position[0]));
      offset = offset + sizeof(config->safe_position[0]);
   }
   for(int i=0;i<NUM_MOTORS;i++){
      memcpy(&config->home_position[i], &string[offset], sizeof(config->home_position[0]));
      offset = offset + sizeof(config->home_position[0]);
   }
   for(int i=0;i<NUM_MOTORS;i++){
      memcpy(&config->axis_size[i], &string[offset], sizeof(config->axis_size[0]));
      offset = offset + sizeof(config->axis_size[0]);
   }

}

uint16_t status_to_string(struct cnc_status_struct* status, uint8_t* string){
   uint16_t offset = 0;

   for(int i=0;i<NUM_ENDSTOPS;i++){
      string[offset] = status->endstop_status[i];
      offset = offset + 1;
   }
   for(int i=0;i<NUM_MOTORS;i++){
      memcpy(&string[offset], &status->position[i], sizeof(status->position[0]));
      offset = offset + sizeof(status->position[0]);
   }
   for(int i=0;i<NUM_HEATERS;i++){
      memcpy(&string[offset], &status->temp[i], sizeof(status->temp[0]));
      offset = offset + sizeof(status->temp[0]);
   }
   for(int i=0;i<NUM_HEATERS;i++){
      string[offset] = status->temp_locked[i];
      offset = offset + 1;
   }
   
   return offset;
}
void string_to_status(struct cnc_status_struct* status, uint8_t* string){
   uint16_t offset = 0;
   for(int i=0;i<NUM_ENDSTOPS;i++){
      status->endstop_status[i] = string[offset];
      offset = offset + 1;
   }
   for(int i=0;i<NUM_MOTORS;i++){
      memcpy(&status->position[i], &string[offset], sizeof(status->position[0]));
      offset = offset + sizeof(status->position[0]);
   }
   for(int i=0;i<NUM_HEATERS;i++){
      memcpy(&status->temp[i], &string[offset], sizeof(status->temp[0]));
      offset = offset + sizeof(status->temp[0]);
   }
   for(int i=0;i<NUM_HEATERS;i++){
      status->temp_locked[i] = string[offset];
      offset = offset + 1;
   }
}

void clear_instruction(struct cnc_instruction_struct* instruction){
   instruction->instruction_valid = 0;
   instruction->instant_instruction = 0;

   memset(&instruction->instruction, 0, sizeof(instruction->instruction));
}

void clear_motor_instruction(struct cnc_motor_instruction_struct* instruction){
   instruction->end_position = 0;
   instruction->instruction_valid = 0;
   instruction->relative_move = 0;
   instruction->ramp_down = 0;
   instruction->ramp_up = 0;
}

void clear_heater_instruction(struct cnc_heater_instruction_struct* instruction){
   instruction->fan_duty = 0;
   instruction->instruction_valid = 0;
   instruction->target_temp = 0;
   instruction->wait_for_temp = 0;
}

