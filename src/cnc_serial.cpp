/*
 * cnc_spi.cpp
 *
 *  Created on: Aug 18, 2018
 *      Author: Kyle
 */

#include "cnc_serial.h"
#include "Arduino.h"

int16_t ser_avail = 0;
int16_t data_sent = 0;
uint8_t serial_send_data(uint16_t send_size, uint8_t* send_data){
   ser_avail = Serial.availableForWrite();
   data_sent = 0;
   if(ser_avail >= send_size){
      data_sent = Serial.write(send_data, send_size);
      if(data_sent > 0){
         return 1;
      } else {
         return 0;
      }
   } else {
      return 0;
   }
}

uint16_t i = 0;
uint8_t* data_pointer = 0;
uint16_t serial_get_data(uint16_t receive_size, uint8_t* receive_data){
   i = 0;
   data_pointer = receive_data;
   ser_avail = (int16_t) Serial.available();

   //for(uint16_t count=0;count<receive_size;count++){
   //   receive_data[count] = 0;
   //}
   if(ser_avail >= receive_size){
      while(i<receive_size){
         *data_pointer = Serial.read();
         data_pointer++;
         i++;
      }
   }
   return i;
}

void serial_flush(void){
   Serial.flush();
}


