#include "pid.h"
#include "mpu6050.h"
extern Acceleration  AccelerationMsg;
extern AngularSpeed  AngularSpeedMsg;
extern Angle AngleMsg;

int PIDCalculate(PID_Val *para,PID *p,int now_Val)
{
		int dError;//微分误差
		int Error;//误差
		float speedItem;
		int posKpItem,posKiItem,posKdItem;
		/********************速度环************************/
		speedItem = para->kpB*AngularSpeedMsg.Z_AngSpeed;
		/*********************位置环*****************************/
		Error = p->int_mubiao-now_Val;
		p->int_lastError = p->int_nowError;
		p->int_nowError = Error;
		p->int_sumError += Error;
		dError = p->int_nowError-p->int_lastError;
		posKpItem = para->kpA*Error;
		posKiItem = para->kiA*p->int_sumError;
		posKdItem = para->kdA*dError;
		/****************控制作用的叠加*******************/
		return (posKpItem+posKiItem+posKdItem-speedItem);	
}

void PID_Init(PID *pid)
{
		pid->int_lastError = 0;
		pid->int_nowError = 0;
		pid->int_mubiao = 2080;
		pid->int_sumError = 0;
}

