/*
 * util.cpp
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#include "util.h"

int32_t string_to_int32(char length_string[MAX_STRING_LENGTH], uint8_t byte_length){
	int32_t temp = 0;
	uint8_t length_count = 0;

	// parse string into int
	for(length_count=0;length_count<byte_length;length_count++){
		temp = (temp << 8) | length_string[byte_length - 1 - length_count];
	}

	// check for negative and pad
	if(length_string[byte_length - 1 - length_count] >> 7){
		// top bit was set... so fill negative
		temp = (-1 << (byte_length*8)) | temp;
	}

	return temp;
}

