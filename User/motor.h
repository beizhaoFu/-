#ifndef MOTOR_H
#define MOTOR_H

#include "stm32f10x.h"

void MOTOR_configuration(void);
void MOTOR_SetFrenquency(uint32_t Fren);
void MOTOR_Set1DutyRatio(int PWM1_Val);
void MOTOR_Set2DutyRatio(int PWM2_Val);
void MOTOR_ForWard(int PWM_Val);
void MOTOR_BackWard(int PWM_Val);

#endif
