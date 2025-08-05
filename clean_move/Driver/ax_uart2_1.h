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
  * @��  ��  �ƶ�eps8266ͨ��
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_UART2_H
#define __AX_UART2_H

#include "stm32f4xx.h"
#include "ax_servo.h"

// ��е��״̬�ṹ��
typedef struct {
    float angles[6];      // 6���ؽڽǶ�
    uint8_t emergency;    // ��ͣ״̬
    uint8_t mode;         // ����ģʽ
} ArmState;

// ��������
void UART2_Init(void);
void UART2_SendString(const char *str);
void ProcessUARTCommand(const char *cmd);
void UpdateArmPosition(void);
void EmergencyStop(uint8_t state);

extern ArmState arm_state;

//#define MAX_SAFE_ANGLE      90.0f   // ������ȫ�Ƕ�
//#define MIN_SAFE_ANGLE     -90.0f   // ��С��ȫ�Ƕ�
//#define MAX_STEP           5.0f     // �������Ƕȱ仯��
//#define OVERLOAD_THRESHOLD 800      // ������ص�����ֵ(mA)

#endif 

/******************* (C) ��Ȩ 2023 XTARK **************************************/
