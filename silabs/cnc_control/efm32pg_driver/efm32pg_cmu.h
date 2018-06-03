/**
 * @file
 *
 * @copyright 2016 by Sensus. All rights reserved.
 *
 * EFM32GG Clock Components
 */

#ifndef EFM32GG_CMU_H_
#define EFM32GG_CMU_H_

/**
 * @addtogroup clocks EFM32GG Clocking
 * @{
 */

/**
 * @brief Initialize EFM32GG clocks for default operation
 *
 * Enables default oscillators:
 *    - Internal HFRCO (default) - 14MHz
 *    - External LFXO - 32kHz
 *
 * Increases, tunes, and calibrates internal HFRCO to 21MHz as normal operating
 * core speed. See also Efm32gg_HfrcoCal().
 *
 * Once clocks are active enable HFRCO to high frequency peripherals and
 * enable low frequency modules. Low frequency peripherals are referenced from
 * the LFXO so they can maintain functionality in deep sleep.
 *
 * @note See EFM32GG errata for details on an issue regarding the LFXO
 *       at high temperatures.
 */
void Efm32pg_CmuInit(void);
/** @} */


#endif /* EFM32GG_CMU_H_ */
