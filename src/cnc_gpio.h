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
   #include "rtcdriver.h"
#else
   #define GPIO_Port_TypeDef uint8_t*
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
   #define ENDSTOP_0_PIN          A4
   #define ENDSTOP_1_PIN          A5
   #define ENDSTOP_2_PIN          A6
   #define ENDSTOP_3_PIN          A7
   #define ENDSTOP_4_PIN          A8
   #define ENDSTOP_5_PIN          A9
   #define ENDSTOP_6_PIN          A10
   #define ENDSTOP_7_PIN          A11
   #define ENDSTOP_8_PIN          A12
   #define ENDSTOP_9_PIN          A13
   #define ENDSTOP_10_PIN         A14
   #define ENDSTOP_11_PIN         A15
   #define ENDSTOP_0_PINBIT       4
   #define ENDSTOP_1_PINBIT       5
   #define ENDSTOP_2_PINBIT       6
   #define ENDSTOP_3_PINBIT       7
   #define ENDSTOP_4_PINBIT       0
   #define ENDSTOP_5_PINBIT       1
   #define ENDSTOP_6_PINBIT       2
   #define ENDSTOP_7_PINBIT       3
   #define ENDSTOP_8_PINBIT       4
   #define ENDSTOP_9_PINBIT       5
   #define ENDSTOP_10_PINBIT      6
   #define ENDSTOP_11_PINBIT      7
   #define ENDSTOP_0_PINPORT      (uint8_t*) &PINF
   #define ENDSTOP_1_PINPORT      (uint8_t*) &PINF
   #define ENDSTOP_2_PINPORT      (uint8_t*) &PINF
   #define ENDSTOP_3_PINPORT      (uint8_t*) &PINF
   #define ENDSTOP_4_PINPORT      (uint8_t*) &PINK
   #define ENDSTOP_5_PINPORT      (uint8_t*) &PINK
   #define ENDSTOP_6_PINPORT      (uint8_t*) &PINK
   #define ENDSTOP_7_PINPORT      (uint8_t*) &PINK
   #define ENDSTOP_8_PINPORT      (uint8_t*) &PINK
   #define ENDSTOP_9_PINPORT      (uint8_t*) &PINK
   #define ENDSTOP_10_PINPORT     (uint8_t*) &PINK
   #define ENDSTOP_11_PINPORT     (uint8_t*) &PINK

   #define TEMP_ADC_0_PIN     A0
   #define TEMP_ADC_1_PIN     A1
   #define TEMP_ADC_2_PIN     A2
   #define TEMP_ADC_3_PIN     A3
   #define TEMP_ADC_0_PINBIT  0
   #define TEMP_ADC_1_PINBIT  1
   #define TEMP_ADC_2_PINBIT  2
   #define TEMP_ADC_3_PINBIT  3
   #define TEMP_ADC_0_PINPORT (uint8_t*) &PINF
   #define TEMP_ADC_1_PINPORT (uint8_t*) &PINF
   #define TEMP_ADC_2_PINPORT (uint8_t*) &PINF
   #define TEMP_ADC_3_PINPORT (uint8_t*) &PINF

   #define FANS_0_PIN     18
   #define FANS_1_PIN     19
   #define FANS_2_PIN     20
   #define FANS_3_PIN     21
   #define FANS_0_PINBIT  3
   #define FANS_1_PINBIT  2
   #define FANS_2_PINBIT  1
   #define FANS_3_PINBIT  0
   #define FANS_0_PINPORT (uint8_t*) &PORTD
   #define FANS_1_PINPORT (uint8_t*) &PORTD
   #define FANS_2_PINPORT (uint8_t*) &PORTD
   #define FANS_3_PINPORT (uint8_t*) &PORTD

   #define HEAT_0_PIN 14
   #define HEAT_1_PIN 15
   #define HEAT_2_PIN 16
   #define HEAT_3_PIN 17
   #define HEAT_0_PINBIT 1
   #define HEAT_1_PINBIT 0
   #define HEAT_2_PINBIT 1
   #define HEAT_3_PINBIT 0
   #define HEAT_0_PINPORT (uint8_t*) &PORTJ
   #define HEAT_1_PINPORT (uint8_t*) &PORTJ
   #define HEAT_2_PINPORT (uint8_t*) &PORTH
   #define HEAT_3_PINPORT (uint8_t*) &PORTH

   #define TOOL_0_EN_PIN     2
   #define TOOL_1_EN_PIN     7
   #define TOOL_0_EN_PINBIT  4
   #define TOOL_1_EN_PINBIT  4
   #define TOOL_0_EN_PINPORT (uint8_t*) &PORTE
   #define TOOL_1_EN_PINPORT (uint8_t*) &PORTH

   #define MOTOR_0_MS0_PIN     24
   #define MOTOR_0_MS1_PIN     25
   #define MOTOR_1_MS0_PIN     28
   #define MOTOR_1_MS1_PIN     29
   #define MOTOR_2_MS0_PIN     32
   #define MOTOR_2_MS1_PIN     33
   #define MOTOR_3_MS0_PIN     36
   #define MOTOR_3_MS1_PIN     37
   #define MOTOR_4_MS0_PIN     40
   #define MOTOR_4_MS1_PIN     41
   #define MOTOR_5_MS0_PIN     44
   #define MOTOR_5_MS1_PIN     45
   #define MOTOR_6_MS0_PIN     48
   #define MOTOR_6_MS1_PIN     49
   #define MOTOR_0_MS0_PINBIT  2
   #define MOTOR_0_MS1_PINBIT  3
   #define MOTOR_1_MS0_PINBIT  6
   #define MOTOR_1_MS1_PINBIT  7
   #define MOTOR_2_MS0_PINBIT  5
   #define MOTOR_2_MS1_PINBIT  4
   #define MOTOR_3_MS0_PINBIT  1
   #define MOTOR_3_MS1_PINBIT  0
   #define MOTOR_4_MS0_PINBIT  1
   #define MOTOR_4_MS1_PINBIT  0
   #define MOTOR_5_MS0_PINBIT  5
   #define MOTOR_5_MS1_PINBIT  4
   #define MOTOR_6_MS0_PINBIT  1
   #define MOTOR_6_MS1_PINBIT  0
   #define MOTOR_0_MS0_PINPORT (uint8_t*) &PORTA
   #define MOTOR_0_MS1_PINPORT (uint8_t*) &PORTA
   #define MOTOR_1_MS0_PINPORT (uint8_t*) &PORTA
   #define MOTOR_1_MS1_PINPORT (uint8_t*) &PORTA
   #define MOTOR_2_MS0_PINPORT (uint8_t*) &PORTC
   #define MOTOR_2_MS1_PINPORT (uint8_t*) &PORTC
   #define MOTOR_3_MS0_PINPORT (uint8_t*) &PORTC
   #define MOTOR_3_MS1_PINPORT (uint8_t*) &PORTC
   #define MOTOR_4_MS0_PINPORT (uint8_t*) &PORTG
   #define MOTOR_4_MS1_PINPORT (uint8_t*) &PORTG
   #define MOTOR_5_MS0_PINPORT (uint8_t*) &PORTL
   #define MOTOR_5_MS1_PINPORT (uint8_t*) &PORTL
   #define MOTOR_6_MS0_PINPORT (uint8_t*) &PORTL
   #define MOTOR_6_MS1_PINPORT (uint8_t*) &PORTL

   #define MOTOR_0_ENABLE_PIN     22
   #define MOTOR_1_ENABLE_PIN     26
   #define MOTOR_2_ENABLE_PIN     30
   #define MOTOR_3_ENABLE_PIN     34
   #define MOTOR_4_ENABLE_PIN     38
   #define MOTOR_5_ENABLE_PIN     42
   #define MOTOR_6_ENABLE_PIN     46
   #define MOTOR_0_ENABLE_PINBIT  0
   #define MOTOR_1_ENABLE_PINBIT  4
   #define MOTOR_2_ENABLE_PINBIT  7
   #define MOTOR_3_ENABLE_PINBIT  3
   #define MOTOR_4_ENABLE_PINBIT  7
   #define MOTOR_5_ENABLE_PINBIT  7
   #define MOTOR_6_ENABLE_PINBIT  3
   #define MOTOR_0_ENABLE_PINPORT (uint8_t*) &PORTA
   #define MOTOR_1_ENABLE_PINPORT (uint8_t*) &PORTA
   #define MOTOR_2_ENABLE_PINPORT (uint8_t*) &PORTC
   #define MOTOR_3_ENABLE_PINPORT (uint8_t*) &PORTC
   #define MOTOR_4_ENABLE_PINPORT (uint8_t*) &PORTD
   #define MOTOR_5_ENABLE_PINPORT (uint8_t*) &PORTL
   #define MOTOR_6_ENABLE_PINPORT (uint8_t*) &PORTL

   #define MOTOR_0_STEP_PIN     4
   #define MOTOR_1_STEP_PIN     5
   #define MOTOR_2_STEP_PIN     6
   #define MOTOR_3_STEP_PIN     7
   #define MOTOR_4_STEP_PIN     8
   #define MOTOR_5_STEP_PIN     9
   #define MOTOR_6_STEP_PIN     10
   #define MOTOR_0_STEP_PINBIT  5
   #define MOTOR_1_STEP_PINBIT  3
   #define MOTOR_2_STEP_PINBIT  3
   #define MOTOR_3_STEP_PINBIT  4
   #define MOTOR_4_STEP_PINBIT  5
   #define MOTOR_5_STEP_PINBIT  6
   #define MOTOR_6_STEP_PINBIT  4
   #define MOTOR_0_STEP_PINPORT (uint8_t*) &PORTG
   #define MOTOR_1_STEP_PINPORT (uint8_t*) &PORTE
   #define MOTOR_2_STEP_PINPORT (uint8_t*) &PORTH
   #define MOTOR_3_STEP_PINPORT (uint8_t*) &PORTH
   #define MOTOR_4_STEP_PINPORT (uint8_t*) &PORTH
   #define MOTOR_5_STEP_PINPORT (uint8_t*) &PORTH
   #define MOTOR_6_STEP_PINPORT (uint8_t*) &PORTB

   #define MOTOR_0_DIR_PIN 23
   #define MOTOR_1_DIR_PIN 27
   #define MOTOR_2_DIR_PIN 31
   #define MOTOR_3_DIR_PIN 35
   #define MOTOR_4_DIR_PIN 39
   #define MOTOR_5_DIR_PIN 43
   #define MOTOR_6_DIR_PIN 47
   #define MOTOR_0_DIR_PINBIT 1
   #define MOTOR_1_DIR_PINBIT 5
   #define MOTOR_2_DIR_PINBIT 6
   #define MOTOR_3_DIR_PINBIT 2
   #define MOTOR_4_DIR_PINBIT 2
   #define MOTOR_5_DIR_PINBIT 6
   #define MOTOR_6_DIR_PINBIT 2
   #define MOTOR_0_DIR_PINPORT (uint8_t*) &PORTA
   #define MOTOR_1_DIR_PINPORT (uint8_t*) &PORTA
   #define MOTOR_2_DIR_PINPORT (uint8_t*) &PORTC
   #define MOTOR_3_DIR_PINPORT (uint8_t*) &PORTC
   #define MOTOR_4_DIR_PINPORT (uint8_t*) &PORTG
   #define MOTOR_5_DIR_PINPORT (uint8_t*) &PORTL
   #define MOTOR_6_DIR_PINPORT (uint8_t*) &PORTL
#endif

void init_gpio(struct cnc_state_struct* cnc);
void cnc_gpio_write(uint8_t pin, uint8_t* port, uint8_t value);
uint8_t cnc_gpio_read(uint8_t pin, uint8_t* port);

#endif /* SRC_CNC_GPIO_H_ */
