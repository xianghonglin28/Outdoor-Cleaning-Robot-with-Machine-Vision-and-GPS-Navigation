/**			                                                    
		   ____                    _____ _______ _____       XTARK@���˴���
		  / __ \                  / ____|__   __|  __ \ 
		 | |  | |_ __   ___ _ __ | |       | |  | |__) |
		 | |  | | '_ \ / _ \ '_ \| |       | |  |  _  / 
		 | |__| | |_) |  __/ | | | |____   | |  | | \ \ 
		  \____/| .__/ \___|_| |_|\_____|  |_|  |_|  \_\
				| |                                     
				|_|                OpenCTR   �����˿�����
									 
  ****************************************************************************** 
  *           
  * ��Ȩ���У� XTARK@���˴���  ��Ȩ���У�����ؾ�
  * ��˾��վ�� www.xtark.cn   www.tarkbot.com
  * �Ա����̣� https://xtark.taobao.com  
  * ����΢�ţ� ���˴��£���ע���ںţ���ȡ���¸�����Ѷ��
  *      
  ******************************************************************************
  * @��  ��  Musk Han@XTARK
  * @��  ��  V1.0
  * @��  ��  OLED��ʾ��������
  *
  ******************************************************************************
  */
  

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __AX_OLED_H
#define __AX_OLED_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
#include "ax_delay.h"
#include "ax_sys.h"

/***  OLED�������� **********/
void AX_OLED_Init(void);    //OLED��ʼ��
void AX_OLED_ClearScreen(void);    //OLED�����Ļ

void AX_OLED_DispChar(uint8_t x, uint8_t y, uint8_t ch, uint8_t mode);    // OLEDָ��λ����ʾһ��ASCII�ַ���6X8��
void AX_OLED_DispStr(uint8_t x, uint8_t y, uint8_t *ch, uint8_t mode);    //OLEDָ��λ����ʾASCII�ַ�����6X8��
void AX_OLED_DispNum(uint8_t x, uint8_t y, uint8_t num, uint8_t mode);    //OLEDָ��λ����ʾһ�����֣�6X8��
void AX_OLED_DispValue(uint8_t x, uint8_t y, int32_t value, uint8_t inte, uint8_t deci, uint8_t mode);    //OLEDָ��λ����ʾ��ֵ

void AX_OLED_Disp16Char(uint8_t x, uint8_t y, uint8_t ch, uint8_t mode);    // OLEDָ��λ����ʾһ��ASCII�ַ���8X16��
void AX_OLED_Disp16Str(uint8_t x, uint8_t y, uint8_t *ch, uint8_t mode);    //OLEDָ��λ����ʾASCII�ַ�����8X16��
void AX_OLED_Disp16Num(uint8_t x, uint8_t y, uint8_t num, uint8_t mode);    //OLEDָ��λ����ʾһ�����֣�8X16��
void AX_OLED_Disp16Value(uint8_t x, uint8_t y, int32_t value, uint8_t inte, uint8_t deci, uint8_t mode);    //OLEDָ��λ����ʾ��ֵ

void AX_OLED_DispPicture(uint8_t x, uint8_t y, uint8_t xsize, uint8_t ysize, const uint8_t *pbuf, uint8_t mode);    //OLEDָ��λ����ʾһ��ָ���ߴ���Ƭ
void AX_OLED_DispChinese(uint8_t x, uint8_t y, const uint8_t *pbuf, uint8_t mode);    //OLEDָ��λ����ʾһ�����֣�16X16��

#endif

/******************* (C) ��Ȩ 2023 XTARK **************************************/
