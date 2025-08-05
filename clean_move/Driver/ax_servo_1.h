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
  * @��  ��  PWM�ӿڶ������
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_SERVO_H
#define __AX_SERVO_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"




//X-SOFT �ӿں���
void AX_SERVO_S1_Init(void);                 //����ӿڳ�ʼ��
void AX_SERVO_S1_SetAngle(int16_t angle);    //������� 
void AX_SERVO_S1_Release(void);              //�ͷŶ������

void AX_SERVO_S2_Init(void);                 //����ӿڳ�ʼ��
void AX_SERVO_S2_SetAngle(int16_t angle);    //�������  
void AX_SERVO_S2_Release(void);              //�ͷŶ������

void AX_SERVO_S3456_Init(void);              //����ӿڳ�ʼ��
void AX_SERVO_S3_SetAngle(int16_t angle);    //�������   
void AX_SERVO_S4_SetAngle(int16_t angle);    //�������
void AX_SERVO_S5_SetAngle(int16_t angle);    //�������   
void AX_SERVO_S6_SetAngle(int16_t angle);    //�������
void AX_SERVO_S3_Release(void);              //�ͷŶ������
void AX_SERVO_S4_Release(void);              //�ͷŶ������
void AX_SERVO_S5_Release(void);              //�ͷŶ������
void AX_SERVO_S6_Release(void);              //�ͷŶ������

//void AX_SERVO_INIT(void);//��е�۳�ʼ״̬
//void AX_SERVO_up(void);//ץȡ
//void AX_SERVO_down(void);//����

// ��е�۳�ʼ״̬
void AX_SERVO_INIT(void); 
void AX_SERVO_Grab(void);
void AX_SERVO_Release(void);


#endif

/******************* (C) ��Ȩ 2023 XTARK **************************************/

