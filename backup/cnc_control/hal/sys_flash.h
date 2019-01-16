/**
 * @file
 *
 * MSP430F5437 internal flash definitions.
 */

#ifndef SYS_FLASH_H // {
#define SYS_FLASH_H

#include <stdint.h>

/**
 * Lowest address of internal flash memory.
 */
#define SYS_FLASH_START_ADDR          (0x00000000UL)

/**
 * Highest address of internal flash memory.
 */
#define SYS_FLASH_END_ADDR            (0x0000FFFFFUL)

/**
 * Size of internal flash page/segment/block in bytes.
 */
#define SYS_FLASH_PAGE_SIZE           (0x1000UL)

#endif // SYS_FLASH_H }
