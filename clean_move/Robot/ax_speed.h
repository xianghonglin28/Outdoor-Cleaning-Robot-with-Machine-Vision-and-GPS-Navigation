#ifndef __AX_SPEED_H
#define __AX_SPEED_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//���PID�ջ��ٶȿ��ƺ���
int16_t AX_SPEED_PidCtlA(float spd_target, float spd_current);   //PID���ƺ��������A
int16_t AX_SPEED_PidCtlB(float spd_target, float spd_current);    //PID���ƺ��������B
int16_t AX_SPEED_PidCtlC(float spd_target, float spd_current);    //PID���ƺ��������C
int16_t AX_SPEED_PidCtlD(float spd_target, float spd_current);    //PID���ƺ��������D


#endif
