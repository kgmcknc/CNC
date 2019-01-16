/**
 * @file sys_gpio.h
 *
 * @copyright 2013 by Sensus. All rights reserved.
 *
 * Functions used by components for basic control of hardware GPIO.
 *
 */

#ifndef SYS_GPIO_H // {
#define SYS_GPIO_H

#include <stdbool.h>

#include "em_device.h"

//NOTE The GPIO macros here do not handle Port 4
typedef struct {
   uint8_t portNum;
   uint8_t pinNum;
} Sys_GpioPin_t;

#define SYS_GPIO_PIN(port,pin) ((Sys_GpioPin_t)      \
   {                                                 \
      port,                                          \
      pin                                            \
   }                                                 \
   )

typedef enum {
	Default,
} GPIO_Pin_t;

#define EFM32PG_GPIO_PORT_A (0)
#define EFM32PG_GPIO_PORT_B (1)
#define EFM32PG_GPIO_PORT_C (2)
#define EFM32PG_GPIO_PORT_D (3)
#define EFM32PG_GPIO_PORT_E (4)
#define EFM32PG_GPIO_PORT_F (5)

#define EFM32PG_GPIO_PORT_A_PIN_COUNT 16
#define EFM32PG_GPIO_PORT_B_PIN_COUNT 16
#define EFM32PG_GPIO_PORT_C_PIN_COUNT 16
#define EFM32PG_GPIO_PORT_D_PIN_COUNT 16
#define EFM32PG_GPIO_PORT_E_PIN_COUNT 16
#define EFM32PG_GPIO_PORT_F_PIN_COUNT 13

#define EFM32PG_GPIO_PORT_A_PIN_MASK 0xFFFF
#define EFM32PG_GPIO_PORT_B_PIN_MASK 0xFFFF
#define EFM32PG_GPIO_PORT_C_PIN_MASK 0xFFFF
#define EFM32PG_GPIO_PORT_D_PIN_MASK 0xFFFF
#define EFM32PG_GPIO_PORT_E_PIN_MASK 0xFFFF
#define EFM32PG_GPIO_PORT_F_PIN_MASK 0x1FFF

#define EFM32PG_PIN_PA_0    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 0)
#define EFM32PG_PIN_PA_1    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 1)
#define EFM32PG_PIN_PA_2    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 2)
#define EFM32PG_PIN_PA_3    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 3)
#define EFM32PG_PIN_PA_4    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 4)
#define EFM32PG_PIN_PA_5    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 5)
#define EFM32PG_PIN_PA_6    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 6)
#define EFM32PG_PIN_PA_7    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 7)
#define EFM32PG_PIN_PA_8    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 8)
#define EFM32PG_PIN_PA_9    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 9)
#define EFM32PG_PIN_PA_10   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 10)
#define EFM32PG_PIN_PA_11   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 11)
#define EFM32PG_PIN_PA_12   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 12)
#define EFM32PG_PIN_PA_13   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 13)
#define EFM32PG_PIN_PA_14   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 14)
#define EFM32PG_PIN_PA_15   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_A, 15)

#define EFM32PG_PIN_PB_0    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 0)
#define EFM32PG_PIN_PB_1    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 1)
#define EFM32PG_PIN_PB_2    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 2)
#define EFM32PG_PIN_PB_3    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 3)
#define EFM32PG_PIN_PB_4    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 4)
#define EFM32PG_PIN_PB_5    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 5)
#define EFM32PG_PIN_PB_6    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 6)
#define EFM32PG_PIN_PB_7    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 7)
#define EFM32PG_PIN_PB_8    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 8)
#define EFM32PG_PIN_PB_9    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 9)
#define EFM32PG_PIN_PB_10   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 10)
#define EFM32PG_PIN_PB_11   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 11)
#define EFM32PG_PIN_PB_12   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 12)
#define EFM32PG_PIN_PB_13   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 13)
#define EFM32PG_PIN_PB_14   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 14)
#define EFM32PG_PIN_PB_15   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_B, 15)

#define EFM32PG_PIN_PC_0    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 0)
#define EFM32PG_PIN_PC_1    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 1)
#define EFM32PG_PIN_PC_2    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 2)
#define EFM32PG_PIN_PC_3    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 3)
#define EFM32PG_PIN_PC_4    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 4)
#define EFM32PG_PIN_PC_5    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 5)
#define EFM32PG_PIN_PC_6    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 6)
#define EFM32PG_PIN_PC_7    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 7)
#define EFM32PG_PIN_PC_8    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 8)
#define EFM32PG_PIN_PC_9    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 9)
#define EFM32PG_PIN_PC_10   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 10)
#define EFM32PG_PIN_PC_11   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 11)
#define EFM32PG_PIN_PC_12   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 12)
#define EFM32PG_PIN_PC_13   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 13)
#define EFM32PG_PIN_PC_14   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 14)
#define EFM32PG_PIN_PC_15   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_C, 15)

#define EFM32PG_PIN_PD_0    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 0)
#define EFM32PG_PIN_PD_1    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 1)
#define EFM32PG_PIN_PD_2    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 2)
#define EFM32PG_PIN_PD_3    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 3)
#define EFM32PG_PIN_PD_4    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 4)
#define EFM32PG_PIN_PD_5    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 5)
#define EFM32PG_PIN_PD_6    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 6)
#define EFM32PG_PIN_PD_7    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 7)
#define EFM32PG_PIN_PD_8    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 8)
#define EFM32PG_PIN_PD_9    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 9)
#define EFM32PG_PIN_PD_10   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 10)
#define EFM32PG_PIN_PD_11   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 11)
#define EFM32PG_PIN_PD_12   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 12)
#define EFM32PG_PIN_PD_13   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 13)
#define EFM32PG_PIN_PD_14   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 14)
#define EFM32PG_PIN_PD_15   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_D, 15)

#define EFM32PG_PIN_PE_0    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 0)
#define EFM32PG_PIN_PE_1    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 1)
#define EFM32PG_PIN_PE_2    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 2)
#define EFM32PG_PIN_PE_3    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 3)
#define EFM32PG_PIN_PE_4    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 4)
#define EFM32PG_PIN_PE_5    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 5)
#define EFM32PG_PIN_PE_6    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 6)
#define EFM32PG_PIN_PE_7    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 7)
#define EFM32PG_PIN_PE_8    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 8)
#define EFM32PG_PIN_PE_9    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 9)
#define EFM32PG_PIN_PE_10   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 10)
#define EFM32PG_PIN_PE_11   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 11)
#define EFM32PG_PIN_PE_12   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 12)
#define EFM32PG_PIN_PE_13   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 13)
#define EFM32PG_PIN_PE_14   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 14)
#define EFM32PG_PIN_PE_15   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_E, 15)

#define EFM32PG_PIN_PF_0    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 0)
#define EFM32PG_PIN_PF_1    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 1)
#define EFM32PG_PIN_PF_2    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 2)
#define EFM32PG_PIN_PF_3    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 3)
#define EFM32PG_PIN_PF_4    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 4)
#define EFM32PG_PIN_PF_5    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 5)
#define EFM32PG_PIN_PF_6    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 6)
#define EFM32PG_PIN_PF_7    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 7)
#define EFM32PG_PIN_PF_8    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 8)
#define EFM32PG_PIN_PF_9    SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 9)
#define EFM32PG_PIN_PF_10   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 10)
#define EFM32PG_PIN_PF_11   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 11)
#define EFM32PG_PIN_PF_12   SYS_GPIO_PIN(EFM32PG_GPIO_PORT_F, 12)


//Sets the pin to be a digital push pull output
#define GPIO_DirOut(pin)    { \
    if (pin.pinNum < 8) \
    {  \
      GPIO->P[pin.portNum].MODEL &= ~(0xF << 4*pin.pinNum);\
      GPIO->P[pin.portNum].MODEL |= _GPIO_P_MODEL_MODE0_PUSHPULL << (4*pin.pinNum); \
    } \
    else \
    { \
      GPIO->P[pin.portNum].MODEH &= ~(0xF << 4*(pin.pinNum-8));\
      GPIO->P[pin.portNum].MODEH |= _GPIO_P_MODEL_MODE0_PUSHPULL << (4*(pin.pinNum-8)); \
    } \
}

//Sets the pin to be a digital push pull output with a drive strength
#define GPIO_DirOutDrive(pin)    { \
    if (pin.pinNum < 8) \
    {  \
      GPIO->P[pin.portNum].MODEL &= ~(0xF << 4*pin.pinNum);\
      GPIO->P[pin.portNum].MODEL |= _GPIO_P_MODEL_MODE0_PUSHPULLDRIVE << (4*pin.pinNum); \
    } \
    else \
    { \
      GPIO->P[pin.portNum].MODEH &= ~(0xF << 4*(pin.pinNum-8));\
      GPIO->P[pin.portNum].MODEH |= _GPIO_P_MODEL_MODE0_PUSHPULLDRIVE << (4*(pin.pinNum-8)); \
    } \
}

//Sets the pin to be a digital Open Drain output
#define GPIO_SetOpenDrain(pin)  { \
   if (pin.pinNum < 8) \
   {  \
      GPIO->P[pin.portNum].MODEL &= ~(0xF << 4*pin.pinNum);\
      GPIO->P[pin.portNum].MODEL |= _GPIO_P_MODEL_MODE0_WIREDAND << (4*pin.pinNum); \
   } \
   else \
   { \
      GPIO->P[pin.portNum].MODEH &= ~(0xF << 4*(pin.pinNum-8));\
      GPIO->P[pin.portNum].MODEH |= _GPIO_P_MODEL_MODE0_WIREDAND << (4*(pin.pinNum-8)); \
   } \
}

//Sets the pin to be a digital Open Drain output
#define GPIO_SetWiredPullUpFilter(pin)  { \
   if (pin.pinNum < 8) \
   {  \
      GPIO->P[pin.portNum].MODEL &= ~(0xF << 4*pin.pinNum);\
      GPIO->P[pin.portNum].MODEL |= _GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER << (4*pin.pinNum); \
   } \
   else \
   { \
      GPIO->P[pin.portNum].MODEH &= ~(0xF << 4*(pin.pinNum-8));\
      GPIO->P[pin.portNum].MODEH |= _GPIO_P_MODEL_MODE0_WIREDANDPULLUPFILTER << (4*(pin.pinNum-8)); \
   } \
}

//sets pin up as digital Input
#define GPIO_DirIn(pin)    { \
    if (pin.pinNum < 8) \
    {  \
       GPIO->P[pin.portNum].MODEL &= ~(0xF << 4*pin.pinNum);\
       GPIO->P[pin.portNum].MODEL |= _GPIO_P_MODEL_MODE0_INPUT << (4*pin.pinNum); \
    } \
    else \
    { \
       GPIO->P[pin.portNum].MODEH &= ~(0xF << 4*(pin.pinNum-8));\
       GPIO->P[pin.portNum].MODEH |= _GPIO_P_MODEL_MODE0_INPUT << (4*(pin.pinNum-8)); \
    } \
}

//sets pin up as digital Input
#define GPIO_DirInPullDown(pin)    { \
    if (pin.pinNum < 8) \
    {  \
       GPIO->P[pin.portNum].MODEL &= ~(0xF << 4*pin.pinNum);\
       GPIO->P[pin.portNum].MODEL |= _GPIO_P_MODEL_MODE0_INPUTPULL << (4*pin.pinNum); \
    } \
    else \
    { \
       GPIO->P[pin.portNum].MODEH &= ~(0xF << 4*(pin.pinNum-8));\
       GPIO->P[pin.portNum].MODEH |= _GPIO_P_MODEL_MODE0_INPUTPULL << (4*(pin.pinNum-8)); \
    } \
}

//sets pin up as Analog function
#define GPIO_SetAsAnalog(pin) {  \
   if (pin.pinNum < 8) \
   {  \
      GPIO->P[pin.portNum].MODEL &= ~(0xF << 4*pin.pinNum);\
      GPIO->P[pin.portNum].MODEL |= _GPIO_P_MODEL_MODE0_INPUT << (4*pin.pinNum); \
   } \
   else \
   { \
      GPIO->P[pin.portNum].MODEH &= ~(0xF << (4*pin.pinNum-8));\
      GPIO->P[pin.portNum].MODEH |= _GPIO_P_MODEL_MODE0_INPUT << (4*(pin.pinNum-8)); \
   } \
}

//drives pin high
#define GPIO_Set(pin)      (GPIO->P[pin.portNum].DOUTSET = 1 << pin.pinNum)

//drive pin low
#define GPIO_Clr(pin)      (GPIO->P[pin.portNum].DOUTCLR = 1 << pin.pinNum)

//toggle pin
#define GPIO_Not(pin)      (GPIO->P[pin.portNum].DOUTTGL = 1 << pin.pinNum)


//Sets pin up with high drive strength
#define GPIO_DriveHigh(pin)  (GPIO->P[pin.portNum].CTRL = GPIO_P_CTRL_DRIVEMODE_HIGH)

//sets pin up with low drive strength
#define GPIO_DriveLow(pin)  (GPIO->P[pin.portNum].CTRL = GPIO_P_CTRL_DRIVEMODE_STANDARD)

//Reads a GPIO level from pin
#define GPIO_In(pin) (GPIO->P[pin.portNum].DIN & 0x1 << pin.pinNum)

#if 0
//enables the weak pull up for all the pins on the port (individual pins can not be selectively pulled up)
#define GPIO_EnablePortPullUps(port)  ((port)->PBDRV_SET = SI32_PBSTD_A_PBDRV_PBPUEN_MASK)

//disables the weak pull up for all the pins on the port (individual pins can not be selectively pulled up)
#define GPIO_DisablePortPullUps(port)  ((port)->PBDRV_CLR = SI32_PBSTD_A_PBDRV_PBPUEN_MASK)
#endif

#define GPIO_Id(A)                     &(A)

// Sets the IO port as a general purpose IO
#define GPIO_FuncGPIO(pin)

// Sets the IO port as a special purpose IO
#define GPIO_FuncSPIO(pin)

#define GPIO_IntClear(pin)

typedef bool (*GPIO_InterruptHandler_t)(void);

bool GPIO_SetInterruptHandler(Sys_GpioPin_t pin, GPIO_InterruptHandler_t functionCallBack);

void GPIO_IntEnable(Sys_GpioPin_t pin);
void GPIO_IntDisable(Sys_GpioPin_t pin);

void GPIO_IntHighLevel(Sys_GpioPin_t pin);
void GPIO_IntLowLevel(Sys_GpioPin_t pin);
void GPIO_IntRise(Sys_GpioPin_t pin);
void GPIO_IntFall(Sys_GpioPin_t pin);
void GPIO_IntRiseFall(Sys_GpioPin_t pin);

#endif // SYS_GPIO_H }
