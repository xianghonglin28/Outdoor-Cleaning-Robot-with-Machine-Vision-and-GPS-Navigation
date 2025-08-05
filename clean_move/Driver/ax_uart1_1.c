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
  * @��  ��  ���Դ���ͨ��
  *
  ******************************************************************************
  * @˵  ��
  *
  * 1.USB����ͨ�ţ�printf�����Ѷ��򵽸ô��ڣ��������������Ϣ
  * 2.����ʹ�þ���X-ProtocolЭ��������ݷ��͡�
  * 3.����UART�Ĵ������ݽ��չ��ܣ�ʹ���жϷ�ʽ��X-ProtocolЭ��ͨ��
  * 4.��ͨ��AX_UART1_GetRxData()�����ж��Ƿ������ݽ���
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

#include "ax_uart1.h"
#include <stdio.h>

static uint8_t uart1_rx_ok = 0; //���ճɹ���־
//static uint8_t uart1_rx_con = 0;       //���ռ�����
//static uint8_t uart1_rx_checksum;    //֡ͷ����У���
static uint8_t uart1_rx_buf[40];     //���ջ��壬��������С�ڵ���32Byte
static uint8_t uart1_tx_buf[40];     //���ͻ���
// ����ⲿ��������������ú�������һ���ļ��ж��壩
extern void ProcessUARTCommand(const char* cmd);

/**
  * @��  ��  UART ���ڳ�ʼ��
  * @��  ��  baud�� ����������
  * @����ֵ	 ��
  */
void AX_UART1_Init(uint32_t baud)
{
    // ��ʼ��USART1������PC�Ĵ��ڵ��Թ��ߣ�
    USART_InitTypeDef USART_InitStruct;
    GPIO_InitTypeDef GPIO_InitStruct;

    RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);

    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);

    GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);

    USART_InitStruct.USART_BaudRate = baud;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART1, &USART_InitStruct);

    USART_Cmd(USART1, ENABLE);
		
		// �ж�����
   // USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
    //NVIC_EnableIRQ(USART1_IRQn);
    
    printf("UART1 Init OK\n");  // ���Դ���1���
}

/**
  * @��  ��  DBUART �����жϷ�����
  * @��  ��  �� 
  * @����ֵ  ��
  */
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        static uint8_t rx_buf[32];
        static uint8_t rx_idx = 0;
        uint8_t c = USART_ReceiveData(USART1);
        
        if (c == '\n') {  // ��⻻�з�
            rx_buf[rx_idx] = '\0';
            printf("Received: %s\n", rx_buf);  // ��ӡԭʼ����
            ProcessUARTCommand((const char*)rx_buf);  // ����ָ��
            rx_idx = 0;
        } else if (rx_idx < sizeof(rx_buf) - 1) {
            rx_buf[rx_idx++] = c;
        }
    }
}

//void USART1_IRQHandler(void)
//{
//	uint8_t Res;
//	
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET)  //�����ж�
//	{
//		Res =USART_ReceiveData(USART1);	
//		
//		if(uart1_rx_con < 3)    //==����֡ͷ + ����
//		{
//			if(uart1_rx_con == 0)  //����֡ͷ1 0xAA
//			{
//				if(Res == 0xAA)
//				{
//					uart1_rx_buf[0] = Res;
//					uart1_rx_con = 1;					
//				}
//				else
//				{
//					
//				}
//			}else if(uart1_rx_con == 1) //����֡ͷ2 0x55
//			{
//				if(Res == 0x55)
//				{
//					uart1_rx_buf[1] = Res;
//					uart1_rx_con = 2;
//				}
//				else
//				{
//					uart1_rx_con = 0;						
//				}				
//			}
//			else  //�������ݳ���
//			{
//				uart1_rx_buf[2] = Res;
//				uart1_rx_con = 3;
//				uart1_rx_checksum = (0xAA+0x55) + Res;	//����У���
//			}
//		}
//		else    //==��������
//		{
//			if(uart1_rx_con < (uart1_rx_buf[2]-1) )
//			{
//				uart1_rx_buf[uart1_rx_con] = Res;
//				uart1_rx_con++;
//				uart1_rx_checksum = uart1_rx_checksum + Res;					
//			}
//			else    //�ж����1λ
//			{
//				//������ɣ��ָ���ʼ״̬
//				uart1_rx_con = 0;	
//				
//				//����У��
//				if( Res == uart1_rx_checksum )  //У����ȷ
//				{	
//					//�˴��������ݽ���
//					uart1_rx_ok = 1;	
//				}
//			}
//		}
//		
//    USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
//	} 
//}

///**
//  * @��  ��  ��ȡ���յ�����
//  * @��  ��  *pbuf����������ָ��,��1���ֽ�Ϊ֡���룬����Ϊ����
//  * @����ֵ	 0-�����ݽ��գ�other-��Ҫ��ȡ�������ֽڸ���
//  */
//uint8_t AX_UART1_GetData(uint8_t *pbuf)
//{
//	uint8_t cnt,i;
//	
//	if(uart1_rx_ok != 0)
//	{
//		cnt = uart1_rx_buf[2]-4;
//		
//		for(i=0; i<cnt; i++)
//		{
//			*(pbuf+i) = uart1_rx_buf[3+i];
//		}
//		
//		uart1_rx_ok = 0;
//		return cnt;
//	}
//	else
//	{
//		return 0;
//	}	
//}

///**
//  * @��  ��  UART �������ݣ�X-ProtocolЭ�飩
//  * @��  ��  *pbuf����������ָ��
//  *          len���������ݳ��ȸ�������27 (32-5)
//  *          num��֡�ţ�֡����
//  * @����ֵ	 ��
//  */
//void AX_UART1_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num)
//{
//	uint8_t i,cnt;	
//  uint8_t tx_checksum = 0;//����У���
//	
//	if(len <= 32)
//	{
//		/******��ȡ����******/
//		uart1_tx_buf[0] = 0xAA;    //֡ͷ
//		uart1_tx_buf[1] = 0x55;    //
//		uart1_tx_buf[2] = len+5;  //����������ȼ���֡����
//		uart1_tx_buf[3] = num;    //֡����
//		
//		for(i=0; i<len; i++)
//		{
//			uart1_tx_buf[4+i] = *(pbuf+i);
//		}
//		
//		/******����У���******/	
//		cnt = 4+len;
//		for(i=0; i<cnt; i++)
//		{
//			tx_checksum = tx_checksum + uart1_tx_buf[i];
//		}
//		uart1_tx_buf[i] = tx_checksum;
//		
//		
//		/******��������******/	
//		cnt = 5+len;
//		
//		//��ѯ���䷽ʽ
//		for(i=0; i<cnt; i++)
//		{
//			USART_SendData(USART1, uart1_tx_buf[i]);
//			while(USART_GetFlagStatus(USART1,USART_FLAG_TC) != SET);
//		}
//	}
//}








/**************************���ڴ�ӡ��غ����ض���********************************/
/**
  * @��  ��  �ض���putc������USART3��	
  */
int fputc(int ch, FILE *f)
{
	/* Place your implementation of fputc here */
	/* e.g. write a character to the USART */
	USART_SendData(USART3, (uint8_t) ch);

	/* Loop until the end of transmission */
	while (USART_GetFlagStatus(USART3, USART_FLAG_TC) == RESET)
	{}

	return ch;
}

/**
  * @��  ��  �ض���getc������USART3��	
  */
int fgetc(FILE *f)
{
	/* �ȴ�����1�������� */
	while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
	{}

	return (int)USART_ReceiveData(USART3);
}

/******************* (C) ��Ȩ 2023 XTARK **************************************/
