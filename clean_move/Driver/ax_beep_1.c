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
  * @��  ��  BEEP����������
  ******************************************************************************
  * @˵  ��
  *
  * 
  ******************************************************************************
  */

#include "ax_beep.h"

/**
  * @��  ��  ��ʼ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_BEEP_Init(void) 
{
	GPIO_InitTypeDef GPIO_InitStructure;

	//GPIO���� 
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE); 
	
	//����GPIO  �������ģʽ 
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
	//�رշ�����
	GPIO_ResetBits(GPIOE,  GPIO_Pin_1);	

}	


/******************* (C) ��Ȩ 2023 XTARK **************************************/
