#ifndef __AX_UART4_H
#define __AX_UART4_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"

//�ֱ�ģʽ��������
#define  APP_JOY1_Y         0x01    //Y����
#define  APP_JOY1_X         0x02    //X����
#define  APP_JOY1_B         0x04    //B���� 
#define  APP_JOY1_A         0x08    //A����
#define  APP_JOY1_RIGHT     0x10    //���� �Ұ���
#define  APP_JOY1_LEFT      0x20    //���� �󰴼�
#define  APP_JOY1_DOWN      0x40    //���� �°��� 
#define  APP_JOY1_UP        0x80    //���� �ϰ���

#define  APP_JOY2_K4         0x01    //K1����
#define  APP_JOY2_K3         0x02    //K2����
#define  APP_JOY2_K2         0x04    //K3���� 
#define  APP_JOY2_K1         0x08    //K4����
#define  APP_JOY2_R2         0x10    //R2����
#define  APP_JOY2_R1         0x20    //R1����
#define  APP_JOY2_L2         0x40    //L2���� 
#define  APP_JOY2_L1         0x80    //L1����

//OpenCTR�����ӿں���
void  AX_UART4_Init(uint32_t baud);  //UART ���ڳ�ʼ��
uint8_t AX_UART4_GetData(uint8_t *pbuf);    //UART ��ȡ���ڽ�������
void  AX_UART4_SendPacket(uint8_t *pbuf, uint8_t len, uint8_t num);  //UART �������ݣ�X-ProtocolЭ�飩

#endif 

