#ifndef __AX_DELAY_H
#define __AX_DELAY_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//OpenCTR �ӿں���
void AX_DELAY_Init(void); //��ʱ��ʼ��
void AX_Delayus(uint32_t us);  //���΢����ʱ
void AX_Delayms(uint16_t ms);  //���������ʱ

#endif 
