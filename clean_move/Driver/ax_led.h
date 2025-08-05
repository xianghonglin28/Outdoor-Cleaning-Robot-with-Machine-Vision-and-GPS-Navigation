#ifndef __AX_LED_H
#define __AX_LED_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//�ӿں���
void AX_LED_Init(void);

#define AX_LED_Red_Off()        GPIO_SetBits(GPIOD, GPIO_Pin_3)      //LEDR��ɫϨ��
#define AX_LED_Red_On()  	    GPIO_ResetBits(GPIOD, GPIO_Pin_3)    //LEDR��ɫ����
#define AX_LED_Red_Toggle()     GPIO_WriteBit(GPIOD, GPIO_Pin_3, (BitAction) (1 - GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_3)))	//LEDR��ɫ״̬��ת

#define AX_LED_Green_Off()  	GPIO_SetBits(GPIOD, GPIO_Pin_4)      //LEDG��ɫϨ��
#define AX_LED_Green_On()		GPIO_ResetBits(GPIOD, GPIO_Pin_4)    //LEDG��ɫ����
#define AX_LED_Green_Toggle()   GPIO_WriteBit(GPIOD, GPIO_Pin_4, (BitAction) (1 - GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_4)))	//LEDG��ɫ״̬��ת


#endif 
