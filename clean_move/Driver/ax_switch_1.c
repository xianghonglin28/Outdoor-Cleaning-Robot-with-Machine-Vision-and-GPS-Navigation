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
  * @��  ��  ���뿪�ؼ��
  *
  ******************************************************************************
  * @˵  ��
  *
  * 
  * 
  ******************************************************************************
  */

#include "ax_switch.h"

/**
  * @��  ��  ���뿪�� ��ʼ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_SW_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	// GPIO����
	RCC_AHB1PeriphClockCmd( RCC_AHB1Periph_GPIOB, ENABLE); //����GPIOʱ�� 
	

	//����ģʽѡ��S1 GPIO  ����ģʽ
	GPIO_SetBits(GPIOB,  GPIO_Pin_0);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	//����ģʽѡ��S2 GPIO  ����ģʽ 
	GPIO_SetBits(GPIOB,  GPIO_Pin_1);
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
}

/**
  * @��  ��  SW1���ѡ��״̬
  * @��  ��  ��	  
  * @����ֵ  ���뿪��״̬
  *			 1 - ���ز�����ON��
  *			 0 - ���ز�����ON�Բ�
  */
uint8_t AX_SW_GetS1(void)
{
   	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0))
	return 0;
	else
	return 1;
}

/**
  * @��  ��  SW2���ѡ��״̬
  * @��  ��  ��	  
  * @����ֵ  ���뿪��״̬
  *			 1 - ���ز�����ON��
  *			 0 - ���ز�����ON�Բ�
  */
uint8_t AX_SW_GetS2(void)
{
   	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1))
	return 0;
	else
	return 1;
}


/**
  * @��  ��  SW���ѡ��״̬
  * @˵  ��	 ���ز�����ON��Ϊ1���Բ�Ϊ0��
  * @��  ��  ��	  
  * @����ֵ  ����״̬
  *			SW1(PA8)  SW2(PE10)    ����ֵ
  *			 0         0             0  
  *          0         1             1  
  *          1         0             2
  *          1         1             3   
  */
uint8_t AX_SW_GetS12(void)
{
    uint8_t sw = 0;

	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_1)) 	
	{sw <<= 1;}
	else
	{sw=sw+1; sw<<=1;}

	if(GPIO_ReadInputDataBit(GPIOB,GPIO_Pin_0))
	{;}
	else
	{sw=sw+1;}

	return sw;
}

/******************* (C) ��Ȩ 2023 XTARK **************************************/
