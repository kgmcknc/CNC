#ifndef EFM32GG_SYS_TIMER_H  // {
#define EFM32GG_SYS_TIMER_H

#include <stdint.h>
#include <stdbool.h>

typedef void (*SysTimerCallbackFunction_t)(void);

typedef struct
{
   SysTimerCallbackFunction_t _1milliSecondEvent;
   SysTimerCallbackFunction_t _1SecondEvent;
} SysTimer_Configuration_t;

void                     SystemTimerInit(SysTimer_Configuration_t config);
uint64_t        GetMsTicks(void);

#endif
