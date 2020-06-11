#include "main.h"
#include "interface_serial.h"

int serial_fd;

uint8_t serial_send_data(uint16_t send_size, uint8_t* send_data){
   int16_t write_size;
   write_size = write(serial_fd, send_data, send_size);
   if(write_size > 0){
      return 1;
   } else {
      return 0;
   }
}

uint16_t i=0;
uint8_t* data_pointer = 0;
int16_t ser_avail = 0;
uint16_t serial_get_data(uint16_t receive_size, uint8_t* receive_data){
   i=0;
   data_pointer = receive_data;
   ser_avail = serialDataAvail(serial_fd);
   
   //for(uint16_t count=0;count<receive_size;count++){
   //   receive_data[count] = 0;
   //}
   if(ser_avail >= receive_size){
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
