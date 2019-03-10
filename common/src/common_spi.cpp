/*
 * common_spi.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "common_spi.h"

struct spi_struct spi = SPI_STRUCT_INIT;

// MUST ADD DEFINE FOR SPI_SLAVE or SPI_MASTER IN YOUR CODE!

#ifdef SPI_MASTER
void spi_check_slave_request(struct spi_struct* spi){
	if(spi->transfer_request){
		//printf("transfer already set\n");
		// if transfer is already been detected... don't recheck until master has sent data
	} else {
		uint8_t new_pin_level = spi_get_slave_request_pin(); // must be defined in your user code. See header
		//printf("saved level: %d, pin level: %d\n", spi->transfer_pin, new_pin_level);
		// toggle indicates slave trsansfer set
		if(spi->transfer_pin){
			if(new_pin_level){
				spi->transfer_request = 0;
				//printf("detected both high\n");
			} else {
				spi->transfer_request = 1;
				//printf("detected toggle high to low\n");
			}
		} else {
			if(new_pin_level){
				spi->transfer_request = 1;
				//printf("detected toggle low to high\n");
			} else {
				spi->transfer_request = 0;
				//printf("detected both low\n");
			}
		}
		spi->transfer_pin = new_pin_level;
	}
}
void spi_set_master_request(struct spi_struct* spi){
	spi_set_master_request_pin(); // must be defined in your user code. See header
	// used to be reset pin
	//	spi->reset_request = 1;
}
void spi_clear_master_request(struct spi_struct* spi){
	spi_clear_master_request_pin(); // must be defined in your user code. See header
	// used to be reset pin
	//spi->reset_request = 0;
}
#endif
#ifdef SPI_SLAVE
void spi_check_master_request(struct spi_struct* spi){
	spi_get_master_request_pin(); // must be defined in your user code. See header
	// used to be reset pin
	//spi->reset_request = 0;
}
void spi_set_slave_request(struct spi_struct* spi){
	if(spi->transfer_request){
		// don't toggle if transfer already requested...
	} else {
		// toggle on the pin indicates spi is set
		if(spi->transfer_pin){
			spi_clear_slave_request_pin(); // must be defined in your user code. See header
			spi->transfer_pin = 0;
		} else {
			spi_set_slave_request_pin(); // must be defined in your user code. See header
			spi->transfer_pin = 1;
		}
		spi->transfer_request = 1;
	}
}
void spi_clear_slave_request(struct spi_struct* spi){
	// only used to reset to default zero;
	spi_clear_slave_request_pin(); // must be defined in your user code. See header
}
void spi_clear_slave_request_ready(struct spi_struct* spi){
	spi->transfer_request = 0;
}
#endif
void spi_check_reset(struct spi_struct* spi){
	#ifdef SPI_SLAVE
	if(spi->state == spi_reset){
		spi->reset_detect_count = 0;
	} else {
		// must be defined in your user code. See header
		if(spi_get_master_request_pin()){ // change the request structure for master... add master rand slave request flags
			spi->reset_detect_count++;
		} else {
			// reset count if slave drops request
			spi->reset_detect_count = 0;
		}
		if(spi->reset_detect_count >= SPI_RESET_COUNT_VALUE){
			// reached maximum value with master request high...
			spi->reset_detect_count = 0;
			spi->state = spi_reset;
			spi->transfer_pin = 0;
			spi->transfer_pending = 0;
			spi->transfer_finished = 0;
			spi->transfer_request = 0;
			spi_clear_slave_request_pin();
		}
	}
	#endif
}
void spi_set_pending_transfer(struct spi_struct* spi){
	spi->transfer_pending = 1;
}
void spi_clear_pending_transfer(struct spi_struct* spi){
	spi->transfer_pending = 0;
}
void spi_clear_finished_transfer(struct spi_struct* spi){
	spi->transfer_finished = 0;
}
void spi_finish_transfer(char* spi_data, uint16_t data_length){
	spi.transfer_finished = 1;
	spi.transfer_length = data_length;
	memcpy(spi.spi_data, spi_data, data_length);
	spi_data[data_length] = '\0';
}

int32_t spi_set_read(void){
	if(spi.read_in_progress){
		// can't do another read...
		return -1;
	} else {
		spi.read_requested = 1;
		return 1;
	}
}
int32_t spi_set_write(char* spi_data, uint16_t data_length){
	if(spi.write_in_progress || spi.write_requested){
		// can't do another write...
		return -1;
	} else {
		memcpy(spi.write_data, spi_data, data_length);
		spi.write_length = data_length;
		spi.write_requested = 1;
		return 1;
	}
}
int32_t spi_check_read(char* spi_data){
	int32_t data_length = 0;
	if(spi.read_in_progress || spi.read_requested){
		if(spi.read_complete){
			memcpy(spi_data, spi.read_data, spi.read_length);
			#ifdef SPI_MASTER
			//printf("read data: %s, %d\n", spi_data, spi.read_length);
			#endif
			data_length = spi.read_length;
			spi.read_in_progress = 0;
			spi.read_complete = 0;
			spi.read_length = 0;
			spi.read_remaining = 0;
			spi.read_state = transfer_idle;
			return data_length;
		} else {
			// read isn't finished yet
			return -1;
		}
	} else {
		// not reading, so read is complete
		return 0;
	}
}
int32_t spi_check_write(void){
	if(spi.write_in_progress){
		if(spi.write_complete){
			spi.write_in_progress = 0;
			spi.write_complete = 0;
			spi.write_length = 0;
			spi.write_remaining = 0;
			spi.write_state = transfer_idle;
			#ifdef SPI_MASTER
				//printf("Spi write complete\n");
			#endif
			return 1;
		} else {
			// write isn't finished yet
			return -1;
		}
	} else {
		// not writing, so write complete
		return 0;
	}
}
void spi_reconnect(void){
	spi.state = spi_reset;
}

void init_spi_struct(struct spi_struct* spi){
	spi->connected = 0;
	spi->read_complete = 0;
	spi->read_in_progress = 0;
	spi->read_length = 0;
	spi->read_remaining = 0;
	spi->read_requested = 0;
	spi->read_state = transfer_idle;
	spi->transfer_finished = 0;
	spi->transfer_length = 0;
	spi->transfer_pending = 0;
	spi->transfer_request = 0;
	spi->transfer_pin = 0;
	spi->write_complete = 0;
	spi->write_in_progress = 0;
	spi->write_length = 0;
	spi->write_remaining = 0;
	spi->write_requested = 0;
	spi->spi_data[0] = '\0';
	spi->reset_detect_count = 0;
	spi->timeout_detect_count = 0;

	spi->initialized = 1;
	spi->state = spi_reset;
}

uint8_t handle_spi(void){
	spi_check_reset(&spi);
	switch(spi.state){
		case spi_reset : {
			handle_spi_reset(&spi);
			break;
		}
		case spi_initialized : {
			send_connect(&spi);
			break;
		}
		case spi_running : {
			get_new_transfer(&spi);
			handle_transfer(&spi);
			break;
		}
		default : {
			spi.state = spi_reset;
		}
	}
	return spi.connected;
}

void handle_spi_reset(struct spi_struct* spi){
	spi->connected = 0;
	spi->reset_detect_count = 0;
	if(spi->initialized){
		if(spi->transfer_pending){
			if(spi->transfer_finished){
				spi_clear_pending_transfer(spi);
				spi_clear_finished_transfer(spi);
				#ifdef SPI_MASTER
				//printf("Finished Data transfer\n");
				if(!strcmp(spi->spi_data, SPI_RESET_STRING)){
					printf("Received Reset String %s, so CNC Spi is reset\n", spi->spi_data);
					spi->state = spi_initialized;
					spi_clear_master_request(spi);
				} else {
					//printf("CNC Spi isn't reset... Received: %s\n", spi->spi_data);
				}
				#endif
				#ifdef SPI_SLAVE
				if(spi_get_master_request_pin() == 0){
					spi->state = spi_initialized;
				}
				#endif
				spi->transfer_request = 0;
				spi->timeout_detect_count = 0;
			} else {
				#ifdef SPI_SLAVE
				// wait for master to send data
				spi->timeout_detect_count++;
				if(spi->timeout_detect_count >= SPI_TIMEOUT_COUNT_VALUE){
					// timeout detected so toggle the request again to trigger slave event
					spi->transfer_request = 0;
					spi_set_slave_request(spi);
					spi->timeout_detect_count = 0;
				}
				#endif
			}
		} else {
			#ifdef SPI_MASTER
			spi_set_master_request(spi);
			spi_check_slave_request(spi);
			if(spi->transfer_request){
				strcpy(spi->spi_data, SPI_IDLE_STRING);
				spi_transfer_data(spi->spi_data, strlen(SPI_IDLE_STRING));
				spi_set_pending_transfer(spi);
			}
			#endif
			#ifdef SPI_SLAVE
			if(spi_get_master_request_pin()){
				strcpy(spi->spi_data, SPI_RESET_STRING);
				spi_transfer_data(spi->spi_data, strlen(SPI_RESET_STRING));
				spi_set_pending_transfer(spi);
				spi_set_slave_request(spi);
			}
			#endif
		}
	} else {
		// Initialize a SPI driver instance
		init_spi_struct(spi);
		#ifdef SPI_MASTER
		spi_clear_master_request(spi);
		#endif
		#ifdef SPI_SLAVE
		spi_clear_slave_request(spi);
		#endif
		spi->initialized = 1;
		spi->state = spi_initialized;
	}
}

void send_connect(struct spi_struct* spi){
	if(spi->transfer_pending){
		if(spi->transfer_finished){
			spi_clear_pending_transfer(spi);
			spi_clear_finished_transfer(spi);
			#ifdef SPI_MASTER
			if(!strcmp(spi->spi_data, SPI_SLAVE_INIT_STRING)){
				printf("Received %s, so CNC is connected\n", spi->spi_data);
				spi->state = spi_running;
				spi->connected = 1;
			} else {
				//printf("CNC Spi Wasn't Ready... Received: %s, length: %d\n", spi->spi_data, spi->transfer_length);
			}
			#endif
			#ifdef SPI_SLAVE
			// master sent data
			if(!strcmp(spi->spi_data, SPI_MASTER_INIT_STRING)){
				spi->state = spi_running;
				spi->connected = 1;
			}
			#endif
			spi->timeout_detect_count = 0;
			spi->transfer_request = 0;
		} else {
			#ifdef SPI_SLAVE
			// waiting for master to initialize data write
			spi->timeout_detect_count++;
			if(spi->timeout_detect_count >= SPI_TIMEOUT_COUNT_VALUE){
				// timeout detected so toggle the request again to trigger slave event
				spi->transfer_request = 0;
				spi_set_slave_request(spi);
				spi->timeout_detect_count = 0;
			}
			#endif
		}
	} else {
		#ifdef SPI_MASTER
		spi_set_master_request(spi);
		spi_check_slave_request(spi);
		if(spi->transfer_request){
			strcpy(spi->spi_data, SPI_MASTER_INIT_STRING);
			spi_transfer_data(spi->spi_data, strlen(SPI_MASTER_INIT_STRING));
			spi_set_pending_transfer(spi);
			spi_clear_master_request(spi);
		}
		//printf("Sending Connection\n");
		#endif
		#ifdef SPI_SLAVE
		//spi_check_master_request();
		if(spi_get_master_request_pin()){
			strcpy(spi->spi_data, SPI_SLAVE_INIT_STRING);
			spi_transfer_data(spi->spi_data, strlen(SPI_SLAVE_INIT_STRING));
			spi_set_slave_request(spi);
			spi_set_pending_transfer(spi);
		}
		#endif
	}
}

void get_new_transfer(struct spi_struct* spi){
	if(spi->read_requested){
		spi->read_in_progress = 1;
		spi->read_state = transfer_requested;
		spi->read_remaining = 0;
		spi->read_requested = 0;
		#ifdef SPI_MASTER
			//printf("got new read request\n");
		#endif
	}
	if(spi->write_requested){
		spi->write_in_progress = 1;
		spi->write_state = transfer_requested;
		spi->write_remaining = spi->write_length;
		spi->write_requested = 0;
		#ifdef SPI_MASTER
			//printf("got new write request\n");
		#endif
	}
}

void handle_transfer(struct spi_struct* spi){
	if(spi->read_in_progress || spi->write_in_progress){
		if(spi->transfer_pending){
			//printf("read %d, write %d, in progress, transfer pending\n", spi->read_in_progress, spi->write_in_progress);
			if(spi->transfer_finished){
				spi_parse_transfer(spi);
				spi->timeout_detect_count = 0;
				//printf("transfer done\n");
			} else {
				// wait for transfer to finish
				//printf("transfer not done\n");
				#ifdef SPI_SLAVE
					spi->timeout_detect_count++;
					if(spi->timeout_detect_count >= SPI_TIMEOUT_COUNT_VALUE){
						// timeout detected so toggle the request again to trigger slave event
						spi->transfer_request = 0;
						spi_set_slave_request(spi);
						spi->timeout_detect_count = 0;
					}
				#endif
			}
		} else {
			// check read and write for max transfer size needed
			//printf("read %d, write %d, in progress, transfer not pending\n", spi->read_in_progress, spi->write_in_progress);
			spi_prepare_transfer(spi);
		}
	} else {
		#ifdef SPI_MASTER
		// don't do anything on master when idle...
		//printf("not reading or writing\n");
		#endif
		#ifdef SPI_SLAVE
		// actually... don't do this... let master read nothing which means not ready with idle
		// set idle transfer on slave when idle...
		// that way we can process anything the master does

		#endif
	}
}

void spi_prepare_transfer(struct spi_struct* spi){
	uint16_t write_size = 0;
	uint16_t read_size = 0;
	uint16_t total_write_length = 0;
	uint8_t send_transfer = 0;
	enum spi_opcodes current_write_opcode = write_idle;
	enum spi_opcodes current_read_opcode = read_idle;

	if(spi->write_in_progress && !spi->write_complete){
		send_transfer = 1;
		if(spi->write_state == transfer_requested){
			// set initial opcode
			#ifdef SPI_MASTER
				//printf("setting initial write\n");
			#endif
			total_write_length = spi->write_length;
			write_size = 0; // don't write on first packet
			if(total_write_length > SPI_DATA_LENGTH){
				current_write_opcode = start_multi_write;
			} else {
				current_write_opcode = start_single_write;
			}
		} else {
			// set transfer opcodes
			#ifdef SPI_MASTER
				//printf("transferring write data\n");
			#endif
			if(spi->write_remaining > SPI_DATA_LENGTH){
				current_write_opcode = continue_write;
			} else {
				current_write_opcode = continue_write;
			}
			total_write_length = spi->write_length;
			write_size = spi->write_remaining;
		}
		// this will get set whenever an initial write has been received
		//spi->write_state = transfer_in_progress;
	} else {
		current_write_opcode = write_idle;
	}
	if(spi->read_in_progress && !spi->read_complete){
		#ifdef SPI_MASTER
		spi_check_slave_request(spi);
		if(spi->transfer_request){
			send_transfer = 1;
		}
		#endif
		#ifdef SPI_SLAVE
			if(spi_get_master_request_pin()){
				send_transfer = 1;
			}
		#endif
		if(send_transfer){
			if(spi->read_state == transfer_requested){
				// set initial opcode and length to get actual read length
				current_read_opcode = read_ready;
			} else {
				read_size = spi->read_remaining;
				current_read_opcode = read_ready;
			}
		}
		// this will get set whenever an initial read has been received
		//spi->read_state = transfer_in_progress;
	} else {
		current_read_opcode = read_idle;
	}
	if(write_size > SPI_DATA_LENGTH){
		write_size = SPI_DATA_LENGTH;
	}
	if(read_size > SPI_DATA_LENGTH){
		read_size = SPI_DATA_LENGTH;
	}
	if(write_size > read_size){
		spi->transfer_length = write_size;
	} else {
		spi->transfer_length = read_size;
	}
	#ifdef SPI_MASTER
	if(send_transfer){
		spi_check_slave_request(spi);
		if(spi->transfer_request){
			// clear master request since slave responded
			spi_clear_master_request(spi);
			// allow transfer to be processed
			send_transfer = 1;
		} else {
			// set master request
			spi_set_master_request(spi);
			// wait for slave transfer request
			send_transfer = 0;
		}
	}
	#endif
	if(send_transfer){
		#ifdef SPI_MASTER
			//printf("sending transfer lengths: %d, %d\n", spi->transfer_length, spi->transfer_length+SPI_HEADER_LENGTH);
		#endif
		spi->transfer_length = spi->transfer_length + SPI_HEADER_LENGTH;
		// fix this line to a vsprintf or something...
		// write out all the fields...
		spi->spi_data[0] = current_write_opcode;
		spi->spi_data[1] = current_read_opcode;
		spi->spi_data[2] = write_size & 0xFF;
		spi->spi_data[3] = (write_size >> 8) & 0xFF;
		spi->spi_data[4] = total_write_length & 0xFF;
		spi->spi_data[5] = (total_write_length >> 8) & 0xFF;
		#ifdef SPI_MASTER
			//printf("s: Cur W %d, Cur R %d, Wsize %d, TotalS: %d\n", current_write_opcode, current_read_opcode, write_size, total_write_length);
		#endif
		if((write_size > 0) && (total_write_length > 0)){
			memcpy(&spi->spi_data[6], &spi->write_data[(total_write_length)-(spi->write_remaining)], write_size);
		}
		if(spi_transfer_data(spi->spi_data, spi->transfer_length)){
			// if didn't get zero, than couldn't transfer, so retry
		} else {
			#ifdef SPI_SLAVE
			spi_set_slave_request(spi);
			#endif
			spi_set_pending_transfer(spi);
		}
		
	}
}

void spi_parse_transfer(struct spi_struct* spi){
	uint16_t write_size = 0;
	uint16_t current_write_size = 0;
	uint16_t read_size = 0;
	enum spi_opcodes current_write_opcode = write_idle;
	enum spi_opcodes current_read_opcode = write_idle;

	if(spi->transfer_length < SPI_HEADER_LENGTH){
		// check to see if this is reset request?

		// didn't receive valid transfer
		#ifdef SPI_MASTER
		//printf("received bad transfer\n");
		#endif
	} else {
		// parse data into the different fields
		current_write_opcode = spi_parse_opcode(&spi->spi_data[0]);
		current_read_opcode = spi_parse_opcode(&spi->spi_data[OPCODE_LENGTH]);
		current_write_size = spi_string_to_int32(&spi->spi_data[2*OPCODE_LENGTH], SPI_SIZE_LENGTH);
		read_size = spi_string_to_int32(&spi->spi_data[2*OPCODE_LENGTH+SPI_SIZE_LENGTH], SPI_SIZE_LENGTH);
		#ifdef SPI_MASTER
		//printf("Valid Transfer\n");
		#endif
		#ifdef SPI_MASTER
		//printf("Rec CurW: %d, CurR: %d, Wsize: %d, Total: %d\n", current_write_opcode, current_read_opcode, write_size, read_size);
		#endif
	}

	if(spi->write_in_progress && !spi->write_complete){
		if(spi->write_state == transfer_requested){
			// processing initial transfer
			if(current_read_opcode == read_ready){
				// initial write received... ready to write
				spi->write_state = transfer_in_progress;
				#ifdef SPI_MASTER
				//printf("Initial Read Ready... Ready to write\n");
				#endif
			} else {
				// data wasn't received... resend initial write
			}
		} else {
			if(current_read_opcode == read_ready){
				// if recevied ready, than data was transferred
				// update write remaining with write length
				write_size = spi->transfer_length - SPI_HEADER_LENGTH;
				if(write_size >= spi->write_remaining){
					spi->write_remaining = 0;
					spi->write_state = transfer_complete;
					spi->write_complete = 1;
					#ifdef SPI_MASTER
						//printf("Wrote all the data, set write complete\n");
					#endif
				} else {
					spi->write_remaining = spi->write_remaining - write_size;
				}
			} else {
				// data wasn't recevied... resend data
			}
		}
	}
	if(spi->read_in_progress && !spi->read_complete){
		if(spi->read_state == transfer_requested){
			// processing initial transfer
			if((current_write_opcode == start_single_write) || (current_write_opcode == start_multi_write)){
				// initial read received... ready to read data
				// copy write length to read length and update read remaining
				#ifdef SPI_MASTER
					//printf("initial write received read size copied\n");
				#endif
				spi->read_length = read_size;
				spi->read_remaining = read_size;
				spi->read_state = transfer_in_progress;
			} else {
				// data wasn't received... resend initial read
			}
		} else {
			if(current_write_opcode == continue_write){
				// write opcode contained data
				// copy data to read buffer and update read remaining
				#ifdef SPI_MASTER
					//printf("read data received\n");
				#endif
				memcpy(&spi->read_data[spi->read_length - spi->read_remaining], &spi->spi_data[SPI_HEADER_LENGTH], current_write_size);
				if(current_write_size >= spi->read_remaining){
					spi->read_remaining = 0;
					spi->read_state = transfer_complete;
					#ifdef SPI_MASTER
						//printf("read finished - set complete\n");
					#endif
					spi->read_complete = 1;
				} else {
					spi->read_remaining = spi->read_remaining - current_write_size;
				}
			} else {
				// data wasn't received... resend data
				#ifdef SPI_MASTER
					//printf("missed data...\n");
				#endif
			}
		}
	}
	for(uint16_t clear_count=0;clear_count<spi->transfer_length;clear_count++){
		spi->spi_data[clear_count] = '\0';
	}
	spi->transfer_length = 0;
	spi->transfer_request = 0;
	spi_clear_pending_transfer(spi);
	spi_clear_finished_transfer(spi);
}
/*
void set_idle_read(struct spi_struct* spi){
	//enum spi_opcodes opcode;
	char * length_string = 0;

	if(spi->transfer_pending){
		if(spi->transfer_finished){
			// master sent data
			spi->transfer_finished = 0;
			//opcode = spi_parse_opcode(spi->spi_data);
			length_string = &spi->spi_data[OPCODE_LENGTH];
			spi->transfer_length = spi_string_to_int32(length_string, (IDLE_LENGTH-OPCODE_LENGTH));
			//handle_opcode(spi, opcode, cnc);
		} else {

		}
	} else {
		//SPIDRV_STransfer( handle, spi->spi_data, spi->spi_data, IDLE_LENGTH, slave_transfer_done, 0);
		//printf("Set idle print\n");
		if(cnc->request_print){
			spi->state = send_cnc_print;
		} else {
			if(cnc->request_instruction) {
				spi->state = get_cnc_instruction;
			}
		}
	}
}*/

enum spi_opcodes spi_parse_opcode(char* opcode_string){
	enum spi_opcodes new_opcode;
	new_opcode = (enum spi_opcodes) spi_string_to_int32(opcode_string, OPCODE_LENGTH);
	return new_opcode;
}

int32_t spi_string_to_int32(char* length_string, uint8_t byte_length){
	int32_t temp = 0;
	uint8_t length_count = 0;
	
	// parse string into int
	for(length_count=0;length_count<byte_length;length_count++){
		temp = (temp << 8) | length_string[byte_length - 1 - length_count];
	}
	
	// check if negative and pad
	if(length_string[byte_length - 1 - length_count] >> 7){
		// top bit was set... so fill negative
		temp = (-1 << (byte_length*8)) | temp;
	}
	
	return temp;
}
