#include "stm32f10x_it.h"
#include <stdio.h>
#include "usart.h"
#include "systick.h"
#include "motor.h"
#include "pid.h"
#include "mpu6050.h"

void PID_Control(PID_Val *para,PID *pid,int currentAngle);

//---------串口接收ＤＭＡ中断---------------
extern uint8_t parameter[48];
union dataConvert PAValue;
union dataConvert IAValue;
union dataConvert DAValue;
union dataConvert PBValue;
union dataConvert IBValue;
union dataConvert DBValue;
Acceleration  AccelerationMsg;
AngularSpeed  AngularSpeedMsg;
Angle AngleMsg;
PID_Val para;

void DMA1_Channel5_IRQHandler(void)  //
{
		uint8_t i;
		//清除标志位  
    DMA_ClearFlag(DMA1_FLAG_TC5); 	
		DMA_ClearITPendingBit(DMA1_IT_TC5);
		for(i=0;i<8;i++)
		{
				PAValue.table[i] = parameter[i];
		}
		for(i=0;i<8;i++)
		{
				IAValue.table[i] = parameter[8+i];
		}
		for(i=0;i<8;i++)
		{
				DAValue.table[i] = parameter[i+16];
		}
		for(i=0;i<8;i++)
		{
				PBValue.table[i] = parameter[i+24];
		}
		for(i=0;i<8;i++)
		{
				IBValue.table[i] = parameter[32+i];
		}
		for(i=0;i<8;i++)
		{
				DBValue.table[i] = parameter[i+40];
		}
		para.kpA = PAValue.num;para.kiA = IAValue.num*0.01;para.kdA = DAValue.num;
		para.kpB = PBValue.num;para.kiB = IBValue.num;para.kdB = DBValue.num;
}



//-----------------------TIM3中断---------------------------
extern PID pid;
extern uint16_t angleAve;
void TIM3_IRQHandler(void)
{
		if(TIM_GetFlagStatus(TIM3,TIM_FLAG_Update) == SET)
		{
				PID_Control(&para,&pid,angleAve);
		}
		TIM_ClearFlag(TIM3, TIM_FLAG_Update);//清除TIMx的待处理标志位
		TIM_ClearITPendingBit(TIM3, TIM_IT_Update);//清除TIMx的中断待处理位
}

//------------------PID调节-----------------------
void PID_Control(PID_Val *para,PID *pid,int currentAngle)
{
		int pid_out,error;
		if(currentAngle>pid->int_mubiao-300 && currentAngle<pid->int_mubiao+300)
		{
				error = currentAngle - pid->int_mubiao;
				if(error>-8 && error<8)
				{
						MOTOR_Set1DutyRatio(0);
						MOTOR_Set2DutyRatio(0);
						return;
				}
				else
				{
						pid_out = PIDCalculate(para,pid,currentAngle);
						if(pid_out>0)
						{
								if(pid_out>700)
									pid_out=700;
								MOTOR_ForWard(pid_out);
						}
						else
						{
								if(pid_out<-700)
									pid_out = -700;
								pid_out = pid_out*-1;
								MOTOR_BackWard(pid_out);
						}
			}
		}
		else
		{
				MOTOR_Set1DutyRatio(25);
				MOTOR_Set2DutyRatio(25);
		}
}

/***************************************************************************************
*函数功能：串口2中断，用于读取mpu6050的数据，并对mpu6050的数据打包
***************************************************************************************/
uint8_t RxBuffer[11];
uint8_t count = 0;
uint8_t Temp;
uint8_t flag = 0;
void USART2_IRQHandler(void)
{
		if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)  //接收中断有效,若接收数据寄存器满
		{
				Temp = USART_ReceiveData(USART2);//返回USARTx最近接收到的数据
				if((Temp == 0x55) && (count == 0))//判断是不是包头
					flag = 1;
				if(flag == 1)//如果标志位被置为就进入
				{
						RxBuffer[count++] = Temp;
						if(count == 11)
						{
								flag = 0;
								count = 0;
								switch(RxBuffer[1])//数据分流整理
								{
										case 0x51: //标识这个包是加速度包
										{
												AccelerationMsg.X_AcceValue = ((short)(RxBuffer[3]<<8 | RxBuffer[2]))/32768.0*16;      //X轴加速度
												AccelerationMsg.Y_AcceValue = ((short)(RxBuffer[5]<<8 | RxBuffer[4]))/32768.0*16;      //Y轴加速度
												AccelerationMsg.Z_AcceValue = ((short)(RxBuffer[7]<<8 | RxBuffer[6]))/32768.0*16; 		 //Z轴加速度												AccelerationMsg.Z_AcceValue -= 1.080566;
										}
										break;
										case 0x52: //标识这个包是角速度包
										{
												AngularSpeedMsg.X_AngSpeed = ((short)(RxBuffer[3]<<8| RxBuffer[2]))/32768.0*2000;      //X轴角速度
												AngularSpeedMsg.Y_AngSpeed = ((short)(RxBuffer[5]<<8| RxBuffer[4]))/32768.0*2000;      //Y轴角速度
												AngularSpeedMsg.Z_AngSpeed = ((short)(RxBuffer[7]<<8| RxBuffer[6]))/32768.0*2000;      //Z轴角速度
												AngularSpeedMsg.Z_AngSpeed += 0.976563;//误差补偿
										}
										break;
										case 0x53: //标识这个包是角度包
										{
												AngleMsg.X_Angle = ((short)(RxBuffer[3]<<8| RxBuffer[2]))/32768.0*180;   //X轴滚转角（x 轴）
												AngleMsg.Y_Angle = ((short)(RxBuffer[5]<<8| RxBuffer[4]))/32768.0*180;   //Y轴俯仰角（y 轴）
												AngleMsg.Z_Angle = ((short)(RxBuffer[7]<<8| RxBuffer[6]))/32768.0*180;   //Z轴偏航角（z 轴）
										}
										break;
										default: 
										break;
								}
								
						}
				}
				
		}
		USART_ClearFlag(USART2, USART_FLAG_RXNE);//清除USARTx的待处理标志位
		USART_ClearITPendingBit(USART2, USART_IT_RXNE);//清除USARTx的中断待处理位
}
/***************************************************************************************
*函数功能：定时器2的中断程序，用于起摆
***************************************************************************************/
typedef enum {BackWard = 0, Forward = 1} DirectionStatus;
DirectionStatus state=Forward;
void TIM2_IRQHandler(void)
{
		if(TIM_GetFlagStatus(TIM2,TIM_FLAG_Update) == SET)
		{
				if(state)
				{
						MOTOR_ForWard(176);
						state = !state;
				}
				else
				{
						MOTOR_BackWard(176);
						state = !state;
				}
				
		}
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除TIMx的待处理标志位
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除TIMx的中断待处理位
}
/***************************************************************************************
*函数功能：外部中断的中断服务程序
***************************************************************************************/
extern uint16_t DealDate(uint16_t* table);
extern uint16_t ADCCnovertedValue[30];//ADC转换的值
void EXTI15_10_IRQHandler(void)
{
		if(EXTI_GetITStatus(EXTI_Line12) == SET)
		{
				delay_ms(10);
				if(EXTI_GetITStatus(EXTI_Line12) == SET)
				{
						EXTI_ClearITPendingBit(EXTI_Line12);//清除EXTI线路挂起位
						TIM_ITConfig(TIM2, TIM_IT_Update,ENABLE);//使能数据更新中断
				}
				
		}
		
		if( EXTI_GetITStatus(EXTI_Line13) == SET)
		{
				EXTI_ClearITPendingBit(EXTI_Line13);//清除EXTI线路挂起位
		}
		
		if( EXTI_GetITStatus(EXTI_Line14) == SET)
		{
				EXTI_ClearITPendingBit(EXTI_Line14);//清除EXTI线路挂起位
		}
}
