/*
 * cnc_gpio.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifdef SILABS
   #include "em_msc.h"
#else
   #include "Arduino.h"
#endif

#include "common_cnc.h"
#include "cnc_gpio.h"
#include "main.h"
#include "stdint.h"

void init_gpio(void){
   #ifdef SILABS
      //GPIO_PinModeSet(SPI_REQUEST_PORT, SPI_REQUEST_PIN, gpioModePushPull, 0);
      //GPIO_PinModeSet(SPI_RESET_PORT, SPI_RESET_PIN, gpioModeInputPull, 1);

      GPIO_PinModeSet(ENDSTOP_0_PORT, ENDSTOP_0_PIN, gpioModeInputPull, 0);
      GPIO_PinModeSet(ENDSTOP_1_PORT, ENDSTOP_1_PIN, gpioModeInputPull, 0);
      GPIO_PinModeSet(ENDSTOP_2_PORT, ENDSTOP_2_PIN, gpioModeInputPull, 0);
      GPIO_PinModeSet(ENDSTOP_3_PORT, ENDSTOP_3_PIN, gpioModeInputPull, 0);
      GPIO_PinModeSet(ENDSTOP_4_PORT, ENDSTOP_4_PIN, gpioModeInputPull, 0);
      GPIO_PinModeSet(ENDSTOP_5_PORT, ENDSTOP_5_PIN, gpioModeInputPull, 0);
      GPIO_PinModeSet(ENDSTOP_6_PORT, ENDSTOP_6_PIN, gpioModeInputPull, 0);
      GPIO_PinModeSet(ENDSTOP_7_PORT, ENDSTOP_7_PIN, gpioModeInputPull, 0);

      GPIO_PinModeSet(TOOL_0_EN_PORT, TOOL_0_EN_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(TOOL_1_EN_PORT, TOOL_1_EN_PIN, gpioModePushPull, 0);

      GPIO_PinModeSet(TEMP_ADC_0_PORT, TEMP_ADC_0_PIN, gpioModeDisabled, 0);
      GPIO_PinModeSet(TEMP_ADC_1_PORT, TEMP_ADC_1_PIN, gpioModeDisabled, 0);
      GPIO_PinModeSet(TEMP_ADC_2_PORT, TEMP_ADC_2_PIN, gpioModeDisabled, 0);
      GPIO_PinModeSet(TEMP_ADC_3_PORT, TEMP_ADC_3_PIN, gpioModeDisabled, 0);

      GPIO_PinModeSet(HEAT_0_PORT, HEAT_0_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(HEAT_1_PORT, HEAT_1_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(HEAT_2_PORT, HEAT_2_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(HEAT_3_PORT, HEAT_3_PIN, gpioModePushPull, 0);

      GPIO_PinModeSet(MOTOR_0_ENABLE_PORT, MOTOR_0_ENABLE_PIN, gpioModePushPull, 1);
      GPIO_PinModeSet(MOTOR_0_DIR_PORT, MOTOR_0_DIR_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_0_STEP_PORT, MOTOR_0_STEP_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_0_MS0_PORT, MOTOR_0_MS0_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_0_MS1_PORT, MOTOR_0_MS1_PIN, gpioModePushPull, 0);

      GPIO_PinModeSet(MOTOR_1_ENABLE_PORT, MOTOR_1_ENABLE_PIN, gpioModePushPull, 1);
      GPIO_PinModeSet(MOTOR_1_DIR_PORT, MOTOR_1_DIR_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_1_STEP_PORT, MOTOR_1_STEP_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_1_MS0_PORT, MOTOR_1_MS0_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_1_MS1_PORT, MOTOR_1_MS1_PIN, gpioModePushPull, 0);

      GPIO_PinModeSet(MOTOR_2_ENABLE_PORT, MOTOR_2_ENABLE_PIN, gpioModePushPull, 1);
      GPIO_PinModeSet(MOTOR_2_DIR_PORT, MOTOR_2_DIR_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_2_STEP_PORT, MOTOR_2_STEP_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_2_MS0_PORT, MOTOR_2_MS0_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_2_MS1_PORT, MOTOR_2_MS1_PIN, gpioModePushPull, 0);

      GPIO_PinModeSet(MOTOR_3_ENABLE_PORT, MOTOR_3_ENABLE_PIN, gpioModePushPull, 1);
      GPIO_PinModeSet(MOTOR_3_DIR_PORT, MOTOR_3_DIR_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_3_STEP_PORT, MOTOR_3_STEP_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_3_MS0_PORT, MOTOR_3_MS0_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_3_MS1_PORT, MOTOR_3_MS1_PIN, gpioModePushPull, 0);

      GPIO_PinModeSet(MOTOR_4_ENABLE_PORT, MOTOR_4_ENABLE_PIN, gpioModePushPull, 1);
      GPIO_PinModeSet(MOTOR_4_DIR_PORT, MOTOR_4_DIR_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_4_STEP_PORT, MOTOR_4_STEP_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_4_MS0_PORT, MOTOR_4_MS0_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_4_MS1_PORT, MOTOR_4_MS1_PIN, gpioModePushPull, 0);

      GPIO_PinModeSet(MOTOR_5_ENABLE_PORT, MOTOR_5_ENABLE_PIN, gpioModePushPull, 1);
      GPIO_PinModeSet(MOTOR_5_DIR_PORT, MOTOR_5_DIR_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_5_STEP_PORT, MOTOR_5_STEP_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_5_MS0_PORT, MOTOR_5_MS0_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_5_MS1_PORT, MOTOR_5_MS1_PIN, gpioModePushPull, 0);

      GPIO_PinModeSet(MOTOR_6_ENABLE_PORT, MOTOR_6_ENABLE_PIN, gpioModePushPull, 1);
      GPIO_PinModeSet(MOTOR_6_DIR_PORT, MOTOR_6_DIR_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_6_STEP_PORT, MOTOR_6_STEP_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_6_MS0_PORT, MOTOR_6_MS0_PIN, gpioModePushPull, 0);
      GPIO_PinModeSet(MOTOR_6_MS1_PORT, MOTOR_6_MS1_PIN, gpioModePushPull, 0);
   #else

      pinMode(ENDSTOP_0_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_1_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_2_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_3_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_4_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_5_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_6_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_7_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_8_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_9_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_10_PIN, INPUT_PULLUP);
      pinMode(ENDSTOP_11_PIN, INPUT_PULLUP);

      pinMode(TOOL_0_EN_PIN, OUTPUT);
      pinMode(TOOL_1_EN_PIN, OUTPUT);

      //pinMode(TEMP_ADC_0_PIN, gpioModeDisabled);
      //pinMode(TEMP_ADC_1_PIN, gpioModeDisabled);
      //pinMode(TEMP_ADC_2_PIN, gpioModeDisabled);
      //pinMode(TEMP_ADC_3_PIN, gpioModeDisabled);

      pinMode(HEAT_0_PIN, OUTPUT);
      pinMode(HEAT_1_PIN, OUTPUT);
      pinMode(HEAT_2_PIN, OUTPUT);
      pinMode(HEAT_3_PIN, OUTPUT);

      pinMode(FANS_0_PIN, OUTPUT);
      pinMode(FANS_1_PIN, OUTPUT);
      pinMode(FANS_2_PIN, OUTPUT);
      pinMode(FANS_3_PIN, OUTPUT);

      pinMode(MOTOR_0_ENABLE_PIN, OUTPUT);
      pinMode(MOTOR_0_DIR_PIN, OUTPUT);
      pinMode(MOTOR_0_STEP_PIN, OUTPUT);
      pinMode(MOTOR_0_MS0_PIN, OUTPUT);
      pinMode(MOTOR_0_MS1_PIN, OUTPUT);

      pinMode(MOTOR_1_ENABLE_PIN, OUTPUT);
      pinMode(MOTOR_1_DIR_PIN, OUTPUT);
      pinMode(MOTOR_1_STEP_PIN, OUTPUT);
      pinMode(MOTOR_1_MS0_PIN, OUTPUT);
      pinMode(MOTOR_1_MS1_PIN, OUTPUT);

      pinMode(MOTOR_2_ENABLE_PIN, OUTPUT);
      pinMode(MOTOR_2_DIR_PIN, OUTPUT);
      pinMode(MOTOR_2_STEP_PIN, OUTPUT);
      pinMode(MOTOR_2_MS0_PIN, OUTPUT);
      pinMode(MOTOR_2_MS1_PIN, OUTPUT);

      pinMode(MOTOR_3_ENABLE_PIN, OUTPUT);
      pinMode(MOTOR_3_DIR_PIN, OUTPUT);
      pinMode(MOTOR_3_STEP_PIN, OUTPUT);
      pinMode(MOTOR_3_MS0_PIN, OUTPUT);
      pinMode(MOTOR_3_MS1_PIN, OUTPUT);

      pinMode(MOTOR_4_ENABLE_PIN, OUTPUT);
      pinMode(MOTOR_4_DIR_PIN, OUTPUT);
      pinMode(MOTOR_4_STEP_PIN, OUTPUT);
      pinMode(MOTOR_4_MS0_PIN, OUTPUT);
      pinMode(MOTOR_4_MS1_PIN, OUTPUT);

      pinMode(MOTOR_5_ENABLE_PIN, OUTPUT);
      pinMode(MOTOR_5_DIR_PIN, OUTPUT);
      pinMode(MOTOR_5_STEP_PIN, OUTPUT);
      pinMode(MOTOR_5_MS0_PIN, OUTPUT);
      pinMode(MOTOR_5_MS1_PIN, OUTPUT);

      pinMode(MOTOR_6_ENABLE_PIN, OUTPUT);
      pinMode(MOTOR_6_DIR_PIN, OUTPUT);
      pinMode(MOTOR_6_STEP_PIN, OUTPUT);
      pinMode(MOTOR_6_MS0_PIN, OUTPUT);
      pinMode(MOTOR_6_MS1_PIN, OUTPUT);
   #endif
}

void cnc_gpio_write(uint8_t pin, uint8_t* port, uint8_t value){
   #ifdef SILABS
   if(value == 0){
      GPIO_PinOutClear((cast to port) port, (cast to pin) pin);
   } else {
      GPIO_PinOutSet((cast to port) port, (cast to pin) pin);
   }
   #else
   if(value == 0){
      CLR(*port, pin);
   } else {
      SET(*port, pin);
   }
   //digitalWrite(pin, value);
   #endif
}

uint8_t cnc_gpio_read(uint8_t pin, uint8_t* port){
   uint8_t ret_data;
   #ifdef SILABS
   ret_data = (uint8_t) GPIO_PinInGet((cast to port) port, (cast to pin) pin);
   #else
   //ret_data = (uint8_t) digitalRead(pin);
   ret_data = (uint8_t) GET(*port, pin);
   #endif
   return ret_data;
}

#ifdef SILABS
void disable_debug_pins(void){
	uint32_t *lock_bit_addr;
	uint32_t lock_bit_data;
	MSC_Init();
	lock_bit_addr = (uint32_t *) DEBUG_LOCK_BIT_OFFSET;
	lock_bit_data = *lock_bit_addr;
	lock_bit_data = lock_bit_data & 0xFFFFFFF0;
	MSC_WriteWord(lock_bit_addr, &lock_bit_data, sizeof(lock_bit_data));
	MSC_Deinit();
}

void enable_debug_pins(void){
	uint32_t *lock_bit_addr;
	uint32_t lock_bit_data;
	MSC_Init();
	lock_bit_addr = (uint32_t *) DEBUG_LOCK_BIT_OFFSET;
	lock_bit_data = *lock_bit_addr;
	lock_bit_data = lock_bit_data | 0x0000000F;
	MSC_WriteWord(lock_bit_addr, &lock_bit_data, sizeof(lock_bit_data));
	MSC_Deinit();
}

void disable_bootloader_pins(void){
	uint32_t *lock_bit_addr;
	uint32_t lock_bit_data;
	MSC_Init();
	lock_bit_addr = (uint32_t *) BOOTLOADER_LOCK_BIT_OFFSET;
	lock_bit_data = *lock_bit_addr;
	lock_bit_data = lock_bit_data & 0xFFFFFFFD;
	MSC_WriteWord(lock_bit_addr, &lock_bit_data, sizeof(lock_bit_data));
	MSC_Deinit();
}

void enable_bootloader_pins(void){
	uint32_t *lock_bit_addr;
	uint32_t lock_bit_data;
	MSC_Init();
	lock_bit_addr = (uint32_t *) BOOTLOADER_LOCK_BIT_OFFSET;
	lock_bit_data = *lock_bit_addr;
	lock_bit_data = lock_bit_data | 0x00000002;
	MSC_WriteWord(lock_bit_addr, &lock_bit_data, sizeof(lock_bit_data));
	MSC_Deinit();
}

void disable_route_pins(void){
	GPIO_DbgSWOEnable(0);
	GPIO_DbgSWDIOEnable(0);
	GPIO_DbgSWDClkEnable(0);
}

void enable_route_pins(void){
	GPIO_DbgSWOEnable(1);
	GPIO_DbgSWDIOEnable(1);
	GPIO_DbgSWDClkEnable(1);
}
#endif
