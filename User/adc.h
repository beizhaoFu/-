#ifndef ADC_H
#define ADC_H

#include "stm32f10x.h"
#define ADC1_DR_ADDRESS 		 0x4001244C
void ADC_Config(void);
void ADC_WaitForConvertEnd(void);
#endif
