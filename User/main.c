#include "stm32f10x.h"
#include "usart.h"
#include "systick.h"
#include "adc.h"
#include "tim.h"
#include "motor.h"
#include "pid.h"
#include "mpu6050.h"
#include "app.h"
#include "exti.h"
#include "tim2.h"

extern union dataConvert PAValue,IAValue,DAValue,PBValue,IBValue,DBValue;
extern Acceleration  AccelerationMsg;
extern AngularSpeed  AngularSpeedMsg;
extern Angle AngleMsg;
extern uint16_t ADCCnovertedValue[30];//ADC转换的值
extern PID_Val para;
PID pid;
uint16_t angleAve;
uint16_t DealDate(uint16_t* table);
void para_Init(PID_Val *para);
int main(void)
{
		PID_Init(&pid);
		para_Init(&para);
		USARTInit();
		ADC_Config();
		MPU6050_Init();
		EXTI_Config();
		TIM_CompositeInit();
		TIM2_CompositeInit();
		MOTOR_configuration();
		MOTOR_SetFrenquency(1000);
		MOTOR_Set1DutyRatio(0);
		MOTOR_Set2DutyRatio(0);
		while(1)
		{
				angleAve = DealDate(ADCCnovertedValue);   //起摆
				if(angleAve>pid.int_mubiao-300 && angleAve<pid.int_mubiao+300)
				{
						TIM_ITConfig(TIM3, TIM_IT_Update,ENABLE);
						TIM_ITConfig(TIM2, TIM_IT_Update,DISABLE);
				}
		}
		
}

/**********************AD采集的中值滤波****************************/
uint16_t DealDate(uint16_t table[])
{
		uint8_t i,j;
		uint16_t temp,ave;
		ADC_WaitForConvertEnd();
		for(j=0;j<29;j++)
			for(i=0;i<29-j;i++)
			if(table[i]>table[i+1])
			{
					temp=table[i];
					table[i]=table[i+1];
					table[i+1]=temp;
			}
		ave = (table[14]+table[15])*0.5;
		ave = VoltageConvert(ave);
		return ave;
}
/******************参数初始化************************/
void para_Init(PID_Val *para)
{
		para->kpA = 2.80;		para->kiA = 0.0008;		para->kdA = 11.50;
		para->kpB = 2.30;		para->kiB = 0;    		para->kdB = 0;
}

