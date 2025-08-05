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
  * @��  ��  VIN�����ѹ���
  *
  ******************************************************************************
  * @˵  ��
  *
  * 1.ʹ��ADC2��VIN�����ѹ��������ѯ��ʽ
	* 2.�����ѹ��⹦�ܣ���ʵ�ֶԵ�ص���������
  * 
  ******************************************************************************
  */

#include "ax_vin.h"

#define ADC_REVISE  99.6

/**
  * @��  ��  VIN �����ѹ����ʼ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_VIN_Init(void)
{
	
	GPIO_InitTypeDef GPIO_InitStructure;
	ADC_CommonInitTypeDef ADC_CommonInitStructure;
	ADC_InitTypeDef ADC_InitStructure;

    //ʹ��ADC2��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC2, ENABLE); 

    //����PB1Ϊģ������(ADC Channel10) 
    GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AN;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOC, &GPIO_InitStructure);

	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,ENABLE);	 
	RCC_APB2PeriphResetCmd(RCC_APB2Periph_ADC2,DISABLE);
	
    ADC_CommonInitStructure.ADC_Mode = ADC_Mode_Independent;
    ADC_CommonInitStructure.ADC_TwoSamplingDelay = ADC_TwoSamplingDelay_5Cycles;
    ADC_CommonInitStructure.ADC_DMAAccessMode = ADC_DMAAccessMode_Disabled;
    ADC_CommonInitStructure.ADC_Prescaler = ADC_Prescaler_Div4;
    ADC_CommonInit(&ADC_CommonInitStructure);
	
    //����ADC2
    ADC_InitStructure.ADC_Resolution = ADC_Resolution_12b;
    ADC_InitStructure.ADC_ScanConvMode = DISABLE;	
    ADC_InitStructure.ADC_ContinuousConvMode = DISABLE;
    ADC_InitStructure.ADC_ExternalTrigConvEdge = ADC_ExternalTrigConvEdge_None;
    ADC_InitStructure.ADC_DataAlign = ADC_DataAlign_Right;	
    ADC_InitStructure.ADC_NbrOfConversion = 1; 
    ADC_Init(ADC2, &ADC_InitStructure);

    //ʹ��ADC2
    ADC_Cmd(ADC2, ENABLE);
	
	//ADC2 ����ͨ������ 
    ADC_RegularChannelConfig(ADC2, ADC_Channel_10, 1, ADC_SampleTime_480Cycles);	
	
}

/**
  * @��  ��  VIN ��������ѹ
  * @��  ��  ��	  
  * @����ֵ  ��ѹֵ������100��������7.2V���Ϊ720��
  */
uint16_t AX_VIN_GetVol_X100(void)
{
    uint16_t Input_Vol,temp;
	
	ADC_SoftwareStartConv(ADC2);   //ʹ��ָ����ADC2��, ���ת���������� 
	
    while(!ADC_GetFlagStatus(ADC2, ADC_FLAG_EOC ));//�ȴ�ת������ 
	
	temp = ADC_GetConversionValue(ADC2);
	
	Input_Vol = (uint16_t)((3.3*11.0*ADC_REVISE * temp)/4095);	//��ѹ����1/11
	
	return Input_Vol;
	
}


/******************* (C) ��Ȩ 2023 XTARK **************************************/
