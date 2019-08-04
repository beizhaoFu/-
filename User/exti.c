#include "exti.h"

/************************************************************************************************
*函数名称：void EXTI_GPIO_Configuration(void)
*函数功能：对按键IO口和时钟的配置
*输入参数：无
*输出参数：无
************************************************************************************************/
static void EXTI_GPIO_Configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		EXTI_InitTypeDef EXTI_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能或者失能APB2外设时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能或者失能APB2外设时钟
	
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_13|GPIO_Pin_14;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;//上拉输入
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化外设GPIOx寄存器
		
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource12);//选择GPIO管脚用作外部中断线路
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource13);//选择GPIO管脚用作外部中断线路
		GPIO_EXTILineConfig(GPIO_PortSourceGPIOB, GPIO_PinSource14);//选择GPIO管脚用作外部中断线路
		
		EXTI_InitStructure.EXTI_Line = EXTI_Line12|EXTI_Line13|EXTI_Line14;//中断线
		EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;//中断请求
		EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;//下降沿为中断请求
		EXTI_InitStructure.EXTI_LineCmd = ENABLE;//使能
		EXTI_Init(&EXTI_InitStructure);//检查指定的EXTI线路触发请求发生与否

}

/********************************************************************************************
*函数名称：void EXTI_NVIC_Configuration(void)
*函数功能：对外部中断的优先级进行配置
*输入参数：无
*输出参数：无
********************************************************************************************/
static void EXTI_NVIC_Configuration(void)
{
		NVIC_InitTypeDef   NVIC_InitStructure;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = EXTI15_10_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 3;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

/********************************************************************************************
*函数名称：void EXTI_Config(void)
*函数功能：对外部中断进行配置
*输入参数：无
*输出参数：无
********************************************************************************************/
void EXTI_Config(void)
{
		EXTI_GPIO_Configuration();
		EXTI_NVIC_Configuration();
}
