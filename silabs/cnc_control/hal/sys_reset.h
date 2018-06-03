/**
 * @file
 *
 * sim3l1xx reset functions.
 */

#ifndef SYS_RESET_H // {
#define SYS_RESET_H

#include <stdint.h>

#include "efm32gg.h"

#define Sys_Reset()  NVIC_SystemReset()

#endif // SYS_RESET_H }
