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
  * @��  ��  �������
  *
  ******************************************************************************
  * @˵  ��
  *
  * 1.��ȡ��ֵ����
  * 
  ******************************************************************************
  */

#include "ax_key.h"
#include "ax_delay.h"
#include "ax_sys.h"

/**
  * @��  ��  KEY ������ʼ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_KEY_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//GPIO���� 
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOE, ENABLE); //����GPIOʱ�� 
	

	//����ģʽѡ��S1 GPIO  ����ģʽ
	GPIO_SetBits(GPIOE,  GPIO_Pin_0);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);
	
}

/**
  * @��  ��  KEY ��ȡ����ֵ
  * @��  ��  ��
  * @����ֵ  ����ֵ����������Ϊ1��̧��Ϊ0
  */
uint8_t AX_KEY_Scan(void)
{
	if(PEin(0) == 0)		
	{
		return 1;
	}
	
	return 0;
}



/******************* (C) ��Ȩ 2023 XTARK **************************************/
