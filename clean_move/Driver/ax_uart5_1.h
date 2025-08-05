#ifndef __AX_UART5_H
#define __AX_UART5_H
#include "ax_sys.h"

#define UART5_MAX_RECV_LEN		400					//�����ջ����ֽ���
#define UART5_MAX_SEND_LEN		400					//����ͻ����ֽ���
#define UART5_RX_EN 			1					//0,������;1,����.

extern u8  UART5_RX_BUF[UART5_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  UART5_TX_BUF[UART5_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern u16 UART5_RX_STA;   						//��������״̬

void uart5_init(u32 bound);				//����3��ʼ�� 
void TIM7_Int_Init(u16 arr,u16 psc);
void u5_printf(char* fmt, ...);







#endif

