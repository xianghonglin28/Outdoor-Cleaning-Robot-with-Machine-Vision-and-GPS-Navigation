#ifndef __AX_EEPROM_H
#define __AX_EEPROM_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//OpenCTR�ӿں���
void AX_EEPROM_Init(void);    //��ʼ��
uint8_t AX_EEPROM_ReadOneByte(uint16_t addr);  //��ȡһ���ֽ�
void AX_EEPROM_WriteOneByte(uint16_t addr,uint8_t data);  //дһ���ֽ�
void AX_EEPROM_Read(uint16_t addr, uint8_t *buf, uint16_t num);  //��ȡ����
void AX_EEPROM_Write(uint16_t addr, uint8_t *buf, uint16_t num);  //д������


#endif

