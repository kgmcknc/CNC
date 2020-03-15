/*
 * cnc_spi.cpp
 *
 *  Created on: Aug 18, 2018
 *      Author: Kyle
 */

#include "cnc_serial.h"
#include "Arduino.h"

uint8_t serial_send_data(uint32_t send_size, uint8_t* send_data){
   Serial.write(send_data, send_size);
   return 1;
}

uint32_t serial_get_data(uint32_t receive_size, uint8_t* receive_data){
   uint32_t i=0;
   uint8_t* data_pointer = receive_data;
   if((uint32_t) Serial.available() >= receive_size){
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


