#ifndef PROD_CONFIG_H // {
#define PROD_CONFIG_H

#include <stdint.h>
#include <stdbool.h>

// Clock configuration
#define SYS_FREQ_XTAL_LF     (32768UL)
#define RTC_CLOCK_FREQ       (SYS_FREQ_XTAL_LF)
#define SYS_FREQ_MCU         (21000000UL)
#define HFPER_CLOCK          (SYS_FREQ_MCU)

#endif // PROD_CONFIG_H }

