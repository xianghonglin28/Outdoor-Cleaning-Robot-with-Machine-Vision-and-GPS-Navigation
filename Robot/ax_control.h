#ifndef __AX_CONTROL_H
#define __AX_CONTROL_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//机器人各种控制方式处理文件
//void AX_CTL_Ps2(void);     //PS2手柄控制
void AX_CTL_App(void);     //APP控制
void AX_CTL_RemoteSbus(void);  //SBUS航模遥控器控制

#endif
