#ifndef PID_H
#define PID_H

#include "stm32f10x.h"

#define abs(x)  (x>0)?x:(-1*x)

typedef struct PID
{
		int int_mubiao; //目标值
		int int_nowError;//当前误差
		int int_lastError;//上一次的误差
		int int_sumError;//误差和
}PID;

typedef struct PID_Val
{
		float kpA;
		float kiA;
		float kdA;
	
		float kpB;
		float kiB;
		float kdB;
}PID_Val;

int PIDCalculate(PID_Val *para,PID *p,int now_Val);
void PID_Init(PID *pid);

#endif
