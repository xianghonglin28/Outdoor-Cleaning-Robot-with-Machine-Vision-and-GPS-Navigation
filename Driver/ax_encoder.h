#ifndef __AX_ENCODER_H
#define __AX_ENCODER_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//X-SOFT �ӿں���
void AX_ENCODER_A_Init(void);          //��������ʼ��
uint16_t AX_ENCODER_A_GetCounter(void);          //��������ȡ��������ֵ
void AX_ENCODER_A_SetCounter(uint16_t count);    //���������ü�������ֵ

void AX_ENCODER_B_Init(void);          //��������ʼ��
uint16_t AX_ENCODER_B_GetCounter(void);          //��������ȡ��������ֵ
void AX_ENCODER_B_SetCounter(uint16_t count);    //���������ü�������ֵ

void AX_ENCODER_C_Init(void);          //��������ʼ��
uint16_t AX_ENCODER_C_GetCounter(void);          //��������ȡ��������ֵ
void AX_ENCODER_C_SetCounter(uint16_t count);    //���������ü�������ֵ

void AX_ENCODER_D_Init(void);          //��������ʼ��
uint16_t AX_ENCODER_D_GetCounter(void);          //��������ȡ��������ֵ
void AX_ENCODER_D_SetCounter(uint16_t count);    //���������ü�������ֵ

#endif
