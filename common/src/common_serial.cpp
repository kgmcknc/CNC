
#include "stdint.h"
#include "common_serial.h"
#include "string.h"
#include "stdio.h"

serial_class::serial_class(uint8_t config_as_master){
   state = INIT;
   is_master = (config_as_master == 0) ? 0 : 1;
}

uint8_t serial_class::send(uint16_t send_size, uint8_t* send_data){
   if(tx_pending){
      // can't start another transfer
      return 0;
   } else {
      transfer_size = send_size;
      memcpy(tx_data, send_data, send_size);
      tx_pending = 1;
      return 1;
   }
}

uint16_t serial_class::receive(uint8_t* receive_data){
   uint16_t rx_size;
   if(rx_queue_fullness > 0){
      rx_size = rx_size_queue[rx_queue_read_pointer];
      memcpy(receive_data, rx_data_queue[rx_queue_read_pointer], rx_size);
      rx_queue_read_pointer = (rx_queue_read_pointer < (RX_QUEUE_DEPTH-1)) ? (rx_queue_read_pointer + 1): 0;
      rx_queue_fullness = rx_queue_fullness - 1;
      return rx_size;
   } else {
      return 0;
   }
}

//#define debug_prints
//#define USE_TIMEOUT
void serial_class::process(void){
   uint8_t temp_data;
   #ifdef USE_TIMEOUT
   if(timeout_count > MAX_WAIT_TIME){
      timeout_count = 0;
      state = INIT;
   }
   #endif
   switch(state){
      case INIT : {
         init();
         state = (is_master) ? MASTER_SEND_START : SLAVE_WAIT_START;
         #ifdef debug_prints
            printf("Initlializing\n");
         #endif
         break;
      }
      case MASTER_SEND_START : {
         // flush serial receive queue at start
         while(serial_get_data(1, &temp_data));
         temp_data = (uint8_t) MASTER_START;
         if(serial_send_data(1, &temp_data)){
            #ifdef debug_prints
               printf("Master Sending Start\n");
            #endif
            state = MASTER_WAIT_CONNECT;
         }
         timeout_count = 0;
         break;
      }
      case MASTER_WAIT_CONNECT : {
         if(serial_get_data(1, &temp_data)){
            if((serial_opcodes) temp_data == SLAVE_CONNECT){
               state = MASTER_SEND_CONNECT;
            } else {
               state = MASTER_SEND_START;
            }
            timeout_count = 0;
            #ifdef debug_prints
               printf("Master Received Slave Connect\n");
            #endif
         } else {
            if(timeout_count >= INIT_WAIT_TIME){
               timeout_count = 0;
               state = MASTER_SEND_START;
            } else {
               timeout_count = timeout_count + 1;
            }
         }
         break;
      }
      case MASTER_SEND_CONNECT : {
         temp_data = (uint8_t) MASTER_CONNECT;
         if(serial_send_data(1, &temp_data)){
            #ifdef debug_prints
               printf("Master Sending Connect\n");
            #endif
            is_connected = 1;
            state = IDLE;
         }
         timeout_count = 0;
         break;
      }
      case SLAVE_WAIT_START : {
         if(serial_get_data(1, &temp_data)){
            if((serial_opcodes) temp_data == MASTER_START){
               state = SLAVE_SEND_CONNECT;
            } else {
               state = SLAVE_WAIT_START;
            }
            #ifdef debug_prints
               printf("Slave Received Start\n");
            #endif
         }
         timeout_count = 0;
         break;
      }
      case SLAVE_SEND_CONNECT : {
         temp_data = (uint8_t) SLAVE_CONNECT;
         if(serial_send_data(1, &temp_data)){
            state = SLAVE_WAIT_CONNECT;
            #ifdef debug_prints
               printf("Slave Sending Connect\n");
            #endif
         }
         timeout_count = 0;
         break;
      }
      case SLAVE_WAIT_CONNECT : {
         if(serial_get_data(1, &temp_data)){
            if((serial_opcodes) temp_data == MASTER_CONNECT){
               is_connected = 1;
               state = IDLE;
            } else {
               state = SLAVE_WAIT_START;
            }
            #ifdef debug_prints
               printf("Slave Received Connect\n");
            #endif
         }
         timeout_count = 0;
         break;
      }
      case IDLE : {
         timeout_count = 0;
         // see if there is data ready to be received
         if(serial_get_data(1, &temp_data)){
            switch((serial_opcodes) temp_data){
               case TX_REQ : {
                  if(is_master){
                     // check to see if there is data ready to transmit
                     if(tx_pending){
                        // master transmit takes priority over slave transmit
                        state = SEND_TRANSFER_INIT;
                     } else {
                        if(rx_queue_fullness >= RX_QUEUE_DEPTH){
                           state = SEND_QUEUE_FULL;
                        } else {
                           state = SEND_TRANSFER_ACCEPT;
                        }
                     }
                  } else {
                     // if slave, always have to process master request first
                     if(rx_queue_fullness >= RX_QUEUE_DEPTH){
                        state = SEND_QUEUE_FULL;
                     } else {
                        state = SEND_TRANSFER_ACCEPT;
                     }
                  }
                  break;
               }
               case MASTER_START : {
                  state = INIT;
                  break;
               }
               default : {
                  state = IDLE;
               }
            }
         } else {
            // if no data received, process your requests
            if(tx_pending){
               state = SEND_TRANSFER_INIT;
            } else {
               state = IDLE;
            }
         }
         break;
      }
      case SEND_TRANSFER_INIT : {
         temp_data = (uint8_t) TX_REQ;
         if(serial_send_data(1, &temp_data)){
            state = WAIT_TRANSFER_RESPONSE;
            #ifdef debug_prints
               printf("Sending Transfer Init\n");
            #endif
         }
         timeout_count = 0;
         break;
      }
      case SEND_QUEUE_FULL : {
         temp_data = (uint8_t) RX_FULL;
         if(serial_send_data(1, &temp_data)){
            state = IDLE;
            #ifdef debug_prints
               printf("Sending Queue Full\n");
            #endif
         }
         timeout_count = 0;
         break;
      }
      case SEND_TRANSFER_ACCEPT : {
         temp_data = (uint8_t) TX_ACCEPT;
         if(serial_send_data(1, &temp_data)){
            state = RECEIVE_TRANSFER_SIZE;
            #ifdef debug_prints
               printf("Sending Transfer Accept\n");
            #endif
         }
         timeout_count = 0;
         break;
      }
      case WAIT_TRANSFER_RESPONSE : {
         if(serial_get_data(1, &temp_data)){
            switch((serial_opcodes) temp_data){
               case TX_REQ : {
                  if(is_master){
                     // slave issued request at same time as us... ignore this...
                     // slave will respond to our tx req
                  } else {
                     // master sent a request when we did... ignore ours
                     // send response for his
                     if(rx_queue_fullness >= RX_QUEUE_DEPTH){
                        state = SEND_QUEUE_FULL;
                     } else {
                        state = SEND_TRANSFER_ACCEPT;
                     }  
                  }
                  break;
               }
               case TX_ACCEPT : {
                  state = SEND_TRANSFER_SIZE;
                  break;
               }
               case RX_FULL : {
                  state = IDLE;
                  break;
               }
               default : {
                  state = IDLE;
                  break;
               }
            }
         } else {
            // do nothing... wait for response
         }
         break;
      }
      case SEND_TRANSFER_SIZE : {
         uint8_t tx_size[2];
         tx_size[0] = transfer_size >> 0;
         tx_size[1] = transfer_size >> 8;
         tx_checksum = 0;
         if(serial_send_data(2, tx_size)){
            state = SEND_TRANSFER_DATA;
            #ifdef debug_prints
               printf("Sending Transfer Size: %d\n", transfer_size);
            #endif
         }
         bytes_transferred = 0;
         break;
      }
      /*case WAIT_SIZE_RESPONSE : {
         if(serial_get_data(1, &temp_data)){
            if(temp_data == SIZE_ACK){
               state = SEND_TRANSFER_DATA;
            } else {
               if(temp_data == SIZE_NACK){
                  serial_flush();
                  state = SEND_TRANSFER_SIZE;
               } else {
                  state = INIT;
               }
            }
         }
         break;
      }*/
      case SEND_TRANSFER_DATA : {
         uint16_t send_size = (transfer_size - bytes_transferred);
         if(send_size >= TRANSFER_BYTE_SIZE){
            if(serial_send_data((uint16_t) TRANSFER_BYTE_SIZE, &tx_data[bytes_transferred])){
               bytes_transferred = bytes_transferred + TRANSFER_BYTE_SIZE;
               #ifdef debug_prints
                  printf("Sent %d transfer bytes\n", ((uint16_t) TRANSFER_BYTE_SIZE));
               #endif
               state = WAIT_TRANSFER_ACK;
               #ifdef debug_prints
                  printf("Sent Transfer Data\n");
               #endif
            }
         } else {
            if(serial_send_data(send_size, &tx_data[bytes_transferred])){
               #ifdef debug_prints
                  printf("Sent %d transfer bytes\n", send_size);
               #endif
               bytes_transferred = transfer_size;
               state = WAIT_TRANSFER_ACK;
               #ifdef debug_prints
                  printf("Sent Transfer Data\n");
               #endif
            }
         }
         timeout_count = 0;
         break;
      }
      case WAIT_TRANSFER_ACK : {
         if(serial_get_data(1, &temp_data)){
            if(temp_data == TX_ACK){
               if(bytes_transferred < transfer_size){
                  state = SEND_TRANSFER_DATA;
                  #ifdef debug_prints
                     printf("Received Transfer Ack, Send More\n");
                  #endif
               } else {
                  state = SEND_CHECKSUM;
                  #ifdef debug_prints
                     printf("Received Transfer Ack, Done Transfer\n");
                  #endif
               }
               timeout_count = 0;
            } else {
               state = ABORT_TRANSFER;
               // error? resend last bytes?
            }
         }
         break;
      }
      case SEND_CHECKSUM : {
         state = SEND_COMPLETE;
         #ifdef debug_prints
            printf("Send Complete\n");
         #endif
         timeout_count = 0;
         break;
      }
      /*case RECEIVE_SIZE_CODE : {
         uint8_t size_code;
         if(serial_get_data(1, &size_code)){
            if(size_code == (uint8_t) TX_SIZE){
               state = RECEIVE_TRANSFER_SIZE;
            } else {
               state = RECEIVE_SIZE_CODE;
            }
         }
         break;
      }*/
      case RECEIVE_TRANSFER_SIZE : {
         uint8_t rx_size[2];
         transfer_size = 0;
         if(serial_get_data(2, rx_size)){
            transfer_size = transfer_size | (uint16_t) rx_size[0];
            transfer_size = transfer_size | ((uint16_t) rx_size[1] << 8);
            rx_checksum = 0;
            bytes_transferred = 0;
            timeout_count = 0;
            state = RECEIVE_TRANSFER_DATA;
            #ifdef debug_prints
               printf("Received Transfer Size: %d\n", transfer_size);
            #endif
         }
         break;
      }
      /*case SEND_SIZE_ACK : {
         temp_data = (uint8_t) SIZE_ACK;
         serial_send_data(1, &temp_data);
         state = RECEIVE_TRANSFER_DATA;
         timeout_count = 0;
         break;
      }
      case SEND_SIZE_NACK : {
         temp_data = (uint8_t) SIZE_NACK;`
         serial_flush();
         serial_send_data(1, &temp_data);
         state = RECEIVE_SIZE_CODE;
         break;
      }*/
      case RECEIVE_TRANSFER_DATA : {
         uint16_t receive_size = (transfer_size - bytes_transferred);
         if(receive_size >= (uint16_t) TRANSFER_BYTE_SIZE){
            if(serial_get_data((uint16_t) TRANSFER_BYTE_SIZE, &rx_data_queue[rx_queue_write_pointer][bytes_transferred])){
               bytes_transferred = bytes_transferred + TRANSFER_BYTE_SIZE;
               state = SEND_TRANSFER_ACK;
               #ifdef debug_prints
                  printf("Received %d Transfer Data, Send Ack\n", ((uint16_t) TRANSFER_BYTE_SIZE));
               #endif
               timeout_count = 0;
            }
         } else {
            if(serial_get_data(receive_size, &rx_data_queue[rx_queue_write_pointer][bytes_transferred])){
               bytes_transferred = transfer_size;
               state = SEND_TRANSFER_ACK;
               timeout_count = 0;
               #ifdef debug_prints
                  printf("Received %d, Transfer Data, Send Ack\n", receive_size);
               #endif
            }
         }
         break;
      }
      case SEND_TRANSFER_ACK : {
         temp_data = (uint8_t) TX_ACK;
         if(serial_send_data(1, &temp_data)){
            if(bytes_transferred >= transfer_size){
               state = RECEIVE_CHECKSUM;
            } else {
               state = RECEIVE_TRANSFER_DATA;
            }
            #ifdef debug_prints
               printf("Sent Transfer Ack\n");
            #endif
         }
         timeout_count = 0;
         break;
      }
      case RECEIVE_CHECKSUM : {
         timeout_count = 0;
         #ifdef debug_prints
            printf("Received Checksum\n");
         #endif
         state = WAIT_COMPLETE;
         break;
      }
      case SEND_COMPLETE : {
         tx_pending = 0;
         timeout_count = 0;
         #ifdef debug_prints
            printf("Send Done, Going Back To Idle\n");
         #endif
         state = IDLE;
         break;
      }
      case WAIT_COMPLETE : {
         rx_size_queue[rx_queue_write_pointer] = transfer_size;
         rx_queue_fullness = rx_queue_fullness + 1;
         rx_queue_write_pointer = rx_queue_write_pointer + 1;
         if(rx_queue_write_pointer >= RX_QUEUE_DEPTH){
            rx_queue_write_pointer = 0;
         }
         #ifdef debug_prints
            printf("Send Wait Done, Going Back To Idle\n");
         #endif
         timeout_count = 0;
         state = IDLE;
         break;
      }
      case ABORT_TRANSFER : {
         #ifdef debug_prints
            printf("Aborting Transfer\n");
         #endif
         timeout_count = 0;
         state = IDLE;
         break;
      }
      default : {
         state = INIT;
         break;
      }
   }
}

void serial_class::init(void){
   tx_pending = 0;
   tx_active = 0;
   rx_pending = 0;
   rx_active = 0;
   for(int i=0;i<MAX_TRANSFER_SIZE;i++){
      tx_data[i] = 0;
      rx_data[i] = 0;
   }
   rx_queue_fullness = 0;
   rx_queue_write_pointer = 0;
   rx_queue_read_pointer = 0;
   for(int j=0;j<RX_QUEUE_DEPTH;j++){
      for(int i=0;i<MAX_TRANSFER_SIZE;i++){
         rx_data_queue[j][i] = 0;
      }
   }
   transfer_size = 0;
   bytes_transferred = 0;
   tx_checksum = 0;
   rx_checksum = 0;
   timeout_count = 0;
   invalid_response_count = 0;
   is_connected = 0;
   serial_flush();
   state = INIT;
}