#ifndef __AX_CAN_H
#define __AX_CAN_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//�ӿں���
void AX_CAN_Init(void);  //CAN��ʼ��
uint8_t AX_CAN_SendPacket(uint32_t id, uint32_t len, uint8_t *pbuf); //CAN��������

#endif

