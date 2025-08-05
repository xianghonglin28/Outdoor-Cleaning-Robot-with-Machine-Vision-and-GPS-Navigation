/**			                                                    
		   ____                    _____ _______ _____       XTARK@���˴���
		  / __ \                  / ____|__   __|  __ \ 
		 | |  | |_ __   ___ _ __ | |       | |  | |__) |
		 | |  | | '_ \ / _ \ '_ \| |       | |  |  _  / 
		 | |__| | |_) |  __/ | | | |____   | |  | | \ \ 
		  \____/| .__/ \___|_| |_|\_____|  |_|  |_|  \_\
				| |                                     
				|_|                OpenCTR   �����˿�����
									 
  ****************************************************************************** 
  *           
  * ��Ȩ���У� XTARK@���˴���  ��Ȩ���У�����ؾ�
  * ��˾��վ�� www.xtark.cn   www.tarkbot.com
  * �Ա����̣� https://xtark.taobao.com  
  * ����΢�ţ� ���˴��£���ע���ںţ���ȡ���¸�����Ѷ��
  *      
  ******************************************************************************
  * @��  ��  Musk Han@XTARK
  * @��  ��  V1.0
  * @��  ��  LED�ƿ���
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
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

/******************* (C) ��Ȩ 2023 XTARK **************************************/
