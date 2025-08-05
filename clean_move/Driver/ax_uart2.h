#ifndef __AX_UART2_H
#define __AX_UART2_H

#include "stm32f4xx.h"
#include "ax_servo.h"

// 机械臂状态结构体
typedef struct {
    float angles[6];      // 6个关节角度
    uint8_t emergency;    // 急停状态
    uint8_t mode;         // 操作模式
} ArmState;

// 函数声明
void UART2_Init(void);
void UART2_SendString(const char *str);
void ProcessUARTCommand(const char *cmd);
void UpdateArmPosition(void);
void EmergencyStop(uint8_t state);

extern ArmState arm_state;

#endif 
