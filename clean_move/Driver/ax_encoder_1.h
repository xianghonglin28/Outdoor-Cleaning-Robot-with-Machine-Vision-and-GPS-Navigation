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
  * @��  ��  PWM������
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_ENCODER_H
#define __AX_ENCODER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//X-SOFT �ӿں���
void AX_ENCODER_A_Init(void);          //��������ʼ��
uint16_t AX_ENCODER_A_GetCounter(void);          //��������ȡ��������ֵ
void AX_ENCODER_A_SetCounter(uint16_t count);    //���������ü�������ֵ

void AX_ENCODER_B_Init(void);          //��������ʼ��
uint16_t AX_ENCODER_B_GetCounter(void);          //��������ȡ��������ֵ
void AX_ENCODER_B_SetCounter(uint16_t count);    //���������ü�������ֵ

void AX_ENCODER_C_Init(void);          //��������ʼ��
uint16_t AX_ENCODER_C_GetCounter(void);          //��������ȡ��������ֵ
void AX_ENCODER_C_SetCounter(uint16_t count);    //���������ü�������ֵ

void AX_ENCODER_D_Init(void);          //��������ʼ��
uint16_t AX_ENCODER_D_GetCounter(void);          //��������ȡ��������ֵ
void AX_ENCODER_D_SetCounter(uint16_t count);    //���������ü�������ֵ

#endif

/******************* (C) ��Ȩ 2023 XTARK **************************************/
