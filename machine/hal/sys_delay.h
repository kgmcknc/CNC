/**
 * @file sys_delay.h
 *
 * @copyright 2013 by Sensus. All rights reserved.
 *
 * Functions used by components for spin lock timing delays.
 *
 */

#ifndef SYS_DELAY_H // {
#define SYS_DELAY_H

#include <stdint.h>
#include "em_device.h"

/*************************************************************************************************************
**************************************************************************************************************
  Name:       Sys_DelayCycle

  Purpose:    Inserts a single NOP delay in the code.

  PARAMETERS: none

  RETURNS:    none
*************************************************************************************************************/
#define Sys_DelayCycle() __NOP()

/**
 * Busy-waits at least the specified number of microseconds.
 */
void EFM32GG_DelayUs(uint32_t us);

/**
 * Busy-waits at least the specified number of milliseconds.
 */
void EFM32GG_DelayMs(uint32_t ms);

//************************************************************************************************************
//************************************************************************************************************
// SYSTEM DELAY MACROS

/*************************************************************************************************************
  Name:       Sys_FDelayNs

  Purpose:    This function generates a delay in nanoseconds truncated to the
              nearest cycle. Resolution is based on the specified CpuFreq in Hz.
              Note that a minimum delay of one cycle is automatically included
              between instructions regardless of how this function evaluates.

  PARAMETERS: TimeNs  (uint32_t) delay time in nanoseconds

  RETURNS:    none
*************************************************************************************************************/

#ifdef HANDLE_DELAYS_IN_LEGACY_MANNER
#define Sys_FDelayNs(TimeNs,CPUFreq) \
   Sys_DelayCycles((int32_t)((uint64_t) (TimeNs) * (uint64_t) (CPUFreq) / (uint64_t) 1000000000))
#else
#define Sys_FDelayNs(TimeNs,CPUFreq)                                                              \
      {                                                                                           \
      if ( CPUFreq == 13000000UL )                                                                \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeNs) * (uint64_t) (13000000) / (uint64_t) 1000000000)); \
         }                                                                                        \
      else                                                                                        \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeNs) * (uint64_t) (6500000) / (uint64_t) 1000000000));  \
         }                                                                                        \
      }
#endif

/*************************************************************************************************************
  Name:       Sys_FDelayUs

  Purpose:    This function generates a delay in microseconds to the nearest
              cycle. Resolution is based on the specified CpuFreq in Hz.

  PARAMETERS: TimeUs  (uint32_t) delay time in nanoseconds

  RETURNS:    none
*************************************************************************************************************/

#ifdef HANDLE_DELAYS_IN_LEGACY_MANNER
#define Sys_FDelayUs(TimeUs,CPUFreq)\
   Sys_DelayCycles((int32_t)((uint64_t) (TimeUs) * (uint64_t) (CPUFreq) / (uint64_t) 1000000))
#else
#define Sys_FDelayUs(TimeUs,CPUFreq)                                                              \
      {                                                                                           \
      if ( CPUFreq == 13000000UL )                                                                \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeUs) * (uint64_t) (13000000) / (uint64_t) 1000000));    \
         }                                                                                        \
      else                                                                                        \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeUs) * (uint64_t) (6500000) / (uint64_t) 1000000));     \
         }                                                                                        \
      }
#endif

/*************************************************************************************************************
  Name:       Sys_FDelayMs

  Purpose:    This function generates a delay in miliseconds to the nearest
              cycle. Resolution is based on the specified CpuFreq in Hz.

  PARAMETERS: TimeMs  (uint32_t) delay time in nanoseconds

  RETURNS:    none
*************************************************************************************************************/

#ifdef HANDLE_DELAYS_IN_LEGACY_MANNER
#define Sys_FDelayMs(TimeMs,CPUFreq) \
    Sys_DelayCycles((int32_t)((uint64_t) (TimeMs) * (uint64_t) (CPUFreq) / (uint64_t) 1000))
#else
#define Sys_FDelayMs(TimeMs,CPUFreq)                                                              \
      {                                                                                           \
      if ( CPUFreq == 13000000UL )                                                                \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeMs) * (uint64_t) (13000000) / (uint64_t) 1000));       \
         }                                                                                        \
      else                                                                                        \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeMs) * (uint64_t) (6500000) / (uint64_t) 1000));        \
         }                                                                                        \
      }
#endif

/*************************************************************************************************************
  Name:       Sys_FDelayS

  Purpose:    This function generates a delay in seconds to the nearest
              cycle. Resolution is based on the specified CpuFreq in Hz.

  PARAMETERS: TimeS   (uint32_t) delay time in nanoseconds

  RETURNS:    none
*************************************************************************************************************/

#ifdef HANDLE_DELAYS_IN_LEGACY_MANNER
#define Sys_FDelayS(TimeS,CPUFreq) \
   Sys_DelayCycles((int32_t)((uint64_t) (TimeMs) * (uint64_t) (CPUFreq) / (uint64_t) 1))
#else
#define Sys_FDelayS(TimeS,CPUFreq)                                                                \
      {                                                                                           \
      if ( CPUFreq == 13000000UL )                                                                \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeS) * (uint64_t) (13000000) / (uint64_t) 1));           \
         }                                                                                        \
      else                                                                                        \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeS) * (uint64_t) (6500000) / (uint64_t) 1));            \
         }                                                                                        \
      }
#endif

/*************************************************************************************************************
  Name:       Sys_DelayNs

  Purpose:    Generate a delay in nanoseconds truncated to the nearest cycle.
              Resolution is based on specified SYS_FREQ_MCU. Note that a minimum
              delay of one cycle is automatically included between instructions
              regardless of how this function evaluates.

  PARAMETERS: TimeNs  (uint32_t) delay time in nanoseconds

  RETURNS:    none
*************************************************************************************************************/

#ifdef HANDLE_DELAYS_IN_LEGACY_MANNER
#define Sys_DelayNs(TimeNs) \
   Sys_DelayCycles((int32_t)((uint64_t) (TimeNs) * (uint64_t) (SYS_FREQ_MCU) / (uint64_t) 1000000000))
#else
#define Sys_DelayNs(TimeNs)                                                                       \
      {                                                                                           \
      uint32_t CPUFreq = SYS_FREQ_MCU;                                                              \
      if ( CPUFreq == 13000000UL )                                                                \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeNs) * (uint64_t) (13000000) / (uint64_t) 1000000000)); \
         }                                                                                        \
      else                                                                                        \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeNs) * (uint64_t) (6500000) / (uint64_t) 1000000000));  \
         }                                                                                        \
      }
#endif

/*************************************************************************************************************
  Name:       Sys_DelayUs

  Purpose:    Delay in microseconds to the nearest cycle.  Resolution is based
              on specified SYS_FREQ_MCU.

  PARAMETERS: TimeUs  (uint32_t) delay time in microseconds

  RETURNS:    none
*************************************************************************************************************/

#define Sys_DelayUs(TimeUs) (EFM32GG_DelayUs(TimeUs * ( SYS_FREQ_MCU/(20000000UL) ) ) )

/*************************************************************************************************************
  Name:       Sys_DelayMs

  Purpose:    Delay in milliseconds to the nearest cycle. Resolution is based
              on specified SYS_FREQ_MCU.

  PARAMETERS: TimeMs  (uint32_t) delay time in milliseconds

  RETURNS:    none
*************************************************************************************************************/

#define Sys_DelayMs(TimeMs) (EFM32GG_DelayMs(TimeMs * ( SYS_FREQ_MCU/(20000000UL) ) ) )

/*************************************************************************************************************
  Name:       Sys_DelayS

  Purpose:    Delay in seconds to the nearest cycle using the Crystal.
              Resolution is based on specified SYS_FREQ_MCU.

  PARAMETERS: TimeS   (uint32_t) delay time in seconds

  RETURNS:    none
*************************************************************************************************************/

#ifdef HANDLE_DELAYS_IN_LEGACY_MANNER
#define Sys_DelayS(TimeS) \
   Sys_DelayCycles((int32_t)((uint64_t) (TimeS) * (uint64_t) (SYS_FREQ_MCU) / (uint64_t) 1))
#else
#define Sys_DelayS(TimeS)                                                                         \
      {                                                                                           \
      uint32_t CPUFreq = SYS_FREQ_MCU;                                                              \
      if ( CPUFreq == 13000000UL )                                                                \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeS) * (uint64_t) (13000000) / (uint64_t) 1));           \
         }                                                                                        \
      else                                                                                        \
         {                                                                                        \
         Sys_DelayCycles((int32_t)((uint64_t) (TimeS) * (uint64_t) (6500000) / (uint64_t) 1));            \
         }                                                                                        \
      }
#endif

#endif // SYS_DELAY_H }
