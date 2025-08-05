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
  * @��  ��  USBͨ�Ŵ���ͨ��
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

#include "ax_uart3.h"
#include <stdio.h>

#include "ax_robot.h"

static uint8_t uart3_rx_con=0;       //���ռ�����
static uint8_t uart3_rx_checksum;    //֡ͷ����У���
static uint8_t uart3_rx_buf[40];     //���ջ��壬��������С�ڵ���32Byte
static uint8_t uart3_tx_buf[40];     //���ͻ���

/**
  * @��  ��  UART   ���ڳ�ʼ��
  * @��  ��  baud�� ����������
  * @����ֵ	 ��
  */
void AX_UART3_Init(uint32_t baud)
{

	GPIO_InitTypeDef GPIO_InitStructure;
    USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef  NVIC_InitStructure;


	/* ����USART���� */
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD,ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3,ENABLE);
	
	//USART��Ӧ���Ÿ���ӳ��
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource8,GPIO_AF_USART3);
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource9,GPIO_AF_USART3); 

	//USART �˿�����
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;	
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; 
	GPIO_Init(GPIOD,&GPIO_InitStructure); 

	//USART��������
	USART_InitStructure.USART_BaudRate = baud;    //������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART3, &USART_InitStructure);


	//USARTʹ��
	USART_Cmd(USART3, ENABLE); 
	
	
	//�������ڽ����ж�
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);//��������ж�
	
    //USART3 NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;//����1�ж�ͨ��
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
void USART3_IRQHandler(void)
{
//	uint8_t Res;
	
	if(USART_GetITStatus(USART3, USART_IT_RXNE) != RESET)  //�����ж�
	{
		  uint8_t data;
		  data = USART_ReceiveData(USART3);
		  servo_flag = data;
//		Res =USART_ReceiveData(USART3);	
		
//		if(uart3_rx_con < 3)    //==����֡ͷ + ����
//		{
//			if(uart3_rx_con == 0)  //����֡ͷ1 0xAA
//			{
//				if(Res == 0xAA)
//				{
//					uart3_rx_buf[0] = Res;
//					uart3_rx_con = 1;					
//				}
//				else
//				{
//					
//				}
//			}else if(uart3_rx_con == 1) //����֡ͷ2 0x55
//			{
//				if(Res == 0x55)
//				{
//					uart3_rx_buf[1] = Res;
//					uart3_rx_con = 2;
//				}
//				else
//				{
//					uart3_rx_con = 0;						
//				}				
//			}
//			else  //�������ݳ���
//			{
//				uart3_rx_buf[2] = Res;
//				uart3_rx_con = 3;
//				uart3_rx_checksum = (0xAA+0x55) + Res;	//����У���
//			}
//		}
//		else    //==��������
//		{
//			if(uart3_rx_con < (uart3_rx_buf[2]-1) )
//			{
//				uart3_rx_buf[uart3_rx_con] = Res;
//				uart3_rx_con++;
//				uart3_rx_checksum = uart3_rx_checksum + Res;					
//			}
//			else    //�ж����1λ
//			{
//				//������ɣ��ָ���ʼ״̬
//				uart3_rx_con = 0;	
//				
//				//����У��
//				if( Res == uart3_rx_checksum )  //У����ȷ
//				{	
//					
//					//����ΪROS����ģʽ
//					ax_control_mode = CTL_ROS;
//					
//					//�ٶȿ���֡
//					if(uart3_rx_buf[3] == ID_URX_VEL)
//					{
//						R_Vel.TG_IX = (int16_t)((uart3_rx_buf[4]<<8) | uart3_rx_buf[5]);
//						R_Vel.TG_IY = (int16_t)((uart3_rx_buf[6]<<8) | uart3_rx_buf[7]);
//						R_Vel.TG_IW = (int16_t)((uart3_rx_buf[8]<<8) | uart3_rx_buf[9]);
//						
//						//����ǰ����������ˣ�����ʸ���ٶȼ���ǰ��ת���
//						#if (ROBOT_TYPE == ROBOT_AKM)
//							ax_akm_angle = AX_AKM_WToAngle(R_Vel.TG_IX, R_Vel.TG_IW);
//						#endif
//					}
//					else
//					{
//						//IMU������У׼
//						if(uart3_rx_buf[3] == ID_URX_IMU)
//						{
//							ax_imu_calibrate_flag = uart3_rx_buf[4];
//						}	

//						//RGB��Ч����֡
//						if(uart3_rx_buf[3] == ID_URX_LG)
//						{
//							R_Light.M  = uart3_rx_buf[4];
//							R_Light.S  = uart3_rx_buf[5];
//							R_Light.T  = uart3_rx_buf[6];
//							R_Light.R  = uart3_rx_buf[7];
//							R_Light.G  = uart3_rx_buf[8];
//							R_Light.B  = uart3_rx_buf[9];
//						}

//						//��ЧEEPROM����
//						if(uart3_rx_buf[3] == ID_URX_LS)
//						{
//							//ִ�еƹ�Ч�����涯��
//							ax_light_save_flag = 1;
//						}	

//						//���������п���
//						if(uart3_rx_buf[3] == ID_URX_BP)
//						{
//							//����Ϊ1������������
//							if(uart3_rx_buf[4] != 0)
//							{
//								AX_BEEP_On();
//							}
//							else
//							{
//								AX_BEEP_Off();
//							}
//						}							

//						//�����˵����ͺ���Ϣ�������ж��Ƿ�һ��
//						if(uart3_rx_buf[3] == ID_URX_RTY)
//						{
//							//�ж��뵱ǰ�����Ƿ�һ�£�
//							if(uart3_rx_buf[4] == ROBOT_TYPE  )
//							{
//								//һ�£����������������ʾ
//								ax_beep_ring = BEEP_SHORT;
//							}
//							else
//							{
//								//��һ�£�������������б���
//								ax_beep_ring = BEEP_LONG;								
//							}
//						}	
//					}	
//				}
//			}
//		}
//		
//		USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);
//	} 
}
	}

/**
  * @��  ��  UART �������ݣ�X-ProtocolЭ�飩
  * @��  ��  *pbuf����������ָ��
  *          len���������ݳ��ȸ�������27 (32-5)
  *          num��֡�ţ�֡����
  * @����ֵ	 ��
  */
void AX_UART3_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num)
{
	uint8_t i,cnt;	
  uint8_t tx_checksum = 0;//����У���
	
	if(len <= 32)
	{
		/******��ȡ����******/
		uart3_tx_buf[0] = 0xAA;    //֡ͷ
		uart3_tx_buf[1] = 0x55;    //
		uart3_tx_buf[2] = len+5;  //����������ȼ���֡����
		uart3_tx_buf[3] = num;    //֡����
		
		for(i=0; i<len; i++)
		{
			uart3_tx_buf[4+i] = *(pbuf+i);
		}
		
		/******����У���******/	
		cnt = 4+len;
		for(i=0; i<cnt; i++)
		{
			tx_checksum = tx_checksum + uart3_tx_buf[i];
		}
		uart3_tx_buf[i] = tx_checksum;
		
		
		/******��������******/	
		cnt = 5+len;
		
		//��ѯ���䷽ʽ
		for(i=0; i<cnt; i++)
		{
			USART_SendData(USART1, uart3_tx_buf[i]);
			while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
		}
	}
}


/* USART3�����ַ� */
void USART3_SendChar(char ch)
{
    while (USART_GetFlagStatus(USART3, USART_FLAG_TXE) == RESET);
    USART_SendData(USART3, (uint16_t)ch);
}

/* USART3�����ַ��� */
void USART3_SendString(char *str)
{
    while (*str)
    {
        USART3_SendChar(*str++);
    }
}


/******************* (C) ��Ȩ 2023 XTARK **************************************/
