/*
 * common_spi.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#ifndef SRC_COMMON_SPI_H_
#define SRC_COMMON_SPI_H_

#include <stdint.h>

#define MAX_SPI_TRANSFER 4096
#define MAX_SPI_LENGTH 256

#define OPCODE_LENGTH 1
#define SPI_SIZE_LENGTH 2
#define SPI_HEADER_LENGTH (OPCODE_LENGTH*4 + SPI_SIZE_LENGTH*2)
#define SPI_DATA_LENGTH (MAX_SPI_LENGTH - SPI_HEADER_LENGTH)

#define SPI_RESET_STRING "RRRRR"
#define SPI_MASTER_INIT_STRING "MHere"
#define SPI_SLAVE_INIT_STRING "SHere"
#define SPI_IDLE_STRING "XXXXX"
#define IDLE_LENGTH (SPI_HEADER_LENGTH)

/*************************************************
Packet Structure
Byte 0   - Current Write Opcode
Byte 1   - Next Write Opcode
Byte 2   - Current Read Opcode
Byte 3   - Next Read Opcode
Byte 4-5 - Current Write Size
Byte 6-7 - Total Write Size
Byte 8-X - Packet Data
**************************************************/
/*
must define themselves
	spi_get_reset_pin     // slave
	spi_set_reset_pin     // master
	spi_clear_reset_pin   // master
	spi_set_request_pin   // slave
	spi_clear_request_pin // slave
	spi_transfer_data     // master and slave

must call periodically
	handle_spi(void)

must call when finished transfer (whether interrupt like in slave or immediate like master)
	spi_finish_transfer(data, length)

available to interact with
	// for master and slave
	// can do simultaneous read and write, but can't do write while write in progress or read while read in progress... essentially no write/read queue
	spi_set_read();    // this sets string input pointer for data read from slave. size is max spi size. length handled internally, returns 0 if read already running and new read didn't take. returns 1 if all good
	spi_set_write();   // this sets data and size to be written for the transaction. won't apply if another write in progress, returns 0 if write already running and new write didn't take. returns 1 if all good
	spi_check_write(); // this returns 0 if transfer hasn't finished, returns 1 if transfer is complete.
	spi_check_read();  // this returns 0 if transfer hasn't finished, returns 1 if transfer is complete.

i handle read and write between slave. they set size and I transfer in as many as needed and then set flag whenever it's fully done
could be one transfer or mutliple transfers to complete. hidden from interfaces
could add checksum if wanted for verification/error handling... but that adds a lot handling storing old packets and doing ack/nack... day 2
*/

enum spi_opcodes {
	request_reset,
	read_idle,
	read_ready,
	write_idle,
	start_single_write,
	start_multi_write,
	continue_write,
	finish_write
};

enum spi_states {
	spi_reset,
	spi_initialized,
	spi_running
};

enum spi_transfer_states {
	transfer_idle,
	transfer_ready,
	transfer_requested,
	transfer_in_progress,
	transfer_processing,
	transfer_complete
};

struct spi_struct {
	enum spi_states state;     // running states...
	uint8_t initialized;       // if spi struct is initialized
	uint8_t connected;         // if master and slave are connected
	uint8_t reset_request;     // if reset reconnect requested
	uint8_t reset_complete;    // if reset reconnect comlete
	uint8_t transfer_pending;  // if we've sent the transfer
	uint8_t transfer_finished; // if transfer has completed
	uint8_t transfer_request;  // if a transfer has been requested from the slave
	uint8_t transfer_pin;      // level indicator of last read transfer request pin state (high/low)
	uint8_t read_requested;    // if user has requested read
	uint8_t read_in_progress;  // if user read is being processed
	uint8_t read_complete;     // user read is completed - cleared on data read
	uint8_t write_requested;   // if user has requested write
	uint8_t write_in_progress; // if user write is being processed
	uint8_t write_complete;    // user write is complete - cleared on check
	enum spi_transfer_states read_state;   // read states...
	enum spi_transfer_states write_state;   // write states...
	uint32_t read_length;
	uint32_t read_remaining;
	uint32_t write_length;
	uint32_t write_remaining;
	uint32_t transfer_length;
	char spi_data[MAX_SPI_LENGTH]; // temporary transfer buffer of max packet size
	char write_data[MAX_SPI_TRANSFER]; // maximum transfer size buffer
	char read_data[MAX_SPI_TRANSFER];  // maximum transfer size buffer
};

#define SPI_STRUCT_INIT {\
	spi_reset,           \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	transfer_idle,       \
	transfer_idle,       \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	0,                   \
	{0},                 \
	{0},                 \
	{0}                  \
};

// MUST ADD DEFINE FOR SPI_SLAVE or SPI_MASTER IN YOUR CODE!
// example functions...
// define your own gpio control in these functions
/*
uint8_t spi_get_reset_pin(void){
	return GPIO_PinInGet(SPI_RESET_PORT, SPI_RESET_PIN);
}
void spi_set_reset_pin(void){
	GPIO_PinOutSet(SPI_REQUEST_PORT, SPI_REQUEST_PIN);
}
void spi_clear_reset_pin(void){
	GPIO_PinOutClear(SPI_REQUEST_PORT, SPI_REQUEST_PIN);
}
void spi_set_request_pin(void){
	GPIO_PinOutSet(SPI_REQUEST_PORT, SPI_REQUEST_PIN);
}
void spi_clear_request_pin(void){
	GPIO_PinOutClear(SPI_REQUEST_PORT, SPI_REQUEST_PIN);
}
void set_spi_transfer(char* spi_data, uint16_t data_length){
	SPIDRV_STransfer(handle, spi->spi_data, spi->spi_data, strlen(SPI_RESET_STRING), slave_transfer_done, 0);
}
void init_spi_driver(void){
	initData.portLocationClk = SPI_CLK_LOC;
	initData.portLocationCs = SPI_CS_LOC;
	initData.portLocationRx = SPI_MOSI_LOC;
	initData.portLocationTx = SPI_MISO_LOC;
	initData.dummyTxValue = 'X';
	SPIDRV_Init( handle, &initData );
}
*/

/**********************************************************************/
// begin user defined functions
#ifdef SPI_MASTER
uint8_t spi_get_request_pin(void);
void spi_set_reset_pin(void);
void spi_clear_reset_pin(void);
#endif
#ifdef SPI_SLAVE
uint8_t spi_get_reset_pin(void);
void spi_set_request_pin(void);
void spi_clear_request_pin(void);
#endif
//common
void spi_transfer_data(char* spi_data, uint16_t data_length);
// end user defined functions
/**********************************************************************/

/**********************************************************************/
// begin user interface functions
// common for master and slave
uint8_t handle_spi(void); // must call periodically in main or something
void reset_spi(void);
// maybe make get status function to see spy state?
void spi_finish_transfer(char* spi_data, uint16_t data_length); //must call when finished transfer (whether interrupt like in slave or immediate like master)
// can do simultaneous read and write, but can't do write while write in progress or read while read in progress... essentially no write/read queue
int32_t spi_set_read(void);    // this sets string input pointer for data read from slave. size is max spi size. length handled internally, returns 0 if read already running and new read didn't take. returns 1 if all good
int32_t spi_set_write(char* spi_data, uint16_t data_length);   // this sets data and size to be written for the transaction. won't apply if another write in progress, returns 0 if write already running and new write didn't take. returns 1 if all good
int32_t spi_check_read(char* spi_data);  // this returns 0 if transfer hasn't finished, returns 1 if transfer is complete.
int32_t spi_check_write(void); // this returns 0 if transfer hasn't finished, returns 1 if transfer is complete.
// end user interface functions
/**********************************************************************/

/**********************************************************************/
// begin backend functions
void spi_check_request(struct spi_struct* spi);
void spi_set_reset(struct spi_struct* spi);
void spi_clear_reset(struct spi_struct* spi);
void spi_check_reset(struct spi_struct* spi);
void spi_set_request_ready(struct spi_struct* spi);
void spi_clear_request_ready(struct spi_struct* spi);
void spi_set_pending_transfer(struct spi_struct* spi);
void spi_clear_pending_transfer(struct spi_struct* spi);

void init_spi_struct(struct spi_struct* spi);
void handle_spi_reset(struct spi_struct* spi);
void clear_pending_transfer(struct spi_struct* spi);
void send_connect(struct spi_struct* spi);
void get_new_transfer(struct spi_struct* spi);
void handle_transfer(struct spi_struct* spi);
void spi_prepare_transfer(struct spi_struct* spi);
void spi_parse_transfer(struct spi_struct* spi);

enum spi_opcodes spi_parse_opcode(char* opcode_string);
int32_t spi_string_to_int32(char* length_string, uint8_t byte_length);
// end backend functions
/**********************************************************************/

#endif /* SRC_COMMON_SPI_H_ */
