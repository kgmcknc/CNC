
#ifndef SRC_COMMON_SERIAL_H
#define SRC_COMMON_SERIAL_H

   #include <stdint.h>

   #define RX_QUEUE_DEPTH 2
   #define MAX_TRANSFER_SIZE 512
   #define TRANSFER_BYTE_SIZE 16
   #define INIT_WAIT_TIME 80000
   #define MAX_WAIT_TIME 20000

   enum serial_opcodes {
      RESET, MASTER_START, MASTER_CONNECT, SLAVE_START, SLAVE_CONNECT,
      TX_REQ, TX_ACCEPT, TX_SIZE, RX_FULL, SIZE_ACK, SIZE_NACK, TX_ACK, CHECKSUM_PASSED, CHECKSUM_FAILED
   };

   enum serial_state {
      INIT,
      MASTER_SEND_START, MASTER_WAIT_CONNECT, MASTER_SEND_CONNECT,
      SLAVE_WAIT_START, SLAVE_SEND_CONNECT, SLAVE_WAIT_CONNECT,
      IDLE, SEND_TRANSFER_INIT, SEND_QUEUE_FULL, SEND_TRANSFER_ACCEPT, WAIT_TRANSFER_RESPONSE,
      SEND_TRANSFER_SIZE, WAIT_SIZE_RESPONSE, SEND_TRANSFER_DATA, WAIT_TRANSFER_ACK, SEND_CHECKSUM,
      RECEIVE_SIZE_CODE, RECEIVE_TRANSFER_SIZE, SEND_SIZE_ACK, SEND_SIZE_NACK, RECEIVE_TRANSFER_DATA, SEND_TRANSFER_ACK, RECEIVE_CHECKSUM,
      ABORT_TRANSFER, SEND_COMPLETE, WAIT_COMPLETE
   };

   class serial_class {
      public:
         serial_class(uint8_t config_as_master);
         uint8_t is_master;
         uint8_t is_connected;
         serial_state state;
         uint32_t timeout_count;
         uint8_t invalid_response_count;
         uint8_t tx_pending;
         uint8_t tx_active;
         uint8_t rx_pending;
         uint8_t rx_active;
         uint8_t tx_data[MAX_TRANSFER_SIZE];
         uint8_t rx_data[MAX_TRANSFER_SIZE];
         uint8_t rx_data_queue[RX_QUEUE_DEPTH][MAX_TRANSFER_SIZE];
         uint32_t rx_size_queue[RX_QUEUE_DEPTH];
         uint32_t rx_queue_fullness;
         uint32_t rx_queue_write_pointer;
         uint32_t rx_queue_read_pointer;
         uint32_t transfer_size;
         uint32_t bytes_transferred;
         uint8_t tx_checksum;
         uint8_t rx_checksum;
         void init(void);
         void process(void);
         uint8_t send(uint32_t send_size, uint8_t* send_data);
         uint32_t receive(uint8_t* receive_data);
   };

   uint8_t serial_send_data(uint32_t send_size, uint8_t* send_data); // user will define this function to send data with whatever serial interface they use
   uint32_t serial_get_data(uint32_t receive_size, uint8_t* receive_data); // user will define this function to copy received serial data to the class
   void serial_flush(void); // user will define this function to copy received serial data to the class

#endif // SRC_COMMON_SERIAL_H