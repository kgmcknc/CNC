#include <string.h>

#include "em_device.h"

#include "efm32pg_cmu.h"
//#include "efm32pg_dma.h"
#include "efm32pg_sys_timer.h"

#include "sys_int.h"

#include "prod_gpio.h"

#if 0
#include "motor_control.h"
#include "spi_slave_driver.h"
#endif

// System Timer Configuration //
void _1milliSecondEvent(void);
void _1SecondEvent(void);
SysTimer_Configuration_t sysTimerConfig = {
      ._1milliSecondEvent = _1milliSecondEvent,
      ._1SecondEvent = _1SecondEvent
};

// Button Events //
bool Button0Handler(void);
bool Button1Handler(void);

// Endstop Events //
bool EndstopXHandler(void);
bool EndstopYHandler(void);

// SPI Configuration //
#define SPI_BUFFER_SIZE        (256)

bool IncomingData(void);
void SpiComplete(void);

uint8_t TxDataBuffer[SPI_BUFFER_SIZE] = { 0 };
uint8_t RxDataBuffer[SPI_BUFFER_SIZE] = { 0 };

#if 0
SpiSlave_Configuration_t spiConfig = {
      .TxDataBufferPtr = TxDataBuffer,
      .RxDataBufferPtr = RxDataBuffer,
      .DataCount = SPI_BUFFER_SIZE,
      .CompleteCallbackFPtr = SpiComplete
};
#endif

int main(void)
{
   memcpy(TxDataBuffer, "Hello PC!", sizeof("Hello PC!"));

   // Initialize Clocks
   Efm32pg_CmuInit();

#if 0
   // DMA Init
   Efm32gg_DmaInit();

   // System Timer initialization
   SystemTimerInit(sysTimerConfig);

   // HAL initialization
   NVIC_ClearPendingIRQ(GPIO_EVEN_IRQn);
   NVIC_EnableIRQ(GPIO_EVEN_IRQn);
   NVIC_ClearPendingIRQ(GPIO_ODD_IRQn);
   NVIC_EnableIRQ(GPIO_ODD_IRQn);

   // Set global interrupt enable (bit in status register)
   Sys_IntEnableForce();

   // LED initialization
   GPIO_DirOut(GPIO_LED0);
   GPIO_DirOut(GPIO_LED1);

   // Button initialization
   GPIO_DirIn(GPIO_BUTTON0);
   // Register an interrupt for Button 0
   GPIO_SetInterruptHandler(GPIO_BUTTON0, Button0Handler);
   GPIO_IntFall(GPIO_BUTTON0);
   GPIO_IntEnable(GPIO_BUTTON0);

   GPIO_DirIn(GPIO_BUTTON1);
   // Register an interrupt for Button 1
   GPIO_SetInterruptHandler(GPIO_BUTTON1, Button1Handler);
   GPIO_IntFall(GPIO_BUTTON1);
   GPIO_IntEnable(GPIO_BUTTON1);

   // Register X Endstops
   // Home
   GPIO_DirInPullDown(GPIO_MOTOR_X_HOME);
   GPIO_SetInterruptHandler(GPIO_MOTOR_X_HOME, EndstopXHandler);
   GPIO_IntRise(GPIO_MOTOR_X_HOME);
   GPIO_IntEnable(GPIO_MOTOR_X_HOME);
   // Stop
   GPIO_DirInPullDown(GPIO_MOTOR_X_STOP);
   GPIO_SetInterruptHandler(GPIO_MOTOR_X_STOP, EndstopXHandler);
   GPIO_IntRise(GPIO_MOTOR_X_STOP);
   GPIO_IntEnable(GPIO_MOTOR_X_STOP);

   // Register Y Endstops
   // Home
   GPIO_DirInPullDown(GPIO_MOTOR_Y_HOME);
   GPIO_SetInterruptHandler(GPIO_MOTOR_Y_HOME, EndstopYHandler);
   GPIO_IntRise(GPIO_MOTOR_Y_HOME);
   GPIO_IntEnable(GPIO_MOTOR_Y_HOME);
   // Stop
   GPIO_DirInPullDown(GPIO_MOTOR_Y_STOP);
   GPIO_SetInterruptHandler(GPIO_MOTOR_Y_STOP, EndstopYHandler);
   GPIO_IntRise(GPIO_MOTOR_Y_STOP);
   GPIO_IntEnable(GPIO_MOTOR_Y_STOP);

   // SPI initialization
   SpiSlave__PhyInit(spiConfig);

   // Register an interrupt for the GPIO_SPI_CS
   GPIO_SetInterruptHandler(GPIO_SPI_CS, IncomingData);
   GPIO_IntFall(GPIO_SPI_CS);
   GPIO_IntEnable(GPIO_SPI_CS);

   // Motor initialization
   Motor_InitAll();
#endif

   /* Infinite loop */
   while (1);
}

#if 0
// Timer Events //
void _1milliSecondEvent(void)
{
   if(GetMsTicks() % 250 == 0)
   {
      //GPIO_Not(GPIO_LED1);
   }

   // Check for SPI communications triggered
   if(SpiSlave__PhyTriggered())
   {
      // Clear trigger signal
      SpiSlave__PhyClearTrigger();
      SpiSlave_TriggerEventHandler();
   }
}

void _1SecondEvent(void)
{
   //GPIO_Not(GPIO_LED0);
}

// Button Events //
bool Button0Handler(void)
{
   if(!MotorStates[Motor_DeviceId_X].IsRunning)
   {
	   Motor_Run(Motor_DeviceId_X);
   }

   MotorStates[Motor_DeviceId_X].Frequency += 100;

   return true;
}

bool Button1Handler(void)
{
   MotorStates[Motor_DeviceId_X].Frequency -= 100;

   if((int16_t)MotorStates[Motor_DeviceId_X].Frequency < 100)
   {
      Motor_Stop(Motor_DeviceId_X);
      MotorStates[Motor_DeviceId_X].Frequency = 0;
   }

   return true;
}

// Endstop Events //
bool EndstopXHandler(void)
{
	Motor_Stop(Motor_DeviceId_X);
	return true;
}

bool EndstopYHandler(void)
{
	Motor_Stop(Motor_DeviceId_Y);
	return true;
}

// SPI Events //
bool IncomingData(void)
{
   //TODO: Anything in the TX buffer would go out here as the clock happens.

   // Open SPI Interface
   SpiSlave__PhyOpen();

   // Flag indicating SPI transaction started will be set.
   // This allows us to unroll out of interrupt context
   // quickly.
   return true;
}

void SpiComplete(void)
{
   //TODO: Parse the RX buffer and see how we did
}
#endif
