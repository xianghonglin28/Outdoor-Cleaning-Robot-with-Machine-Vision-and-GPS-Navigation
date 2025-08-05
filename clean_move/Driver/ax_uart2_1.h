/**			                                                    
		   ____                    _____ _______ _____       XTARK@塔克创新
		  / __ \                  / ____|__   __|  __ \ 
		 | |  | |_ __   ___ _ __ | |       | |  | |__) |
		 | |  | | '_ \ / _ \ '_ \| |       | |  |  _  / 
		 | |__| | |_) |  __/ | | | |____   | |  | | \ \ 
		  \____/| .__/ \___|_| |_|\_____|  |_|  |_|  \_\
				| |                                     
				|_|                OpenCTR   机器人控制器
									 
  ****************************************************************************** 
  *           
  * 版权所有： XTARK@塔克创新  版权所有，盗版必究
  * 公司网站： www.xtark.cn   www.tarkbot.com
  * 淘宝店铺： https://xtark.taobao.com  
  * 塔克微信： 塔克创新（关注公众号，获取最新更新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @内  容  云端eps8266通信
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
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

//#define MAX_SAFE_ANGLE      90.0f   // 舵机最大安全角度
//#define MIN_SAFE_ANGLE     -90.0f   // 最小安全角度
//#define MAX_STEP           5.0f     // 单步最大角度变化量
//#define OVERLOAD_THRESHOLD 800      // 舵机过载电流阈值(mA)

#endif 

/******************* (C) 版权 2023 XTARK **************************************/
