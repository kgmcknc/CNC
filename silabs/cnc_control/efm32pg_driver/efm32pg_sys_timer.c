#include <stdbool.h>
#include <stddef.h>
#include <sys_delay.h>

#include "sys_int.h"

#include "arch_system_time.h"

#include "em_device.h"

#include "efm32pg_sys_timer.h"

#define SYS_TIMER_PERIOD       (1000) // 1ms

static uint64_t fmsTicks = 0; // ms tick counter

SysTimer_Configuration_t fSysTimerConfig;

void SystemTimerInit(SysTimer_Configuration_t config)
{
   SysTick->LOAD  = (uint32_t)(HFPER_CLOCK/SYS_TIMER_PERIOD - 1UL);  /* set reload register */
   NVIC_SetPriority (SysTick_IRQn, (1UL << __NVIC_PRIO_BITS) - 1UL); /* set Priority for Systick Interrupt */
   SysTick->VAL   = 0UL;                                             /* Load the SysTick Counter Value */
   SysTick->CTRL  = SysTick_CTRL_CLKSOURCE_Msk |
                    SysTick_CTRL_TICKINT_Msk   |
                    SysTick_CTRL_ENABLE_Msk;                         /* Enable SysTick IRQ and SysTick Timer */

   fSysTimerConfig = config;
}

void SysTick_Handler(void)
{
   fmsTicks++;
   (fSysTimerConfig._1milliSecondEvent)();

   if(fmsTicks % 1000 == 0)
   {
      (fSysTimerConfig._1SecondEvent)();
   }
}

uint64_t GetMsTicks(void)
{
   return fmsTicks;
}
