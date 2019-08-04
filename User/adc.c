#include "adc.h"

uint16_t ADCCnovertedValue[30];//ADC转换的值
/***********************************************************************************************
*函数名称：void ADC_GPIO_Configuration(void)
*函数功能：ADC的GPIO和时钟设置
*输入参数：无
*输出参数：无
***********************************************************************************************/
static void ADC_GPIO_Configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);//使能ADC1
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC,ENABLE);//使能GPIOC
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);//使能或者失能AHB外设时钟
		RCC_ADCCLKConfig(RCC_PCLK2_Div6);//设置ADC时钟（ADCCLK）
		
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AIN;//ADC选择模拟输入模式
		GPIO_Init(GPIOC, &GPIO_InitStructure);//初始化
	
}

/***********************************************************************************************
*函数名称：void ADC_Mode_Configuration(void)
*函数功能：ADC的模式配置
*输入参数：无
*输出参数：无
***********************************************************************************************/
static void ADC_Mode_Configuration(void)
{
		ADC_InitTypeDef ADC_InitStructure;
		ADC_InitStructure.ADC_Mode = ADC_Mode_Independent;//独立模式
		ADC_InitStructure.ADC_ScanConvMode = DISABLE;//单次模式(单通道)
		ADC_InitStructure.ADC_ContinuousConvMode = ENABLE;//连续模式
		ADC_InitStructure.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None;//转换由软件而不是外部触发启动
		ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;//ADC数据右对齐
		ADC_InitStructure.ADC_NbrOfChannel = 1;//规则转换的ADC的通道数目,取值范围为1-16
		ADC_Init(ADC1,&ADC_InitStructure);//初始化配置
		/*设置指定ADC的规则组通道,设置它们的转化顺序和采样时间*/
		ADC_RegularChannelConfig(ADC1,ADC_Channel_10, 1, ADC_SampleTime_71Cycles5);//设置ADC采样周期	
	
}

/***********************************************************************************************
*函数名称：void ADC_DMA_Configuration(void)
*函数功能：ADC的ＤＭＡ配置
*输入参数：无
*输出参数：无
***********************************************************************************************/
static void ADC_DMA_Configuration(void)
{
		DMA_InitTypeDef DMA_InitStructure;
		DMA_DeInit(DMA1_Channel1);//将DMA的通道x寄存器重设为缺省值(DMA1 通道1)
		DMA_InitStructure.DMA_PeripheralBaseAddr = ADC1_DR_ADDRESS;//外设基地址
		DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ADCCnovertedValue;//内存基地址
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;//外设作为数据传输的来源
		DMA_InitStructure.DMA_BufferSize = 30;//指定DMA通道的DMA缓存的大小,单位为数据单位
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;//外设地址寄存器不变
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;//内存地址寄存器不变
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_HalfWord;//数据宽度为16位
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_HalfWord;//数据宽度为16位
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;//工作在循环缓存模式
		DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;//非常高优先级
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;//非内存到内存传输
		DMA_Init(DMA1_Channel1, &DMA_InitStructure);
//		DMA_ITConfig(DMA1_Channel1,DMA_IT_TC,ENABLE);
}

/*************************************************************
*函数名称：ADC_NVIC_Configuration(void)
*函数功能：确定USART中断的优先级
*输入：无
*输出：无
*备注：无
*************************************************************/
static void ADC_NVIC_Configuration(void)
{
		NVIC_InitTypeDef NVIC_InitStructure;
		NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel1_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
}

/***********************************************************************************************
*函数名称：void ADC_STARTUP_Configuration(void)
*函数功能：ADC的所有配置
*输入参数：无
*输出参数：无
***********************************************************************************************/
static void ADC_STARTUP_Configuration(void)
{
		ADC_DMACmd(ADC1,ENABLE);//使能或者失能指定的ADC的DMA请求
		DMA_Cmd(DMA1_Channel1,ENABLE);//使能或者失能指定的通道x(DMA1 通道1)
		ADC_Cmd(ADC1,ENABLE);//使能指定的ADC
		ADC_ResetCalibration(ADC1);//复位校验
		while(ADC_GetResetCalibrationStatus(ADC1));
		ADC_StartCalibration(ADC1);//开始校验
		while(ADC_GetCalibrationStatus(ADC1));
		ADC_SoftwareStartConvCmd(ADC1,ENABLE);//软件使能ADC转换

}
/***********************************************************************************************
*函数名称：void ADC_Config(void)
*函数功能：ADC的所有配置
*输入参数：无
*输出参数：无
***********************************************************************************************/
void ADC_Config(void)
{
		ADC_GPIO_Configuration();
		ADC_Mode_Configuration();
		ADC_DMA_Configuration();
//		ADC_NVIC_Configuration();
		ADC_STARTUP_Configuration();
}

/********************************************************************************************
*函数名称：void ADC_WaitForConvertEnd(void)
*函数功能：等待ADC转换结束
*输入参数：无
*输出参数：无
********************************************************************************************/
void ADC_WaitForConvertEnd(void)
{
		while(!ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC));
		ADC_ClearFlag(ADC1,ADC_FLAG_EOC);//清除ADCx的待处理标志位
		while(!DMA_GetFlagStatus(DMA1_FLAG_TC1));
		DMA_ClearFlag(DMA1_FLAG_TC1 );//清除DMA通道x待处理标志位(DMA1通道1传输完成标志位)	
}



