#ifndef APP_GPIO_H
#define APP_GPIO_H

#include "sys_gpio.h"

//-------------- PORT A --------------//
#define GPIO_MOTOR_Y                       (EFM32PG_PIN_PA_14) // TIMER2_CC2 (LOCATION 1)

//-------------- PORT B --------------//
#define GPIO_LFXTAL_P                      (EFM32PG_PIN_PB_7)
#define GPIO_LFXTAL_N                      (EFM32PG_PIN_PB_8)
#define GPIO_BUTTON0                       (EFM32PG_PIN_PB_9)
#define GPIO_BUTTON1                       (EFM32PG_PIN_PB_10)
#define GPIO_MOTOR_X_STOP                  (EFM32PG_PIN_PB_11) // GPIO interrupt 11

//-------------- PORT C --------------//
#define GPIO_MOTOR_X_EN                    (EFM32PG_PIN_PC_0)
// PC1
// PC2
#define GPIO_MOTOR_X_DIR                   (EFM32PG_PIN_PC_3)
#define GPIO_MOTOR_X_STEP                  (EFM32PG_PIN_PC_4)
#define GPIO_MOTOR_X_HOME                  (EFM32PG_PIN_PC_5)  // GPIO interrupt 5
#define GPIO_MOTOR_Y_HOME                  (EFM32PG_PIN_PC_6)  // GPIO interrupt 6

//-------------- PORT D --------------//
#define GPIO_SPI_MOSI                      (EFM32PG_PIN_PD_0)  // Yellow
#define GPIO_SPI_MISO                      (EFM32PG_PIN_PD_1)  // Green
#define GPIO_SPI_CLK                       (EFM32PG_PIN_PD_2)  // Orange
#define GPIO_SPI_CS                        (EFM32PG_PIN_PD_3)  // Brown - GPIO interrupt 3
#define GPIO_MOTOR_Y_EN                    (EFM32PG_PIN_PD_4)
#define GPIO_MOTOR_Y_DIR                   (EFM32PG_PIN_PD_5)
#define GPIO_MOTOR_Y_STEP                  (EFM32PG_PIN_PD_6)
#define GPIO_MOTOR_Y_STOP                  (EFM32PG_PIN_PD_7)  // GPIO interrupt 7
// PD8
#define GPIO_CLKOUT0                       (EFM32PG_PIN_PD_9)

//-------------- PORT E --------------//
#define GPIO_LED0                          (EFM32PG_PIN_PE_2)
#define GPIO_LED1                          (EFM32PG_PIN_PE_3)

//-------------- PORT F --------------//
// SWCLK
// SWDIO
// SWO

#endif // APP_GPIO_H

//************************************************************************************************************
// END OF FILE
