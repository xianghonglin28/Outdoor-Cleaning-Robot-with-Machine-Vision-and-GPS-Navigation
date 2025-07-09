#ifndef __AX_BEEP_H
#define __AX_BEEP_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

// �ӿں���
void AX_BEEP_Init(void);

// ���������������궨��
#define AX_BEEP_Off()       GPIO_ResetBits(GPIOE, GPIO_Pin_1)       //����������
#define AX_BEEP_On()		GPIO_SetBits(GPIOE, GPIO_Pin_1)     //����������
#define AX_BEEP_Toggle()    GPIO_WriteBit(GPIOE, GPIO_Pin_1, (BitAction) (1 - GPIO_ReadInputDataBit(GPIOE, GPIO_Pin_1)))    //������״̬��ת

#endif 
