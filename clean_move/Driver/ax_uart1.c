#include "ax_uart1.h"
#include <stdio.h>

static uint8_t uart1_rx_ok = 0; //接收成功标志
static uint8_t uart1_rx_buf[40];     //接收缓冲，数据内容小于等于32Byte
static uint8_t uart1_tx_buf[40];     //发送缓冲
extern void ProcessUARTCommand(const char* cmd);

/**
  * @简  述  UART 串口初始化
  * @参  数  baud： 波特率设置
  * @返回值	 无
  */
void AX_UART1_Init(uint32_t baud)
{
    // 初始化USART1（连接PC的串口调试工具）
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
    
    printf("UART1 Init OK\n");  // 测试串口1输出
}

/**
  * @简  述  DBUART 串口中断服务函数
  * @参  数  无 
  * @返回值  无
  */
void USART1_IRQHandler(void) {
    if (USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) {
        static uint8_t rx_buf[32];
        static uint8_t rx_idx = 0;
        uint8_t c = USART_ReceiveData(USART1);
        
        if (c == '\n') {  // 检测换行符
            rx_buf[rx_idx] = '\0';
            printf("Received: %s\n", rx_buf);  // 打印原始数据
            ProcessUARTCommand((const char*)rx_buf);  // 处理指令
            rx_idx = 0;
        } else if (rx_idx < sizeof(rx_buf) - 1) {
            rx_buf[rx_idx++] = c;
        }
    }
}


/**************************串口打印相关函数重定义********************************/
/**
  * @简  述  重定义putc函数（USART3）	
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
  * @简  述  重定义getc函数（USART3）	
  */
int fgetc(FILE *f)
{
	/* 等待串口1输入数据 */
	while (USART_GetFlagStatus(USART3, USART_FLAG_RXNE) == RESET)
	{}

	return (int)USART_ReceiveData(USART3);
}
