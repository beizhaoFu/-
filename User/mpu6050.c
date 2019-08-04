#include "mpu6050.h"
/*************************************************************************************
*函数名称：void USART2_GPIOConfig(void)
*函数功能：USART1时钟配置和IO口配置
*输入参数：无
*输出参数：无
*************************************************************************************/
static void USART2_GPIOConfig(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);//使能或者失能APB2外设时钟
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO,ENABLE);//使能或者失能APB2外设时钟
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2,ENABLE);//使能或者失能APB1外设时钟
		//配置USART1的Tx
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//最高输出速率50MHz
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化外设GPIOx寄存器
		//配置USART1的Rx
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;//复用推挽输出
		GPIO_Init(GPIOA, &GPIO_InitStructure);//初始化外设GPIOx寄存器	
}

/*************************************************************************************
*函数名称：void USART2_ModeConfig(void)
*函数功能：USART1模式配置
*输入参数：无
*输出参数：无
*************************************************************************************/
static void USART2_ModeConfig(void)
{
		USART_InitTypeDef USART_InitStructure;
		USART_InitStructure.USART_BaudRate = 115200;//波特率
		USART_InitStructure.USART_WordLength = USART_WordLength_8b;//8位数据
		USART_InitStructure.USART_StopBits = USART_StopBits_1;//在帧结尾传输1个停止位
		USART_InitStructure.USART_Parity = USART_Parity_No;//无校验
		USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;//发送和接收都使能
		USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//硬件流控制失能
		USART_Init(USART2,&USART_InitStructure);
		USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//使能或者失能指定的USART中断
		USART_Cmd(USART2, ENABLE);//使能或者失能USART外设
}

/*************************************************************************************
*函数名称：void UASRT2_NVIC_Configuration(void)
*函数功能：串口2接收中断配置
*输入参数：无
*输出参数：无
*************************************************************************************/
static void UASRT2_NVIC_Configuration(void)
{
		NVIC_InitTypeDef  NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

/*************************************************************************************
*函数名称：void MPU6050_Init(void)
*函数功能：MPU6050的初始化
*输入参数：无
*输出参数：无
*************************************************************************************/
void MPU6050_Init(void)
{
		USART2_GPIOConfig();
		USART2_ModeConfig();
		UASRT2_NVIC_Configuration();
}

/*************************************************************************************
*函数名称：void MPU6050_RetToZero(void)
*函数功能：mpu6050的Z轴归零
*输入参数：无
*输出参数：无
*************************************************************************************/
void MPU6050_RetToZero(void)
{
		USART_SendData(USART2,0XFF);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  //等待串口2发送完毕
		USART_SendData(USART2,0XAA);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  //等待串口2发送完毕
		USART_SendData(USART2,0X52);
		while(USART_GetFlagStatus(USART2, USART_FLAG_TXE) == RESET);  //等待串口2发送完毕
}

