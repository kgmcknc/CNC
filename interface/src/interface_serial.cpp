#include "main.h"
#include "interface_serial.h"

int serial_fd;

uint8_t serial_send_data(uint32_t send_size, uint8_t* send_data){
   write(serial_fd, send_data, send_size);
   return 1;
}

uint32_t serial_get_data(uint32_t receive_size, uint8_t* receive_data){
   uint32_t i=0;
   uint32_t avail;
   uint8_t* data_pointer = receive_data;
   if((uint32_t) serialDataAvail(serial_fd) >= receive_size){
      while(i<receive_size){
         *data_pointer = serialGetchar(serial_fd);
         data_pointer++;
         i++;
      }
   }
   return i;
}

void serial_flush(void){
   serialFlush(serial_fd);
}
