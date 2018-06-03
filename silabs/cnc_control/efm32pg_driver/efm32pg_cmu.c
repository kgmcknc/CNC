#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include "em_device.h"

#include "efm32pg_cmu.h"

#include "sys_gpio.h"
#include "prod_config.h"
#include "prod_gpio.h"

void Efm32pg_CmuInit(void)
{
#if 0
   // Disable HFXO before configuring
   CMU->OSCENCMD = CMU_OSCENCMD_HFXODIS;

   // Wait for HFXO CTRL to be ready for a new value
   while(CMU->SYNCBUSY & CMU_SYNCBUSY_HFXOBSY);

   // Initialize HF External Oscillator
   // Set HFXO source to external oscillator
   CMU->HFXOCTRL = CMU_HFXOCTRL_MODE_XTAL;

   // Set tuning for startup and steady state
   CMU->HFXOSTARTUPCTRL = CMU_HFXOSTARTUPCTRL_CTUNE_DEFAULT |
                          CMU_HFXOSTARTUPCTRL_IBTRIMXOCORE_DEFAULT;
   CMU->HFXOSTEADYSTATECTRL = CMU_HFXOSTEADYSTATECTRL_CTUNE_DEFAULT        |
                              CMU_HFXOSTEADYSTATECTRL_IBTRIMXOCORE_DEFAULT |
                              CMU_HFXOSTEADYSTATECTRL_REGISH_DEFAULT;
   //WARNING: there are a couple of other registers here that need to be initialized that prevent the HFXO from starting up.

   // Set timeouts
   CMU->HFXOTIMEOUTCTRL = CMU_HFXOTIMEOUTCTRL_PEAKDETTIMEOUT_32KCYCLES |
                          CMU_HFXOTIMEOUTCTRL_STEADYTIMEOUT_256CYCLES  |
                          CMU_HFXOTIMEOUTCTRL_STARTUPTIMEOUT_4KCYCLES  |
                          CMU_HFXOTIMEOUTCTRL_SHUNTOPTTIMEOUT_16CYCLES;
#endif

   // Enable HF External Oscillator
   CMU->OSCENCMD = CMU_OSCENCMD_HFXOEN;  //Defaults to 40MHz

   // Wait for HFXO to be ready
   while((CMU->STATUS & CMU_STATUS_HFXORDY));

   // Select HFXO as HF clock source
   CMU->HFCLKSEL = CMU_HFCLKSEL_HF_HFXO;

   // Enable HF peripheral clock
   CMU->CTRL = CMU_CTRL_HFPERCLKEN;

   // Enable clock to GPIOs
   CMU->HFBUSCLKEN0 = CMU_HFBUSCLKEN0_GPIO;

   // Output HFXO to PD9
   GPIO_DirOut(GPIO_CLKOUT0);
   CMU->CTRL |= CMU_CTRL_CLKOUTSEL0_HFSRCCLK;
   CMU->ROUTELOC0 = CMU_ROUTELOC0_CLKOUT0LOC_LOC4;
   CMU->ROUTEPEN = CMU_ROUTEPEN_CLKOUT0PEN;
}
