#ifndef __AX_EEPROM_H
#define __AX_EEPROM_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//OpenCTR接口函数
void AX_EEPROM_Init(void);    //初始化
uint8_t AX_EEPROM_ReadOneByte(uint16_t addr);  //读取一个字节
void AX_EEPROM_WriteOneByte(uint16_t addr,uint8_t data);  //写一个字节
void AX_EEPROM_Read(uint16_t addr, uint8_t *buf, uint16_t num);  //读取数据
void AX_EEPROM_Write(uint16_t addr, uint8_t *buf, uint16_t num);  //写入数据


#endif

