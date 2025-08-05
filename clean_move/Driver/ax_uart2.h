#ifndef __AX_UART2_H
#define __AX_UART2_H

#include "stm32f4xx.h"
#include "ax_servo.h"

// ��е��״̬�ṹ��
typedef struct {
    float angles[6];      // 6���ؽڽǶ�
    uint8_t emergency;    // ��ͣ״̬
    uint8_t mode;         // ����ģʽ
} ArmState;

// ��������
void UART2_Init(void);
void UART2_SendString(const char *str);
void ProcessUARTCommand(const char *cmd);
void UpdateArmPosition(void);
void EmergencyStop(uint8_t state);

extern ArmState arm_state;

#endif 
