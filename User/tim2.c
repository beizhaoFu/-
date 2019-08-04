#include "tim2.h"
/******************************************************************
*函数名称：void TIM2_Mode_Configuration(void)
*函数功能：启动TIM2时钟，并且初始化TIM2
*输入：无
*输出：无
*******************************************************************/
static void TIM2_Mode_Configuration(void)
{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2,ENABLE);//使能定时器儿的时钟		
		TIM_TimeBaseStructure.TIM_Prescaler = 3599;//设置预分频值，切记要减1，以为它会自动加1
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Down;//向上计数模式
		TIM_TimeBaseStructure.TIM_Period = 10000;//预装载为20000,1s溢出一次
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//不进行时钟分割，对于时钟分割，暂时还不理解
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;//不进行重复计数
		TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure);//对结构体进行初始化
		/*需要养成一个好习惯，在使能一个中断的时候先将其清空*/
		TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清空更新标志位
		TIM_ClearITPendingBit(TIM2, TIM_IT_Update);//清除数据更新中断
		TIM_ITConfig(TIM2, TIM_IT_Update,DISABLE);//使能数据更新中断
		TIM_Cmd(TIM2, ENABLE);//使能定时器外设，在用到每一个外设时都需要使能
}

/******************************************************************
*函数名称：void TIM2_NVIC_Configuration(void)
*函数功能：配置定时器2的中断优先级
*输入：无
*输出：无
*******************************************************************/
static void TIM2_NVIC_Configuration(void)
{
		NVIC_InitTypeDef  NVIC_InitStructure;
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 4;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);

}
/******************************************************************
*函数名称：TIM_CompositeInit(void)
*函数功能：对TIM2进行所有的初始化操作
*输入：无
*输出：无
*******************************************************************/
void TIM2_CompositeInit(void)
{
		TIM2_Mode_Configuration();
		TIM2_NVIC_Configuration();
}
