#include <stddef.h>
#include <stdbool.h>

#include "sys_gpio.h"

static GPIO_InterruptHandler_t fInterruptHandler[16] =
{
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL,
      NULL
};

bool GPIO_SetInterruptHandler(Sys_GpioPin_t pin, GPIO_InterruptHandler_t functionCallBack)
{
   bool result = false;

   if (functionCallBack != NULL)
   {
      GPIO->IEN &= ~(1 << pin.pinNum);                          // Disable pin interrupt
      if (pin.pinNum < 8)
      {
         GPIO->EXTIPSELL |= pin.portNum << (4*pin.pinNum);      // Select interrupt by port and pin number
      }
      else
      {
         GPIO->EXTIPSELH |= pin.portNum << (4*(pin.pinNum-8));  // Select interrupt by port and pin number
      }
      fInterruptHandler[pin.pinNum] = functionCallBack;         // Assign callback function to the external interrupt pin
      result = true;
   }
   return(result);
}

void GPIO_IntEnable(Sys_GpioPin_t pin)
{
   GPIO->IEN |= (1 << pin.pinNum);    // enable pin interrupt
}

void GPIO_IntDisable(Sys_GpioPin_t pin)
{
   GPIO->IEN &= ~(1 << pin.pinNum);   // disable pin interrupt
}

void GPIO_IntRise(Sys_GpioPin_t pin)
{
   GPIO->IEN &= ~(1 << pin.pinNum);   // disable pin interrupt
   GPIO->EXTIRISE |= 1 << pin.pinNum; // enable rising edge trigger
   GPIO->IFC = 1 << pin.pinNum;       // clear pin interrupt flag
}

void GPIO_IntFall(Sys_GpioPin_t pin)
{
   GPIO->IEN &= ~(1 << pin.pinNum);   // disable pin interrupt
   GPIO->EXTIFALL |= 1 << pin.pinNum; // enable falling edge trigger
   GPIO->IFC = 1 << pin.pinNum;       // clear pin interrupt flag
}

void GPIO_IntRiseFall(Sys_GpioPin_t pin)
{
   GPIO->IEN &= ~(1 << pin.pinNum);   // disable pin interrupt
   GPIO->EXTIRISE |= 1 << pin.pinNum; // enable rising edge trigger
   GPIO->EXTIFALL |= 1 << pin.pinNum; // enable falling edge trigger
   GPIO->IFC = 1 << pin.pinNum;       // clear pin interrupt flag
}

void GPIO_IRQHandler(void)
{
   // loop until the interrupt flags are all cleared in case
   // multiple interrupts occur near each other.
   while(GPIO->IF)
   {
      uint16_t count = 1;
      uint16_t interrupt = 0;

      // due to the way interrupt flags are cycled through the lowest numbered pin
      // will have the highest priority
      while(!(GPIO->IF & count))
      {
         count = count << 1;             // shift through interrupt flags to find active interrupt
         interrupt++;                    // increment index for interrupt handler callback
      }
      GPIO->IFC = 1 << interrupt;        // clear serviced interrupt flag
      if(fInterruptHandler[interrupt] != NULL)
         fInterruptHandler[interrupt](); // if there is a callback available use it
   }
}

void GPIO_ODD_IRQHandler(void)
{
   // both even and odd handlers fall through to a common
   // GPIO IRQ handler
   GPIO_IRQHandler();
}

void GPIO_EVEN_IRQHandler(void)
{
   // both even and odd handlers fall through to a common
   // GPIO IRQ handler
   GPIO_IRQHandler();
}
