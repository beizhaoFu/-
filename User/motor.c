#include "motor.h"

uint16_t ARR_Val;
/***********************************************************************************************
*函数名称：void TIM4_GPIO_configuration(void)
*函数功能：定时器4的时钟基准配置以及时钟设置
*输入参数：无
*输出参数：无
***********************************************************************************************/
static void TIM4_GPIO_configuration(void)
{
		GPIO_InitTypeDef GPIO_InitStructure;
		RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);//使能或者失能APB1外设时钟
		GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6|GPIO_Pin_7;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;//最高输出速率50MHz
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;//复用推挽输出
		GPIO_Init(GPIOB, &GPIO_InitStructure);//初始化外设GPIOx寄存器

}
/***********************************************************************************************
*函数名称：void TIM4_Mode_configuration(void)
*函数功能：定时器4的时钟基准配置以及时钟设置
*输入参数：无
*输出参数：无
***********************************************************************************************/
static void TIM4_Mode_configuration(void)
{
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);//使能或者失能APB1外设时钟
		
		TIM_TimeBaseStructure.TIM_Prescaler = 89;//预分频值
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;//向上计数模式
		TIM_TimeBaseStructure.TIM_Period = 800;//在下一个更新事件装入活动的自动重装载寄存器周期的值
		TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;//TDTS = Tck_tim
		TIM_TimeBaseStructure.TIM_RepetitionCounter = 0x01;//重复计数值
		TIM_TimeBaseInit(TIM4,&TIM_TimeBaseStructure);//初始化
		TIM_ARRPreloadConfig(TIM4, ENABLE);//使能或者失能TIMx在ARR上的预装载寄存器
		TIM_Cmd(TIM4, ENABLE);//使能或者失能TIMx外设
}

/***********************************************************************************************
*函数名称：void TIM4_PWM_configuration(void)
*函数功能：定时器4的PWM输出配置
*输入参数：无
*输出参数：无
***********************************************************************************************/
static void TIM4_PWM_configuration(void)
{
		TIM_OCInitTypeDef  TIM_OCInitStructure;
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 400;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC1Init(TIM4, &TIM_OCInitStructure);
		TIM_OC1PreloadConfig(TIM4, TIM_OCPreload_Enable);
		
		TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
		TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
		TIM_OCInitStructure.TIM_Pulse = 400;
		TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
		TIM_OC2Init(TIM4, &TIM_OCInitStructure);
		TIM_OC2PreloadConfig(TIM4, TIM_OCPreload_Enable);
}


/***********************************************************************************************
*函数名称：void MOTOR_configuration(void)
*函数功能：电机初始化配置
*输入参数：无
*输出参数：无
***********************************************************************************************/
void MOTOR_configuration(void)
{
		TIM4_GPIO_configuration();
		TIM4_Mode_configuration();
		TIM4_PWM_configuration();
}


/***********************************************************************************************
*函数名称：void MOTOR_SetFrenquency(void)
*函数功能：PWM频率调节
*输入参数：Fren:想要的频率
*输出参数：无
***********************************************************************************************/
void MOTOR_SetFrenquency(uint32_t Fren)
{		
		ARR_Val = 800000/Fren;
		TIM_SetAutoreload(TIM4, ARR_Val);	
}

/***********************************************************************************************
*函数名称：void MOTOR_Set1DutyRatio(int PWM1_Val)
*函数功能：设置PWM1占空比
*输入参数：PWM1_Val:占空比值
*输出参数：无
***********************************************************************************************/
void MOTOR_Set1DutyRatio(int PWM1_Val)
{
		TIM_SetCompare1(TIM4, PWM1_Val);//设置TIM4捕获比较1寄存器值
}

/***********************************************************************************************
*函数名称：void MOTOR_Set2DutyRatio(int PWM2_Val)
*函数功能：设置PWM2占空比
*输入参数：PWM2_Val:占空比值
*输出参数：无
***********************************************************************************************/
void MOTOR_Set2DutyRatio(int PWM2_Val)
{
		TIM_SetCompare2(TIM4, PWM2_Val);//设置TIM4捕获比较2寄存器值
}

/*******************************************************************************************
*函数名称：void MOTOR_ForWard(int PWM_Val)
*函数功能：实现电机的正转(顺时钟)
*输入参数：PWM_Val:电机正转的快慢
*输出参数：无
*******************************************************************************************/
void MOTOR_ForWard(int PWM_Val)
{
		MOTOR_Set1DutyRatio(0);
		MOTOR_Set2DutyRatio(PWM_Val);
}

/*******************************************************************************************
*函数名称：void MOTOR_BackWard(int PWM_Val)
*函数功能：实现电机的反转(逆时针)
*输入参数：PWM_Val:电机反转的快慢
*输出参数：无
*******************************************************************************************/
void MOTOR_BackWard(int PWM_Val)
{		
		MOTOR_Set1DutyRatio(PWM_Val);
		MOTOR_Set2DutyRatio(0);
}
