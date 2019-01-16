#ifndef ARCH_SYSTEM_TIME_H // {
#define ARCH_SYSTEM_TIME_H

#include <stdint.h>

#include "prod_config.h"

#define ARCH_SYSTEM_TICKS_PER_SEC   (SYS_FREQ_XTAL_LF)
#define ARCH_SYSTEM_TICKS_PER_MS    (SYS_FREQ_XTAL_LF/1000)

/**
 * Type for system tick which is base unit for representation of time.
 */
typedef int64_t Arch_SystemTick_t;

/**
 * Type for system time which is the number of system ticks since last reset.
 */
typedef uint64_t Arch_SystemTime_t;

#define ARCH_SYSTEM_TICK_MAX (INT64_MAX)
#define ARCH_SYSTEM_TIME_MAX (UINT64_MAX)

/**
 * Type for system tick with reduced range.
 *
 * Used for system tick counts that are equivalent to less than 1 minute.
 */
typedef int32_t Arch_SysTick_Short_t;

// General Purpose Time Bases
#define ARCH_TIME_US_PER_SECOND      (1000000L)
#define ARCH_TIME_MS_PER_SECOND      (1000)
#define ARCH_TIME_SECONDS_PER_MINUTE (60)
#define ARCH_TIME_MINUTES_PER_HOUR   (60)
#define ARCH_TIME_HOURS_PER_DAY      (24)
#define ARCH_TIME_DAYS_PER_WEEK      (7)

#define ARCH_TIME_SECONDS_PER_HOUR   ( (uint32_t)( (ARCH_TIME_MINUTES_PER_HOUR) * (ARCH_TIME_SECONDS_PER_MINUTE) ) )
#define ARCH_TIME_SECONDS_PER_DAY    ( (uint32_t)( (ARCH_TIME_HOURS_PER_DAY) *    (ARCH_TIME_SECONDS_PER_HOUR)   ) )
#define ARCH_TIME_SECONDS_PER_WEEK   ( (uint32_t)( (ARCH_TIME_DAYS_PER_WEEK) *    (ARCH_TIME_SECONDS_PER_DAY)    ) )
#define ARCH_TIME_MINUTES_PER_DAY    ( (uint32_t)( (ARCH_TIME_HOURS_PER_DAY) *    (ARCH_TIME_MINUTES_PER_HOUR)   ) )
#define ARCH_TIME_MINUTES_PER_WEEK   ( (uint32_t)( (ARCH_TIME_DAYS_PER_WEEK) *    (ARCH_TIME_MINUTES_PER_DAY)    ) )
#define ARCH_TIME_HOURS_PER_WEEK     ( (uint32_t)( (ARCH_TIME_DAYS_PER_WEEK) *    (ARCH_TIME_HOURS_PER_DAY)      ) )

#define ARCH_SYSTEM_TICKS_PER_MIN     ( (uint32_t)( (ARCH_SYSTEM_TICKS_PER_SEC) *   (ARCH_TIME_SECONDS_PER_MINUTE) ) )
#define ARCH_SYSTEM_TICKS_PER_HOUR    ( (uint64_t)( (ARCH_SYSTEM_TICKS_PER_SEC) *   (ARCH_TIME_SECONDS_PER_HOUR)   ) )
#define ARCH_SYSTEM_TICKS_PER_DAY     ( (uint64_t)( (uint64_t)(ARCH_SYSTEM_TICKS_PER_SEC) * (uint64_t)(ARCH_TIME_SECONDS_PER_DAY)  ) )
#define ARCH_SYSTEM_TICKS_PER_WEEK    ( (uint64_t)( (uint64_t)(ARCH_SYSTEM_TICKS_PER_SEC) * (uint64_t)(ARCH_TIME_SECONDS_PER_WEEK) ) )

#define ARCH_USECS_TO_SYSTEM_TICKS(usecs) (Arch_SystemTime_t)( ((uint64_t)(usecs) * ARCH_SYSTEM_TICKS_PER_SEC) / ARCH_TIME_US_PER_SECOND)
#define ARCH_MSECS_TO_SYSTEM_TICKS(msecs) (Arch_SystemTime_t)( ((uint64_t)(msecs) * ARCH_SYSTEM_TICKS_PER_SEC) / ARCH_TIME_MS_PER_SECOND)
#define ARCH_SECONDS_TO_SYSTEM_TICKS(secs)(Arch_SystemTime_t)( (secs)  * ARCH_SYSTEM_TICKS_PER_SEC)
#define ARCH_MINUTES_TO_SYSTEM_TICKS(mins)(Arch_SystemTime_t)( (mins)  * ARCH_SYSTEM_TICKS_PER_MIN)
#define ARCH_HOURS_TO_SYSTEM_TICKS(hours) (Arch_SystemTime_t)( (hours) * ARCH_SYSTEM_TICKS_PER_HOUR)
#define ARCH_DAYS_TO_SYSTEM_TICKS(days)   (Arch_SystemTime_t)( (days)  * ARCH_SYSTEM_TICKS_PER_DAY)
#define ARCH_WEEKS_TO_SYSTEM_TICKS(weeks) (Arch_SystemTime_t)( (weeks) * ARCH_SYSTEM_TICKS_PER_WEEK)


/**
 * This function provides an interface to read the present
 * system time.  The system time is started at power up and
 * continuously runs for the life of the product.
 *
 * @return number of system ticks since last reset.
 */
Arch_SystemTime_t  Arch_GetSystemTime(void);

#endif // ARCH_SYSTEM_TIME_H }
