#ifndef __AX_UART5_H
#define __AX_UART5_H
#include "ax_sys.h"

#define UART5_MAX_RECV_LEN		400					//最大接收缓存字节数
#define UART5_MAX_SEND_LEN		400					//最大发送缓存字节数
#define UART5_RX_EN 			1					//0,不接收;1,接收.

extern u8  UART5_RX_BUF[UART5_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  UART5_TX_BUF[UART5_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern u16 UART5_RX_STA;   						//接收数据状态

void uart5_init(u32 bound);				//串口3初始化 
void TIM7_Int_Init(u16 arr,u16 psc);
void u5_printf(char* fmt, ...);







#endif

