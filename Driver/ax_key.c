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
