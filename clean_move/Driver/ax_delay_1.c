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
  * @��  ��  �����ʱ����
  ******************************************************************************
  * @˵  ��
  *
  * 1.��ʱ����ʹ�õδ�ʱ��ʵ��
  * 
  ******************************************************************************
  */

#include "ax_delay.h"

#include "FreeRTOS.h"	//FreeRTOSʹ��		  
static u8  fac_us=0;	//us��ʱ������	

/**
  * @��  ��  ��ʱ������ʼ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_DELAY_Init(void) 
{	 

	u32 reload;
	
	//��ʱ����SysTick����
	SysTick_CLKSourceConfig(SysTick_CLKSource_HCLK); 	//ʱ�ӹ̶�ΪAHBʱ��
	
	fac_us=SystemCoreClock/1000000;				//�����Ƿ�ʹ��OS,fac_us����Ҫʹ��
	reload=SystemCoreClock/1000000;				//ÿ���ӵļ������� ��λΪM
	
	reload*=1000000/configTICK_RATE_HZ;			//����configTICK_RATE_HZ�趨���ʱ��
											    //reloadΪ24λ�Ĵ���,���ֵ:16777216,��168M��,Լ��0.0998s����	
	
	SysTick->CTRL|=SysTick_CTRL_TICKINT_Msk;    //����SYSTICK�ж�
	SysTick->LOAD=reload;                   //ÿ1/configTICK_RATE_HZ���ж�һ�� 	
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk; //����SYSTICK   	

}


/**
  * @��  ��  ���΢����ʱ
  * @��  ��  us����ʱ���ȣ���λus	  
  * @����ֵ  ��
  */
void AX_Delayus(uint32_t us)
{
	u32 ticks;
	u32 told,tnow,tcnt=0;
	u32 reload=SysTick->LOAD;				//LOAD��ֵ	 
	
	ticks=us*fac_us; 						  //��Ҫ�Ľ����� 
	told=SysTick->VAL;        		//�ս���ʱ�ļ�����ֵ
	
	while(1)
	{
		tnow=SysTick->VAL;	
		if(tnow!=told)
		{	    
			if(tnow<told)
				tcnt+=told-tnow;	 //����ע��һ��SYSTICK��һ���ݼ��ļ������Ϳ�����.
			else 
				tcnt+=reload-tnow+told;	  
			
			told=tnow;
			
			if(tcnt>=ticks)
				break;			    //ʱ�䳬��/����Ҫ�ӳٵ�ʱ��,���˳�.
		}  
	}										       				
}


/**
  * @��  ��  ���ms��ʱ,ϵͳ���������󣬲�Ҫʹ�øú���
  * @��  ��  ms����ʱ���ȣ���λms	  
  * @����ֵ  ��
  */
void AX_Delayms(uint16_t ms)
{
	AX_Delayus((uint32_t)(ms*1000));
	
}


/******************* (C) ��Ȩ 2023 XTARK **************************************/
