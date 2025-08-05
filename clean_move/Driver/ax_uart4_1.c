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
  * @��  ��  ����APP����ͨ��
  *
  ******************************************************************************
  * @˵  ��
  *
  * 1.USB����ͨ�ţ�printf�����Ѷ��򵽸ô��ڣ��������������Ϣ
  * 2.����ʹ�þ���X-ProtocolЭ��������ݷ��͡�
  * 3.����UART�Ĵ������ݽ��չ��ܣ�ʹ���жϷ�ʽ��X-ProtocolЭ��ͨ��
  * 4.��ͨ��AX_UART_GetRxData()�����ж��Ƿ������ݽ���
  *
  * X-ProtocolЭ����ܣ���֡����
  * ֡���壺AA 55 | 0B  | 01  | 03 E8 FC 18 00 0A | 14
  *        ֡ͷ   ֡��   ֡��  ����                У���
  * ֡  ͷ��˫֡ͷ��������ǿ
  * ֡  �����������ݳ����趨
  * ֡  �룺�û����ݹ����趨����ʶ֡��Ψһ��
  * ��  �ݣ���λ��ǰ�����ȿɱ䣬�����������8λ��16λ��32λ����
  * У��ͣ�ǰ�������ۼӺ͵ĵ�8λ
  * ֡ʾ����( AA 55 0B 01 03 E8 FC 18 00 0A 14 ) ���ݣ�1000��-1000,10,
  * 
  ******************************************************************************
  */

#include "ax_uart4.h"
#include <stdio.h>

#include "ax_robot.h"

static uint8_t uart4_rx_ok = 0; //���ճɹ���־
static uint8_t uart4_rx_con = 0;       //���ռ�����
static uint8_t uart4_rx_checksum;    //֡ͷ����У���
static uint8_t uart4_rx_buf[40];     //���ջ��壬��������С�ڵ���32Byte
static uint8_t uart4_tx_buf[40];     //���ͻ���

/**
  * @��  ��  UART   ���ڳ�ʼ��
  * @��  ��  baud�� ����������
  * @����ֵ	 ��
  */
void AX_UART4_Init(uint32_t baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;


	/* ����USART���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_UART4,ENABLE);
	
	//USART��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource10,GPIO_AF_UART4);
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource11,GPIO_AF_UART4); 

	//USART �˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOC,&GPIO_InitStructure); 

	//USART��������
	USART_InitStructure.USART_BaudRate = baud;    //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(UART4, &USART_InitStructure);

	//USARTʹ��
	USART_Cmd(UART4, ENABLE); 
	
	//�������ڽ����ж�
	USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);//��������ж�
	
    //USART3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = UART4_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���	
	
}

/**
  * @��  ��  UART �����жϷ�����
  * @��  ��  �� 
  * @����ֵ  ��
  */
void UART4_IRQHandler(void)
{
	uint8_t Res;
	
	if(USART_GetITStatus(UART4, USART_IT_RXNE) != RESET)  //�����ж�
	{
		Res =USART_ReceiveData(UART4);	
		
		if(uart4_rx_con < 3)    //==����֡ͷ + ����
		{
			if(uart4_rx_con == 0)  //����֡ͷ1 0xAA
			{
				if(Res == 0xAA)
				{
					uart4_rx_buf[0] = Res;
					uart4_rx_con = 1;					
				}
				else
				{
					
				}
			}else if(uart4_rx_con == 1) //����֡ͷ2 0x55
			{
				if(Res == 0x55)
				{
					uart4_rx_buf[1] = Res;
					uart4_rx_con = 2;
				}
				else
				{
					uart4_rx_con = 0;						
				}				
			}
			else  //�������ݳ���
			{
				uart4_rx_buf[2] = Res;
				uart4_rx_con = 3;
				uart4_rx_checksum = (0xAA+0x55) + Res;	//����У���
			}
		}
		else    //==��������
		{
			if(uart4_rx_con < (uart4_rx_buf[2]-1) )
			{
				uart4_rx_buf[uart4_rx_con] = Res;
				uart4_rx_con++;
				uart4_rx_checksum = uart4_rx_checksum + Res;					
			}
			else    //�ж����1λ
			{
				//������ɣ��ָ���ʼ״̬
				uart4_rx_con = 0;	
				
				//����У��
				if( Res == uart4_rx_checksum )  //У����ȷ
				{	

					//���ݽ���OK
					uart4_rx_ok = 1;	
					
					//�ֻ�APP�����������
					if(uart4_rx_buf[3] == ID_BLERX_CM)
					{
						if(uart4_rx_buf[4] == 0x55)
						{
							//�л����ֻ�APP����
							ax_control_mode = CTL_APP;
							
							//ִ�з�����������ʾ
							ax_beep_ring = BEEP_SHORT;
						}
					}
					
					//printf("ID = %d \r\n",uart4_rx_buf[4],uart4_rx_buf[4],uart4_rx_buf[4]);
				}
			}
		}
		
    USART_ITConfig(UART4, USART_IT_RXNE, ENABLE);
	} 
}


/**
  * @��  ��  ��ȡ���յ�����
  * @��  ��  *pbuf����������ָ��,��1���ֽ�Ϊ֡���룬����Ϊ����
  * @����ֵ	 0-�����ݽ��գ�other-��Ҫ��ȡ�������ֽڸ���
  */
uint8_t AX_UART4_GetData(uint8_t *pbuf)
{
	uint8_t cnt,i;
	
	if(uart4_rx_ok != 0)
	{
		cnt = uart4_rx_buf[2]-4;
		
		for(i=0; i<cnt; i++)
		{
			*(pbuf+i) = uart4_rx_buf[3+i];
		}
		
		uart4_rx_ok = 0;
		return cnt;
	}
	else
	{
		return 0;
	}	
}

/**
  * @��  ��  UART �������ݣ�X-ProtocolЭ�飩
  * @��  ��  *pbuf����������ָ��
  *          len���������ݳ��ȸ�������27 (32-5)
  *          num��֡�ţ�֡����
  * @����ֵ	 ��
  */
void AX_UART4_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num)
{
	uint8_t i,cnt;	
    uint8_t tx_checksum = 0;//����У���
	
	if(len <= 32)
	{
		/******��ȡ����******/
		uart4_tx_buf[0] = 0xAA;    //֡ͷ
		uart4_tx_buf[1] = 0x55;    //
		uart4_tx_buf[2] = len+5;  //����������ȼ���֡����
		uart4_tx_buf[3] = num;    //֡����
		
		for(i=0; i<len; i++)
		{
			uart4_tx_buf[4+i] = *(pbuf+i);
		}
		
		/******����У���******/	
		cnt = 4+len;
		for(i=0; i<cnt; i++)
		{
			tx_checksum = tx_checksum + uart4_tx_buf[i];
		}
		uart4_tx_buf[i] = tx_checksum;
		
		
		/******��������******/	
		cnt = 5+len;
		
		//��ѯ���䷽ʽ
		for(i=0; i<cnt; i++)
		{
			USART_SendData(UART4, uart4_tx_buf[i]);
			while(USART_GetFlagStatus(UART4,USART_FLAG_TC) != SET);
		}
	}
}



/******************* (C) ��Ȩ 2023 XTARK **************************************/
