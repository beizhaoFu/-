#ifndef MPU6050_H
#define MPU6050_H

#include "stm32f10x.h"
#include <stdio.h>

typedef struct
{
		float X_AcceValue;	//X方向的加速度
		float Y_AcceValue;	//Y方向的加速度
		float Z_AcceValue;	//Z方向的加速度
		
}Acceleration;

typedef struct
{
		float X_AngSpeed;	//X方向角速度
		float Y_AngSpeed;	//Y方向角速度
		float Z_AngSpeed;	//Z方向角速度
}AngularSpeed;

typedef struct
{
		float X_Angle;	//X角度
		float Y_Angle;	//Y角度
		float Z_Angle;	//Z角度
}Angle;

void MPU6050_Init(void);
void MPU6050_RetToZero(void);
#endif
