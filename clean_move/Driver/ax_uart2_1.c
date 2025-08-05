#include "stm32f4xx.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "ax_uart2.h"
#include "ax_servo.h"
#include "ax_openmv.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include <math.h>


#define UART_BUF_SIZE 128

ArmState arm_state = {50.0, -20.0, -90.0, 45.0, 65.0, -45.0};;
char uart_rx_buf[UART_BUF_SIZE];
uint8_t uart_rx_idx = 0;

// UART2��ʼ��
void UART2_Init(void) {
    GPIO_InitTypeDef GPIO_InitStruct;
    USART_InitTypeDef USART_InitStruct;
    NVIC_InitTypeDef NVIC_InitStruct;
    
    // 1. ʹ��ʱ��
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
    
    // 2. ����GPIO
    GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_InitStruct.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStruct.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_Init(GPIOA, &GPIO_InitStruct);
    
    // 3. ���Ÿ���
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_USART2);
    GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_USART2);
    
    // 4. ����USART
    USART_InitStruct.USART_BaudRate = 115200;
    USART_InitStruct.USART_WordLength = USART_WordLength_8b;
    USART_InitStruct.USART_StopBits = USART_StopBits_1;
    USART_InitStruct.USART_Parity = USART_Parity_No;
    USART_InitStruct.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
    USART_InitStruct.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
    USART_Init(USART2, &USART_InitStruct);
    
    // 5. ʹ���ж�
    USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);
    
    // 6. ����NVIC
    NVIC_InitStruct.NVIC_IRQChannel = USART2_IRQn;
    NVIC_InitStruct.NVIC_IRQChannelPreemptionPriority = 5;  // �ʵ����ȼ�
    NVIC_InitStruct.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStruct.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStruct);
    
    // 7. ʹ��USART
    USART_Cmd(USART2, ENABLE);
}

// �����ַ���
void UART2_SendString(const char *str) {
    while(*str) {
        while(!(USART2->SR & USART_SR_TXE));
        USART_SendData(USART2, *str++);
    }
}

// ����UART����
void ProcessUARTCommand(const char *cmd) {
    // ��ӵ������
    printf("Received CMD: %s\n", cmd);
    
    // �ؽڿ��������ʽ: J[�ؽں�1-6]A[�Ƕ�ֵ]
    if(cmd[0] == 'J') {
        char *angle_ptr = strchr(cmd, 'A');
        if(angle_ptr) {
            int joint = atoi(cmd + 1);
            float angle = atof(angle_ptr + 1);
            
            printf("Parsed: Joint=%d, Angle=%.1f\n", joint, angle);
            
            if(joint >= 1 && joint <= 6) {
                taskENTER_CRITICAL();
                arm_state.angles[joint-1] = angle;
                taskEXIT_CRITICAL();
                
                // ����ȷ�Ϸ���
                char feedback[32];
                sprintf(feedback, "JA%.1f\n", angle);
                UART2_SendString(feedback);
                printf("Sent feedback: %s", feedback);
            }
        }
    }
    // ��ͣ����
    else if(strncmp(cmd, "ES", 2) == 0) {
        uint8_t state = atoi(cmd + 2);
        printf("Emergency Stop: %d\n", state);
        
        taskENTER_CRITICAL();
        EmergencyStop(state);
        taskEXIT_CRITICAL();
    }
}

// ���»�е��λ��
void UpdateArmPosition(void) {
    static float current_angles[6] = {0};;
    const float max_step = 5.0f; // ��󵥲��仯��
    //AX_SERVO_INIT();
		
    for(int i = 0; i < 6; i++) {
        // ƽ�����ɵ�Ŀ��Ƕ�
        if(current_angles[i] < arm_state.angles[i]) {
            current_angles[i] += fminf(max_step, arm_state.angles[i] - current_angles[i]);
        } else {
            current_angles[i] -= fminf(max_step, current_angles[i] - arm_state.angles[i]);
        }
        
        // ���ݲ�ͬ�Ķ�����ö�Ӧ�����ú���
        switch(i) {
            case 0: AX_SERVO_S1_SetAngle((int16_t)(current_angles[i] * 10)); break;
            case 1: AX_SERVO_S2_SetAngle((int16_t)(current_angles[i] * 10)); break;
            case 2: AX_SERVO_S3_SetAngle((int16_t)(current_angles[i] * 10)); break;
            case 3: AX_SERVO_S4_SetAngle((int16_t)(current_angles[i] * 10)); break;
            case 4: AX_SERVO_S5_SetAngle((int16_t)(current_angles[i] * 10)); break;
            case 5: AX_SERVO_S6_SetAngle((int16_t)(current_angles[i] * 10)); break;
        }
    }
}
// ��ͣ����
void EmergencyStop(uint8_t state)
{
    // ״̬�ޱ仯ʱֱ�ӷ���
    if(arm_state.emergency == state) return;
    
    // ����״̬
    arm_state.emergency = state;
    
    if(state) 
    {
        // �ͷ����ж��������ԭ���Բ�����
        __disable_irq(); // ���ݹر��ж�
        AX_SERVO_S1_Release();
        AX_SERVO_S2_Release();
        AX_SERVO_S3_Release();
        AX_SERVO_S4_Release();
        AX_SERVO_S5_Release();
        AX_SERVO_S6_Release();
        __enable_irq();
    }
    
    // ����״̬����
    char es_msg[16];
    sprintf(es_msg, "ES%d\n", state);
    UART2_SendString(es_msg);
}

// USART2�жϷ�����(�жϽ��պ���)
void USART2_IRQHandler(void) {
//    static uint8_t rx_buf[32];
//    static uint8_t rx_idx = 0;
    uint8_t c;
    if (USART_GetITStatus(USART2, USART_IT_RXNE) != RESET) {
        c = USART_ReceiveData(USART2);
			  OpenMV_Receive_Data(c);
			  

//        // �����������ӡÿ�����յ����ַ���ͨ������1��
//        printf("[UART2-RX] 0x%02X\n", c);  // ��ȷ������1�ѳ�ʼ��
//        
//        if (c == '\n') {  // ��⵽���з�����Ϊָ�����
//            rx_buf[rx_idx] = '\0';
//            printf("Received Full Command: %s\n", rx_buf);  // ͨ������1��ӡ����ָ��
//            ProcessUARTCommand((const char*)rx_buf);
//            rx_idx = 0;
//        } else if (rx_idx < sizeof(rx_buf) - 1) {
//            rx_buf[rx_idx++] = c;
//        } else {
//            // ���������������
//            rx_idx = 0;
//            printf("Error: UART2 Buffer Overflow!\n");
//        }
    }
		

}									


