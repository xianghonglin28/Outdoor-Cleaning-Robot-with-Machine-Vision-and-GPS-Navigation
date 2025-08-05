#ifndef __AX_UART3_H
#define __AX_UART3_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//X-SOFT �ӿں���
void AX_UART3_Init(uint32_t baud);  //UART ���Դ��ڳ�ʼ��
uint8_t AX_UART3_GetData(uint8_t *pbuf);  //UART ��ȡ���յ�����
void    AX_UART3_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num);  //UART �������ݣ�X-ProtocolЭ�飩



void USART3_SendChar(char ch);
void USART3_SendString(char *str);

#endif 

