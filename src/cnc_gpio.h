/*
 * cnc_gpio.h
 *
 *  Created on: Aug 17, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_GPIO_H_
#define SRC_CNC_GPIO_H_

#include "stdint.h"

#ifdef SILABS
   #include "em_gpio.h"
#else

#endif

#ifdef SILABS
   #define NUMBER_OF_ENDSTOPS 8
   #define ENDSTOP_0_PORT     gpioPortC // C
   #define ENDSTOP_0_PIN      10
   #define ENDSTOP_1_PORT     gpioPortD // D
   #define ENDSTOP_1_PIN      11
   #define ENDSTOP_2_PORT     gpioPortA // A
   #define ENDSTOP_2_PIN      9
   #define ENDSTOP_3_PORT     gpioPortA // A
   #define ENDSTOP_3_PIN      8
   #define ENDSTOP_4_PORT     gpioPortA // A
   #define ENDSTOP_4_PIN      7
   #define ENDSTOP_5_PORT     gpioPortA // A
   #define ENDSTOP_5_PIN      6
   #define ENDSTOP_6_PORT     gpioPortC // C
   #define ENDSTOP_6_PIN      11
   #define ENDSTOP_7_PORT     gpioPortC // C
   #define ENDSTOP_7_PIN      9

   #define ENDSTOP_L_PORT ENDSTOP_0_PORT
   #define ENDSTOP_R_PORT ENDSTOP_1_PORT
   #define ENDSTOP_F_PORT ENDSTOP_2_PORT
   #define ENDSTOP_B_PORT ENDSTOP_3_PORT
   #define ENDSTOP_BL_PORT ENDSTOP_4_PORT
   #define ENDSTOP_TL_PORT ENDSTOP_5_PORT
   #define ENDSTOP_BR_PORT ENDSTOP_6_PORT
   #define ENDSTOP_TR_PORT ENDSTOP_7_PORT

   #define ENDSTOP_L_PIN ENDSTOP_0_PIN
   #define ENDSTOP_R_PIN ENDSTOP_1_PIN
   #define ENDSTOP_F_PIN ENDSTOP_2_PIN
   #define ENDSTOP_B_PIN ENDSTOP_3_PIN
   #define ENDSTOP_BL_PIN ENDSTOP_4_PIN
   #define ENDSTOP_TL_PIN ENDSTOP_5_PIN
   #define ENDSTOP_BR_PIN ENDSTOP_6_PIN
   #define ENDSTOP_TR_PIN ENDSTOP_7_PIN

   #define SPI_REQUEST_PORT     gpioPortD // C
   #define SPI_REQUEST_PIN      10
   #define SPI_RESET_PORT       gpioPortD // C
   #define SPI_RESET_PIN        9

   #define TEMP_ADC_0_PORT gpioPortA
   #define TEMP_ADC_1_PORT gpioPortA
   #define TEMP_ADC_2_PORT gpioPortA
   #define TEMP_ADC_3_PORT gpioPortA
   #define TEMP_ADC_0_PIN 0
   #define TEMP_ADC_1_PIN 1
   #define TEMP_ADC_2_PIN 2
   #define TEMP_ADC_3_PIN 3

   #define FANS_PORT gpioPortK
   #define FANS_PIN 2

   #define HEAT_0_PORT gpioPortB
   #define HEAT_1_PORT gpioPortB
   #define HEAT_2_PORT gpioPortB
   #define HEAT_3_PORT gpioPortB
   #define HEAT_0_PIN 10
   #define HEAT_1_PIN 11
   #define HEAT_2_PIN 12
   #define HEAT_3_PIN 13

   #define TOOL_0_EN_PORT gpioPortK
   #define TOOL_1_EN_PORT gpioPortF
   #define TOOL_0_EN_PIN 2
   #define TOOL_1_EN_PIN 7

   #define MOTOR_0_MS0_PORT gpioPortD
   #define MOTOR_0_MS1_PORT gpioPortD
   #define MOTOR_1_MS0_PORT gpioPortA
   #define MOTOR_1_MS1_PORT gpioPortA
   #define MOTOR_2_MS0_PORT gpioPortK
   #define MOTOR_2_MS1_PORT gpioPortK
   #define MOTOR_3_MS0_PORT gpioPortD
   #define MOTOR_3_MS1_PORT gpioPortD
   #define MOTOR_4_MS0_PORT gpioPortC
   #define MOTOR_4_MS1_PORT gpioPortB
   #define MOTOR_5_MS0_PORT gpioPortC
   #define MOTOR_5_MS1_PORT gpioPortC
   #define MOTOR_6_MS0_PORT gpioPortC
   #define MOTOR_6_MS1_PORT gpioPortC
   #define MOTOR_0_MS0_PIN 12
   #define MOTOR_0_MS1_PIN 13
   #define MOTOR_1_MS0_PIN 4
   #define MOTOR_1_MS1_PIN 5
   #define MOTOR_2_MS0_PIN 0
   #define MOTOR_2_MS1_PIN 1
   #define MOTOR_3_MS0_PIN 14
   #define MOTOR_3_MS1_PIN 15
   #define MOTOR_4_MS0_PIN 8
   #define MOTOR_4_MS1_PIN 9
   #define MOTOR_5_MS0_PIN 6
   #define MOTOR_5_MS1_PIN 7
   #define MOTOR_6_MS0_PIN 4
   #define MOTOR_6_MS1_PIN 5

   #define MOTOR_0_ENABLE_PORT gpioPortJ
   #define MOTOR_1_ENABLE_PORT gpioPortJ
   #define MOTOR_2_ENABLE_PORT gpioPortF
   #define MOTOR_3_ENABLE_PORT gpioPortI
   #define MOTOR_4_ENABLE_PORT gpioPortI
   #define MOTOR_5_ENABLE_PORT gpioPortI
   #define MOTOR_6_ENABLE_PORT gpioPortI
   #define MOTOR_0_ENABLE_PIN 14
   #define MOTOR_1_ENABLE_PIN 15
   #define MOTOR_2_ENABLE_PIN 15
   #define MOTOR_3_ENABLE_PIN 3
   #define MOTOR_4_ENABLE_PIN 2
   #define MOTOR_5_ENABLE_PIN 1
   #define MOTOR_6_ENABLE_PIN 0

   #define MOTOR_0_STEP_PORT gpioPortF
   #define MOTOR_1_STEP_PORT gpioPortF
   #define MOTOR_2_STEP_PORT gpioPortF
   #define MOTOR_3_STEP_PORT gpioPortF
   #define MOTOR_4_STEP_PORT gpioPortF
   #define MOTOR_5_STEP_PORT gpioPortF
   #define MOTOR_6_STEP_PORT gpioPortF
   #define MOTOR_0_STEP_PIN 4
   #define MOTOR_1_STEP_PIN 5
   #define MOTOR_2_STEP_PIN 6
   #define MOTOR_3_STEP_PIN 3
   #define MOTOR_4_STEP_PIN 2
   #define MOTOR_5_STEP_PIN 1
   #define MOTOR_6_STEP_PIN 0

   #define MOTOR_0_DIR_PORT gpioPortF
   #define MOTOR_1_DIR_PORT gpioPortF
   #define MOTOR_2_DIR_PORT gpioPortF
   #define MOTOR_3_DIR_PORT gpioPortF
   #define MOTOR_4_DIR_PORT gpioPortF
   #define MOTOR_5_DIR_PORT gpioPortF
   #define MOTOR_6_DIR_PORT gpioPortF
   #define MOTOR_0_DIR_PIN 12
   #define MOTOR_1_DIR_PIN 13
   #define MOTOR_2_DIR_PIN 14
   #define MOTOR_3_DIR_PIN 11
   #define MOTOR_4_DIR_PIN 10
   #define MOTOR_5_DIR_PIN 9
   #define MOTOR_6_DIR_PIN 8

   #define LOCK_BITS_ADDRESS_OFFSET 0x0FE04000
   #define DEBUG_LOCK_BIT_PAGE 127
   #define BOOTLOADER_LOCK_BIT_PAGE 122
   #define DEBUG_LOCK_BIT_OFFSET (LOCK_BITS_ADDRESS_OFFSET + DEBUG_LOCK_BIT_PAGE*4)
   #define BOOTLOADER_LOCK_BIT_OFFSET (LOCK_BITS_ADDRESS_OFFSET + BOOTLOADER_LOCK_BIT_PAGE*4)

   void disable_debug_pins(void);
   void enable_debug_pins(void);
   void disable_bootloader_pins(void);
   void enable_bootloader_pins(void);
   void enable_route_pins(void);
   void disable_route_pins(void);
#else
   #define LED                LED_BUILTIN

   #define ENDSTOP_0_PIN      A4
   #define ENDSTOP_1_PIN      A5
   #define ENDSTOP_2_PIN      A6
   #define ENDSTOP_3_PIN      A7
   #define ENDSTOP_4_PIN      A8
   #define ENDSTOP_5_PIN      A9
   #define ENDSTOP_6_PIN      A10
   #define ENDSTOP_7_PIN      A11
   #define ENDSTOP_8_PIN      A12
   #define ENDSTOP_9_PIN      A13
   #define ENDSTOP_10_PIN     A14
   #define ENDSTOP_11_PIN     A15

   #define TEMP_ADC_0_PIN 0
   #define TEMP_ADC_1_PIN 1
   #define TEMP_ADC_2_PIN 2
   #define TEMP_ADC_3_PIN 3

   #define FANS_0_PIN 18
   #define FANS_1_PIN 19
   #define FANS_2_PIN 20
   #define FANS_3_PIN 21

   #define HEAT_0_PIN 14
   #define HEAT_1_PIN 15
   #define HEAT_2_PIN 16
   #define HEAT_3_PIN 17

   #define TOOL_0_EN_PIN 2
   #define TOOL_1_EN_PIN 7

   #define MOTOR_0_MS0_PIN 24
   #define MOTOR_0_MS1_PIN 25
   #define MOTOR_1_MS0_PIN 28
   #define MOTOR_1_MS1_PIN 29
   #define MOTOR_2_MS0_PIN 32
   #define MOTOR_2_MS1_PIN 33
   #define MOTOR_3_MS0_PIN 36
   #define MOTOR_3_MS1_PIN 37
   #define MOTOR_4_MS0_PIN 40
   #define MOTOR_4_MS1_PIN 41
   #define MOTOR_5_MS0_PIN 44
   #define MOTOR_5_MS1_PIN 45
   #define MOTOR_6_MS0_PIN 48
   #define MOTOR_6_MS1_PIN 49

   #define MOTOR_0_ENABLE_PIN 22
   #define MOTOR_1_ENABLE_PIN 26
   #define MOTOR_2_ENABLE_PIN 30
   #define MOTOR_3_ENABLE_PIN 34
   #define MOTOR_4_ENABLE_PIN 38
   #define MOTOR_5_ENABLE_PIN 42
   #define MOTOR_6_ENABLE_PIN 46

   #define MOTOR_0_STEP_PIN 4
   #define MOTOR_1_STEP_PIN 5
   #define MOTOR_2_STEP_PIN 6
   #define MOTOR_3_STEP_PIN 7
   #define MOTOR_4_STEP_PIN 8
   #define MOTOR_5_STEP_PIN 9
   #define MOTOR_6_STEP_PIN 10

   #define MOTOR_0_DIR_PIN 23
   #define MOTOR_1_DIR_PIN 27
   #define MOTOR_2_DIR_PIN 31
   #define MOTOR_3_DIR_PIN 35
   #define MOTOR_4_DIR_PIN 39
   #define MOTOR_5_DIR_PIN 43
   #define MOTOR_6_DIR_PIN 47
#endif

void init_gpio(struct cnc_state_struct* cnc);
void cnc_gpio_write(uint32_t pin, uint32_t port, uint8_t value);
uint8_t cnc_gpio_read(uint32_t pin, uint32_t port);

#endif /* SRC_CNC_GPIO_H_ */
