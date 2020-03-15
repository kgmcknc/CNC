/*
 * cnc_adc.h
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#ifndef SRC_CNC_ADC_H_
#define SRC_CNC_ADC_H_

#define NUM_ADCS 4

#ifdef SILABS
   #include "em_cmu.h"
#else
   #include "Arduino.h"
#endif

#include "stdint.h"

extern uint8_t adc_channel_valid[NUM_ADCS];
extern uint32_t adc_channel_data[NUM_ADCS];

void init_adc(void);

#endif /* SRC_CNC_ADC_H_ */
