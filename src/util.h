/*
 * util.h
 *
 *  Created on: Aug 23, 2018
 *      Author: Kyle
 */

#ifndef SRC_UTIL_H_
#define SRC_UTIL_H_

#include "stdint.h"

#define MAX_STRING_LENGTH 4

int32_t string_to_int32(char length_string[MAX_STRING_LENGTH], uint8_t length);

void cnc_sleep(uint32_t count);


#endif /* SRC_UTIL_H_ */
