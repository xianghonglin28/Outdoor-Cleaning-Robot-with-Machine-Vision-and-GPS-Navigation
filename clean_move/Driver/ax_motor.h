#ifndef __AX_MOTOR_H
#define __AX_MOTOR_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//X-SOFT �ӿں���
void AX_MOTOR_AB_Init(void); //���PWM���Ƴ�ʼ��
void AX_MOTOR_A_SetSpeed(int16_t speed);   //���A����
void AX_MOTOR_B_SetSpeed(int16_t speed);   //���B����

void AX_MOTOR_CD_Init(void); //���PWM���Ƴ�ʼ��
void AX_MOTOR_C_SetSpeed(int16_t speed);   //���C����
void AX_MOTOR_D_SetSpeed(int16_t speed);   //���D����

#endif

