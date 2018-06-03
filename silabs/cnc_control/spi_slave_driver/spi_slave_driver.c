#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

#include "sys_delay.h"
#include "sys_gpio.h"

#include "arch_system_time.h"

#include "prod_config.h"
#include "prod_gpio.h"

#include "spi_slave_driver.h"

#include "em_device.h"

#include "efm32pg_sys_timer.h"

#define DMA_CH_SPI_SLAVE_RX   (1)
#define DMA_CH_SPI_SLAVE_TX   (2)

#define SPISLAVE_BAUDRATE     (1000000)   // 1MHz

typedef struct {
   bool                        IsActive;
   bool                        Triggered;
   Arch_SystemTime_t           TriggerTimestamp;
} SpiSlave_State_t;

SpiSlave_State_t fSpiSlaveState;
SpiSlave_Configuration_t fSpiSlaveConfiguration;

static void fSpiSlaveCloseInterface(USART_TypeDef* Interface);
static void fSpiSlaveOpenInterface(USART_TypeDef* Interface, uint32_t baudrate);
static void fSpiSlaveEnableDmaTransmit(void);
static void fSpiSlaveEnableDmaReceive(void);

// Public Functions //

void SpiSlave__PhyInit(SpiSlave_Configuration_t config)
{
   // Enable USART1
   CMU->HFPERCLKEN0 |= CMU_HFPERCLKEN0_USART1;

   // Configure USART1
   fSpiSlaveOpenInterface(USART1, SPISLAVE_BAUDRATE);

   /* Configure USART1 pins */
   GPIO_DirIn(GPIO_SPI_MOSI);
   GPIO_DirOut(GPIO_SPI_MISO);
   GPIO_DirIn(GPIO_SPI_CLK);
   GPIO_DirIn(GPIO_SPI_CS);

   fSpiSlaveConfiguration = config;

   fSpiSlaveState.IsActive = false;
   fSpiSlaveState.Triggered = false;
}

void SpiSlave__PhyOpen(void)
{
   // Update trigger timestamp
   fSpiSlaveState.TriggerTimestamp = GetMsTicks();

   // Enable DMA
   CMU->HFCORECLKEN0 |= CMU_HFCORECLKEN0_DMA;
   DMA->CONFIG       |= DMA_CONFIG_EN;
   // Enable DMA TX
   fSpiSlaveEnableDmaTransmit();
   // Enable DMA RX
   fSpiSlaveEnableDmaReceive();

   // Clear USART registers
   USART1->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;

   // SPI transaction active
   fSpiSlaveState.IsActive = true;

   // Unroll from interrupt context
   fSpiSlaveState.Triggered = true;
}

void SpiSlave__PhyClose(void)
{
   // Close USART 0
   fSpiSlaveCloseInterface(USART1);
   CMU->HFPERCLKEN0 &= ~CMU_HFPERCLKEN0_USART1;
}

bool SpiSlave__PhyIsActive(void)
{
   return fSpiSlaveState.IsActive;
}

bool SpiSlave__PhyTriggered(void)
{
   return fSpiSlaveState.Triggered;
}

void SpiSlave__PhyClearTrigger(void)
{
   fSpiSlaveState.Triggered = false;
}

// Event Handlers //

void SpiSlave_TriggerEventHandler(void)
{
   // Waiting for data to transfer.  Should never take more than 30 msec.
   uint64_t timeout = fSpiSlaveState.TriggerTimestamp + 30;
   while (!GPIO_In(GPIO_SPI_CS) && (timeout > GetMsTicks()));

   // Allow DMA to finish. Timeout after 1ms
   timeout = GetMsTicks() + 20;
   while((((DMA->IF >> DMA_CH_SPI_SLAVE_RX) & 0x01) == 0x00) &&
           timeout > GetMsTicks());

   // SPI transaction complete
   fSpiSlaveState.IsActive = false;

   if (NULL != fSpiSlaveConfiguration.CompleteCallbackFPtr)
   {
      // Process Data
      (fSpiSlaveConfiguration.CompleteCallbackFPtr)();
   }
}

// Private Functions //

static void fSpiSlaveOpenInterface(USART_TypeDef* Interface, uint32_t baudrate)
{
   //clear out all the registers as good habit
   Interface->CMD      =  USART_CMD_RXDIS | USART_CMD_TXDIS | USART_CMD_MASTERDIS | USART_CMD_RXBLOCKDIS | USART_CMD_TXTRIDIS;
   Interface->CTRL     = _USART_CTRL_RESETVALUE;
   Interface->FRAME    = _USART_FRAME_RESETVALUE;
   Interface->TRIGCTRL = _USART_TRIGCTRL_RESETVALUE;
   Interface->CLKDIV   = _USART_CLKDIV_RESETVALUE;
   Interface->IEN      = _USART_IEN_RESETVALUE;
   Interface->IRCTRL   = _USART_IRCTRL_RESETVALUE;
   Interface->ROUTE    = _USART_ROUTE_RESETVALUE;

   // Setup baud rate for APB clock source
   // baudrate = HFPER_CLK/(2*(1+clkdiv/256))
   // clkdiv = (HFPER_CLK/baudrate - 2)*128
   uint32_t clkdiv;
   clkdiv = HFPER_CLOCK / baudrate - 2;
   clkdiv <<= 7;

   // Make sure we don't write to reserved bits
   // clkdiv must be less than 0x001FFFC0
   clkdiv &= _USART_CLKDIV_MASK;

   Interface->CLKDIV = clkdiv; // Set USART clock divider

   Interface->CTRL |= USART_CTRL_SYNC | // USART operates in synchronous SPI mode
                      USART_CTRL_TXBIL; // TXBL and TXBLIF are set when the transmit buffer
                                        // goes from full to half-full or empty. TXBL is
                                        // cleared when the buffer becomes full.

   //flush buffers and disable interrupts
   Interface->CMD = USART_CMD_CLEARRX | USART_CMD_CLEARTX;
   Interface->IEN = 0;

   // Configure USART
   Interface->CTRL  |= USART_CTRL_CLKPHA_SAMPLELEADING;
   Interface->CTRL  |= USART_CTRL_CLKPOL_IDLELOW;
   Interface->CTRL  |= USART_CTRL_MSBF;
   Interface->FRAME |= USART_FRAME_DATABITS_EIGHT;

   // Enable pins
   Interface->ROUTE = USART_ROUTE_TXPEN | USART_ROUTE_RXPEN | USART_ROUTE_CLKPEN | USART_ROUTE_CSPEN | USART_ROUTE_LOCATION_LOC1;
   Interface->CMD   = USART_CMD_TXEN | USART_CMD_RXEN;

   // Clear transmit and receive buffers
   Interface->CMD = USART_CMD_CLEARRX;
   Interface->CMD = USART_CMD_CLEARTX;

   /* Clear previous interrupts */
   Interface->IFC = _USART_IFC_MASK;
}

static void fSpiSlaveCloseInterface(USART_TypeDef* Interface)
{
   //clear out all the registers as good habit
   Interface->CTRL     = _USART_CTRL_RESETVALUE;
   Interface->FRAME    = _USART_FRAME_RESETVALUE;
   Interface->TRIGCTRL = _USART_TRIGCTRL_RESETVALUE;
   Interface->CLKDIV   = _USART_CLKDIV_RESETVALUE;
   Interface->IEN      = _USART_IEN_RESETVALUE;
   Interface->IRCTRL   = _USART_IRCTRL_RESETVALUE;
   Interface->ROUTE    = _USART_ROUTE_RESETVALUE;

   // Clear transmit and receive buffers
   Interface->CMD = USART_CMD_CLEARRX;
   Interface->CMD = USART_CMD_CLEARTX;
}

static void fSpiSlaveEnableDmaTransmit(void)
{
   // Enable TX
   DMA_DESCRIPTOR_TypeDef channelCfg;
   channelCfg.CTRL = DMA_CTRL_DST_INC_NONE                 // TXDATA register will not move
                  |  DMA_CTRL_DST_SIZE_BYTE                // Send single byte at a time
                  |  DMA_CTRL_SRC_INC_BYTE                 // Increment through TX buffer
                  |  DMA_CTRL_SRC_SIZE_BYTE                // Load single byte at a time
                  |  DMA_CTRL_DST_PROT_NON_PRIVILEGED      // Not high priority
                  |  DMA_CTRL_R_POWER_1                    // Arbitrate after 1 DMA transfer. Allows transaction to both send and receive.
                  |  ( 0 << _DMA_CTRL_N_MINUS_1_SHIFT)     /* set when activated */
                  |  ( 0 << _DMA_CTRL_NEXT_USEBURST_SHIFT) /* set when activated */
                  |  DMA_CTRL_CYCLE_CTRL_INVALID;          /* set when activated */

   // Route DMA signals from the peripheral function to a DMA channel
   DMA->CHPRIC = 1 << DMA_CH_SPI_SLAVE_TX; // clear high priority
   DMA->CH[DMA_CH_SPI_SLAVE_TX].CTRL = DMA_CH_CTRL_SOURCESEL_USART1 | DMA_CH_CTRL_SIGSEL_USART1TXBL;

   DMA_DESCRIPTOR_TypeDef* dmaChannel = (DMA_DESCRIPTOR_TypeDef *)DMA->CTRLBASE + DMA_CH_SPI_SLAVE_TX;
   dmaChannel->CTRL = channelCfg.CTRL;

   /* reload primary tx structure*/
   dmaChannel->SRCEND = fSpiSlaveConfiguration.TxDataBufferPtr + fSpiSlaveConfiguration.DataCount - 1;
   dmaChannel->DSTEND = (void*)&(USART1->TXDATA);

   dmaChannel->CTRL &= ~(_DMA_CTRL_CYCLE_CTRL_MASK | _DMA_CTRL_N_MINUS_1_MASK);
   dmaChannel->CTRL |= ((fSpiSlaveConfiguration.DataCount - 1) << _DMA_CTRL_N_MINUS_1_SHIFT) | DMA_CTRL_CYCLE_CTRL_BASIC;

   DMA->CHENS = 1 << DMA_CH_SPI_SLAVE_TX;
}

static void fSpiSlaveEnableDmaReceive(void)
{
   // Enable RX
   DMA_DESCRIPTOR_TypeDef channelCfg;
   channelCfg.CTRL = DMA_CTRL_DST_INC_BYTE                 // Increment through RX buffer
                   | DMA_CTRL_DST_SIZE_BYTE                // Receive single byte at a time
                   | DMA_CTRL_SRC_INC_NONE                 // RXDATA register will not move
                   | DMA_CTRL_SRC_SIZE_BYTE                // Receive single byte at a time
                   | DMA_CTRL_DST_PROT_NON_PRIVILEGED      // Not high priority
                   | DMA_CTRL_R_POWER_1                    // Arbitrate after 1 DMA transfer. Allows transaction to both send and receive.
                   | ( 0 << _DMA_CTRL_N_MINUS_1_SHIFT)     /* set when activated */
                   | ( 0 << _DMA_CTRL_NEXT_USEBURST_SHIFT) /* set when activated */
                   | DMA_CTRL_CYCLE_CTRL_INVALID;          /* set when activated */

   // Route DMA signals from the peripheral function to a DMA channel
   DMA->CHPRIC = 1 << DMA_CH_SPI_SLAVE_RX; // clear high priority
   DMA->CH[DMA_CH_SPI_SLAVE_RX].CTRL = DMA_CH_CTRL_SOURCESEL_USART1 | DMA_CH_CTRL_SIGSEL_USART1RXDATAV;

   DMA_DESCRIPTOR_TypeDef* dmaChannel = (DMA_DESCRIPTOR_TypeDef *)DMA->CTRLBASE + DMA_CH_SPI_SLAVE_RX;
   dmaChannel->CTRL = channelCfg.CTRL;

   /* reload primary tx structure*/
   dmaChannel->SRCEND = (void*)&(USART1->RXDATA);
   dmaChannel->DSTEND = fSpiSlaveConfiguration.RxDataBufferPtr + fSpiSlaveConfiguration.DataCount - 1;

   dmaChannel->CTRL &= ~(_DMA_CTRL_CYCLE_CTRL_MASK | _DMA_CTRL_N_MINUS_1_MASK);
   dmaChannel->CTRL |= ((fSpiSlaveConfiguration.DataCount - 1) << _DMA_CTRL_N_MINUS_1_SHIFT) | DMA_CTRL_CYCLE_CTRL_BASIC;

   DMA->CHENS = 1 << DMA_CH_SPI_SLAVE_RX;
}
