#ifndef APP_H
#define APP_H

#include "stm32f10x.h"
int VoltageConvert(uint16_t Voltage)
{
		if(Voltage>=2532)
		{
				Voltage = Voltage-2532;
		}
		else
		{
				Voltage = 4095-2532+Voltage;
		}
		return Voltage;
}

#endif
