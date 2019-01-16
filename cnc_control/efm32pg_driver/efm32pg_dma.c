/*
 * efm32gg_dma.c
 *
 *  Created on: Mar 14, 2017
 *      Author: Rick.Holloway
 */
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "em_device.h"

#if 0
#include "efm32pg_dma.h"

// DMA Channel Descriptors (primary and alternate) SiLabs only ADH
// - Alignment and order are required by the DMACTRL module.
// - Alternate descriptors are required for ping-pong and scatter-gather cycles only.
/** DMA control block array, requires proper alignment. */
DMA_DESCRIPTOR_TypeDef dmaDescriptor[DMACTRL_CH_CNT * 2] __attribute__ ((aligned(DMACTRL_ALIGNMENT)));

void Efm32gg_DmaInit(void)
{
   // Make sure DMA channel descriptors are empty.  Could be residual left over from previous run.
   memset(dmaDescriptor, 0, sizeof(dmaDescriptor));

   CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_DMA;

   /* Disable DMA interrupts */
   NVIC_DisableIRQ(DMA_IRQn);

   /* Put the DMA controller into a known state, first disabling it. */
   DMA->CONFIG      = _DMA_CONFIG_RESETVALUE;
   DMA->CHUSEBURSTC = _DMA_CHUSEBURSTC_MASK;
   DMA->CHREQMASKC  = _DMA_CHREQMASKC_MASK;
   DMA->CHENC       = _DMA_CHENC_MASK;
   DMA->CHALTC      = _DMA_CHALTC_MASK;
   DMA->CHPRIC      = _DMA_CHPRIC_MASK;
   DMA->ERRORC      = DMA_ERRORC_ERRORC;
   DMA->IEN         = _DMA_IEN_RESETVALUE;
   DMA->IFC         = _DMA_IFC_MASK;

   /* Clear channel control flags */
   for (int i = 0; i < DMA_CHAN_COUNT; i++) 
   {
      DMA->CH[i].CTRL = _DMA_CH_CTRL_RESETVALUE;
   }

   // Setup and enable the DMA controller
   DMA->CTRLBASE = (uint32_t) dmaDescriptor;
   DMA->CONFIG &= ~DMA_CONFIG_CHPROT;
}
#endif
