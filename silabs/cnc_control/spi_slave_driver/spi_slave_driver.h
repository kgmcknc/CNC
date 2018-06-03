/*
 * spi_slave_driver.h
 *
 *  Created on: May 18, 2018
 *      Author: RHOLLOWAY
 */

#ifndef SPI_SLAVE_DRIVER_SPI_SLAVE_DRIVER_H_
#define SPI_SLAVE_DRIVER_SPI_SLAVE_DRIVER_H_

#include <stdbool.h>

typedef void (*SpiSlave_CompleteCallback_t)(void);

typedef struct
{
   uint8_t*                    TxDataBufferPtr;        ///< Pointer to buffer of data to be transmitted.  Buffer has DataCount elements.
   uint8_t*                    RxDataBufferPtr;        ///< Pointer to buffer of data received.  Buffer has DataCount elements.
   uint16_t                    DataCount;              ///< Number of elements to be transmitted/received.  Expected number of SPI transactions.
   SpiSlave_CompleteCallback_t CompleteCallbackFPtr;   ///< Transaction complete callback function pointer.  Called from ISR context.
}  SpiSlave_Configuration_t;

void SpiSlave__PhyInit(SpiSlave_Configuration_t config);
void SpiSlave__PhyOpen(void);
void SpiSlave__PhyClose(void);

bool SpiSlave__PhyIsActive(void);
bool SpiSlave__PhyTriggered(void);
void SpiSlave__PhyClearTrigger(void);

void SpiSlave_TriggerEventHandler(void);

#endif /* SPI_SLAVE_DRIVER_SPI_SLAVE_DRIVER_H_ */
