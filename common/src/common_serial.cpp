
#include "stdint.h"
#include "common_serial.h"
#include "string.h"
#include "stdio.h"

serial_class::serial_class(uint8_t config_as_master){
   state = INIT;
   is_master = (config_as_master == 0) ? 0 : 1;
}

uint8_t serial_class::send(uint32_t send_size, uint8_t* send_data){
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

uint32_t serial_class::receive(uint8_t* receive_data){
   uint32_t rx_size;
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

void serial_class::process(void){
   uint8_t temp_data;
   timeout_count = timeout_count + 1;
   switch(state){
      case INIT : {
         init();
         state = (is_master) ? MASTER_SEND_START : SLAVE_WAIT_START;
         break;
      }
      case MASTER_SEND_START : {
         temp_data = (uint8_t) MASTER_START;
         serial_send_data(1, &temp_data);
         timeout_count = 0;
         state = MASTER_WAIT_START;
         break;
      }
      case MASTER_WAIT_START : {
         if(serial_get_data(1, &temp_data)){
            if((serial_opcodes) temp_data == SLAVE_START){
               state = MASTER_SEND_CONNECT;
            } else {
               state = MASTER_SEND_START;
            }
            timeout_count = 0;
         } else {
            if(timeout_count > MAX_WAIT_TIME){
               timeout_count = 0;
               state = MASTER_SEND_START;
            }
         }
         break;
      }
      case MASTER_SEND_CONNECT : {
         state = MASTER_WAIT_CONNECT;
         temp_data = (uint8_t) MASTER_CONNECT;
         serial_send_data(1, &temp_data);
         timeout_count = 0;
         break;
      }
      case MASTER_WAIT_CONNECT : {
         if(serial_get_data(1, &temp_data)){
            if((serial_opcodes) temp_data == SLAVE_CONNECT){
               is_connected = 1;
               state = IDLE;
            } else {
               if((serial_opcodes) temp_data == SLAVE_START){
                  state = MASTER_SEND_CONNECT;
               } else {
                  state = MASTER_SEND_START;
               }
            }
            timeout_count = 0;
         } else {
            if(timeout_count > MAX_WAIT_TIME){
               timeout_count = 0;
               state = MASTER_SEND_CONNECT;
            }
         }
         break;
      }
      case SLAVE_WAIT_START : {
         if(serial_get_data(1, &temp_data)){
            if((serial_opcodes) temp_data == MASTER_START){
               state = SLAVE_SEND_START;
            } else {
               if((serial_opcodes) temp_data == MASTER_CONNECT){
                  state = SLAVE_SEND_CONNECT;
               } else {
                  state = SLAVE_WAIT_START;
               }
            }
         }
         timeout_count = 0;
         break;
      }
      case SLAVE_SEND_START : {
         temp_data = (uint8_t) SLAVE_START;
         serial_send_data(1, &temp_data);
         state = SLAVE_WAIT_CONNECT;
         timeout_count = 0;
         break;
      }
      case SLAVE_WAIT_CONNECT : {
         if(serial_get_data(1, &temp_data)){
            if((serial_opcodes) temp_data == MASTER_CONNECT){
               state = SLAVE_SEND_CONNECT;
            } else {
               if((serial_opcodes) temp_data == MASTER_START){
                  state = SLAVE_SEND_START;
               } else {
                  state = SLAVE_WAIT_START;
               }
            }
            timeout_count = 0;
         } else {
            if(timeout_count > MAX_WAIT_TIME){
               timeout_count = 0;
               state = SLAVE_WAIT_START;
            }
         }
         break;
      }
      case SLAVE_SEND_CONNECT : {
         temp_data = (uint8_t) SLAVE_CONNECT;
         serial_send_data(1, &temp_data);
         is_connected = 1;
         timeout_count = 0;
         state = IDLE;
         break;
      }
      case IDLE : {
         timeout_count = 0;
         if(is_master){
            if(tx_pending){ // check to see if there is data ready to transmit
               state = SEND_TRANSFER_INIT;
            } else {
               // see if there is data ready to be received
               if(serial_get_data(1, &temp_data)){
                  switch((serial_opcodes) temp_data){
                     case TX_REQ : {
                           if(rx_queue_fullness >= RX_QUEUE_DEPTH){
                              state = SEND_QUEUE_FULL;
                           } else {
                              state = SEND_TRANSFER_ACCEPT;
                           }
                        break;
                     }
                     case SLAVE_CONNECT : {
                        state = IDLE;
                        break;
                     }
                     default : {
                        state = INIT;
                     }
                  }
               } else {
                  state = IDLE;
               }
            }
         } else {
            if(serial_get_data(1, &temp_data)){
               switch((serial_opcodes) temp_data){
                  case TX_REQ : {
                        if(rx_queue_fullness >= RX_QUEUE_DEPTH){
                           state = SEND_QUEUE_FULL;
                        } else {
                           state = SEND_TRANSFER_ACCEPT;
                        }
                     break;
                  }
                  case MASTER_CONNECT : {
                     state = IDLE;
                     break;
                  }
                  default : {
                     state = INIT;
                  }
               }
            } else {
               if(tx_pending){
                  state = SEND_TRANSFER_INIT;
               } else {
                  state = IDLE;
               }
            }
         }
         break;
      }
      case SEND_TRANSFER_INIT : {
         temp_data = (uint8_t) TX_REQ;
         serial_send_data(1, &temp_data);
         timeout_count = 0;
         state = WAIT_TRANSFER_RESPONSE;
         break;
      }
      case SEND_QUEUE_FULL : {
         temp_data = (uint8_t) RX_FULL;
         serial_send_data(1, &temp_data);
         timeout_count = 0;
         state = IDLE;
         break;
      }
      case SEND_TRANSFER_ACCEPT : {
         temp_data = (uint8_t) TX_ACCEPT;
         serial_send_data(1, &temp_data);
         timeout_count = 0;
         state = RECEIVE_SIZE_CODE;
         break;
      }
      case WAIT_TRANSFER_RESPONSE : {
         if(is_master){
            if(serial_get_data(1, &temp_data)){
               switch((serial_opcodes) temp_data){
                  case TX_REQ : {
                     // master can ignore a request assuming slave will ignore it and respond to his
                     if(invalid_response_count < 1){
                        invalid_response_count = invalid_response_count + 1;
                        state = WAIT_TRANSFER_RESPONSE;
                     } else {
                        invalid_response_count = 0;
                        state = IDLE;
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
            }
         } else {
            if(serial_get_data(1, &temp_data)){
               switch((serial_opcodes) temp_data){
                  case TX_REQ : {
                     if(rx_queue_fullness >= RX_QUEUE_DEPTH){
                        state = SEND_QUEUE_FULL;
                     } else {
                        state = SEND_TRANSFER_ACCEPT;
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
            }
         }
         break;
      }
      case SEND_TRANSFER_SIZE : {
         uint8_t tx_size[6];
         tx_size[0] = (uint8_t) TX_SIZE;
         tx_size[1] = transfer_size >> 0;
         tx_size[2] = transfer_size >> 8;
         tx_size[3] = transfer_size >> 16;
         tx_size[4] = transfer_size >> 24;
         tx_size[5] = (uint8_t) TX_SIZE;
         tx_checksum = 0;
         serial_send_data(6, tx_size);
         bytes_transferred = 0;
         state = WAIT_SIZE_RESPONSE;
         break;
      }
      case WAIT_SIZE_RESPONSE : {
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
      }
      case SEND_TRANSFER_DATA : {
         if((transfer_size - bytes_transferred) >= TRANSFER_BYTE_SIZE){
            serial_send_data((uint32_t) TRANSFER_BYTE_SIZE, &tx_data[bytes_transferred]);
            bytes_transferred = bytes_transferred + TRANSFER_BYTE_SIZE;
         } else {
            serial_send_data((uint32_t) (transfer_size - bytes_transferred), &tx_data[bytes_transferred]);
            bytes_transferred = transfer_size;
         }
         timeout_count = 0;
         state = WAIT_TRANSFER_ACK;
         break;
      }
      case WAIT_TRANSFER_ACK : {
         if(serial_get_data(1, &temp_data)){
            if(temp_data == TX_ACK){
               if(bytes_transferred < transfer_size){
                  state = SEND_TRANSFER_DATA;
               } else {
                  state = SEND_CHECKSUM;
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
         timeout_count = 0;
         break;
      }
      case RECEIVE_SIZE_CODE : {
         uint8_t size_code;
         if(serial_get_data(1, &size_code)){
            if(size_code == (uint8_t) TX_SIZE){
               state = RECEIVE_TRANSFER_SIZE;
            } else {
               state = RECEIVE_SIZE_CODE;
            }
         }
         break;
      }
      case RECEIVE_TRANSFER_SIZE : {
         uint8_t rx_size[5];
         transfer_size = 0;
         if(serial_get_data(5, rx_size)){
            if(rx_size[4] == (uint8_t) TX_SIZE){
               transfer_size = transfer_size | (uint32_t) rx_size[0];
               transfer_size = transfer_size | ((uint32_t) rx_size[1] << 8);
               transfer_size = transfer_size | ((uint32_t) rx_size[2] << 16);
               transfer_size = transfer_size | ((uint32_t) rx_size[3] << 24);
               rx_checksum = 0;
               bytes_transferred = 0;
               timeout_count = 0;
               state = SEND_SIZE_ACK;
            } else {
               state = SEND_SIZE_NACK;
            }
         }
         break;
      }
      case SEND_SIZE_ACK : {
         temp_data = (uint8_t) SIZE_ACK;
         serial_send_data(1, &temp_data);
         state = RECEIVE_TRANSFER_DATA;
         timeout_count = 0;
         break;
      }
      case SEND_SIZE_NACK : {
         temp_data = (uint8_t) SIZE_NACK;
         serial_flush();
         serial_send_data(1, &temp_data);
         state = RECEIVE_SIZE_CODE;
         break;
      }
      case RECEIVE_TRANSFER_DATA : {
         if((transfer_size - bytes_transferred) >= (uint32_t) TRANSFER_BYTE_SIZE){
            if(serial_get_data((uint32_t) TRANSFER_BYTE_SIZE, &rx_data_queue[rx_queue_write_pointer][bytes_transferred])){
               bytes_transferred = bytes_transferred + TRANSFER_BYTE_SIZE;
               state = SEND_TRANSFER_ACK;
               timeout_count = 0;
            }
         } else {
            if(serial_get_data((uint32_t) (transfer_size - bytes_transferred), &rx_data_queue[rx_queue_write_pointer][bytes_transferred])){
               bytes_transferred = transfer_size;
               state = SEND_TRANSFER_ACK;
               timeout_count = 0;
            }
         }
         break;
      }
      case SEND_TRANSFER_ACK : {
         temp_data = (uint8_t) TX_ACK;
         serial_send_data(1, &temp_data);
         if(bytes_transferred >= transfer_size){
            state = RECEIVE_CHECKSUM;
         } else {
            state = RECEIVE_TRANSFER_DATA;
         }
         timeout_count = 0;
         break;
      }
      case RECEIVE_CHECKSUM : {
         timeout_count = 0;
         state = WAIT_COMPLETE;
         break;
      }
      case SEND_COMPLETE : {
         tx_pending = 0;
         timeout_count = 0;
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
         timeout_count = 0;
         state = IDLE;
         break;
      }
      case ABORT_TRANSFER : {
         timeout_count = 0;
         state = IDLE;
         break;
      }
      default : {
         state = INIT;
         break;
      }
   }
   if(timeout_count > MAX_WAIT_TIME){
      timeout_count = 0;
      state = INIT;
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