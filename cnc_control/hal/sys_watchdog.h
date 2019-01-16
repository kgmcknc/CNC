/**
 * @file sys_watchdog.h
 *
 * @copyright 2013 by Sensus. All rights reserved.
 *
 * Functions used by components for control of microprocessor watch dog timer.
 *
 */

#ifndef SYS_WATCHDOG_H // {
#define SYS_WATCHDOG_H

#include "prod_config.h"


#ifdef ENABLE_WATCHDOG

#include "efm32gg_watchdog.h"

#define Sys_WDogDisable()                Efm32gg_WatchDogDisable()

#define Sys_WDogEnable()

#define Sys_WDogInit()                   Efm32gg_WatchDogInit()
 
#define Sys_WDogReset()                  Efm32gg_WatchDogReset()

#define Sys_WDogResetLong()              Efm32gg_WatchDogReset()

#define Sys_WDogResetLonger()            Efm32gg_WatchDogReset()

#else

#define Sys_WDogDisable()

#define Sys_WDogEnable()

#define Sys_WDogInit()
 
#define Sys_WDogReset()

#define Sys_WDogResetLong()

#define Sys_WDogResetLonger()

#endif // ENABLE_WATCHDOG
#endif // SYS_WATCHDOG_H }
