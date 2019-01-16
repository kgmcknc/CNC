/*
 * efm32gg_dma.h
 *
 *  Created on: Mar 14, 2017
 *      Author: Rick.Holloway
 */

#ifndef EFM32GG_DMA_H_
#define EFM32GG_DMA_H_

#include "arch_system_time.h"

#define DMACTRL_CH_CNT             16
#define DMACTRL_ALIGNMENT          256

void Efm32gg_DmaInit(void);

#endif /* EFM32GG_DMA_H_ */
