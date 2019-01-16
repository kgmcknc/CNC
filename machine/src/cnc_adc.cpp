/*
 * cnc_adc.cpp
 *
 *  Created on: Sep 8, 2018
 *      Author: Kyle
 */

#include "cnc_gpio.h"
#include "cnc_motors.h"
#include "cnc_adc.h"
#include "cnc_motors.h"
#include "em_cmu.h"
#include "em_adc.h"

uint8_t adc_channel_select = 0;
uint8_t adc_channel_valid[NUM_ADCS] = {0};
uint32_t adc_channel_data[NUM_ADCS] = {0};

void init_adc(void){
	CMU_ClockEnable(cmuClock_ADC0, true);

	ADC_Init_TypeDef          adc_init        = ADC_INIT_DEFAULT;
	ADC_InitScan_TypeDef      adc_scan_init   = ADC_INITSCAN_DEFAULT;

	/* Initialize timebases */
	adc_init.timebase = ADC_TimebaseCalc(0);
	adc_init.prescale = ADC_PrescaleCalc(100000, 0);
	ADC_Init(ADC0, &adc_init);

	/* Set input to temperature sensor. Reference must be 1.25V */
	adc_scan_init.reference       = adcRefVDD;
	adc_scan_init.acqTime         = adcAcqTime16; /* Minimum time for temperature sensor */
	adc_scan_init.rep             = true;

	ADC_ScanSingleEndedInputAdd(&adc_scan_init, adcScanInputGroup0, adcPosSelAPORT3XCH8);
	ADC_ScanSingleEndedInputAdd(&adc_scan_init, adcScanInputGroup0, adcPosSelAPORT3YCH9);
	ADC_ScanSingleEndedInputAdd(&adc_scan_init, adcScanInputGroup0, adcPosSelAPORT3XCH10);
	ADC_ScanSingleEndedInputAdd(&adc_scan_init, adcScanInputGroup0, adcPosSelAPORT3YCH11);

	adc_channel_select = 0;
	//adc_scan_init.posSel      = adcPosSelTEMP;
	ADC_InitScan(ADC0, &adc_scan_init);
	ADC_IntEnable(ADC0, ADC_IEN_SCAN);
	NVIC_EnableIRQ(ADC0_IRQn);
	ADC_Start(ADC0, adcStartScan);
}

void ADC0_IRQHandler(void){
	ADC_IntClear(ADC0, ADC_IEN_SCAN);

	adc_channel_data[adc_channel_select] = ADC_DataScanGet(ADC0);
	adc_channel_valid[adc_channel_select] = 1;

	if(adc_channel_select < (NUM_ADCS - 1)){
		adc_channel_select++;
	} else {
		adc_channel_select = 0;
		update_pid = 1;
	}
}
