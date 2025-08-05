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
  * @��  ��  �����˵�Ч����
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ax_light.h"
#include "ax_robot.h"

//�Ƶ����ض�̬����
uint8_t light_pixel[8][3] =
{
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
};

//��ЧģʽԤ����
void Light_Effect1(void);
void Light_Effect2(void);
void Light_Effect3(void);
void Light_Effect4(void);
void Light_Effect5(void);
void Light_Effect6(void);


/**
  * @��  ��  ��Ч��ʾ����
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_LIGHT_Show(void)
{

	//��ʾģʽ1
	if(R_Light.M == LEFFECT1)
	{
		  Light_Effect1();
	}
	
	//��ʾģʽ2
	else if(R_Light.M == LEFFECT2)
	{
		  Light_Effect2();
	}
	
	//��ʾģʽ3
	else if(R_Light.M == LEFFECT3) 
	{
		 Light_Effect3();
	}
	
	//��ʾģʽ4
	else if(R_Light.M == LEFFECT4) 
	{
		 Light_Effect4();
	}
	
	//��ʾģʽ5
	else if(R_Light.M == LEFFECT5) 
	{
		 Light_Effect5();
	}
	
	//��ʾģʽ6
	else if(R_Light.M == LEFFECT6) 
	{
		 Light_Effect6();
	}
	//������Χ��Ĭ��Ч��1
	else 
	{
		 Light_Effect1();
	}

}		
	

/**
  * @��  ��  �ƹ�Ч������ɫЧ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void Light_Effect1(void)
{
	
	AX_RGB_SetFullColor(R_Light.R, R_Light.G, R_Light.B);
	 
}


/**
  * @��  ��  �ƹ�Ч��������Ч��
  * @��  ��  ��
  * @����ֵ  ��
  */
void Light_Effect2(void)
{
	 static uint8_t cnt;
	 uint8_t ar,ag,ab;
	
	 uint8_t light;
	
	 //��ɫɸѡ
	 ar = ( R_Light.R<128 ) ? 0:1;
	 ag = ( R_Light.G<128 ) ? 0:1;
	 ab = ( R_Light.B<128 ) ? 0:1;
	
	 if(cnt<41)  //����
	 {
		 light = cnt*6;
		 AX_RGB_SetFullColor( ar*light, ag*light, ab*light );
	 }
	 else if (cnt<61)  //����
	 {
		 light = 240;
		 AX_RGB_SetFullColor( ar*light, ag*light, ab*light );
	 }
	 else if (cnt<79)  //�䰵
	 {
		 light =  (80-cnt)*8;
		 AX_RGB_SetFullColor( ar*light, ag*light, ab*light );
	 }
	 else if (cnt<91)  //����
	 {
		 light =  12;
		 AX_RGB_SetFullColor( ar*light, ag*light, ab*light );
	 }
	 else
	 {
		 cnt = 2;
	 }
	 
	 cnt++;
}

 //�Զ����ɫ
const uint8_t light3_pixel[8][3] =
{
	{0xFF,0x00,0x00},{0xFF,0xFF,0x00},{0x00,0xFF,0x00},{0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF},{0x00,0x00,0xFF},{0x00,0xFF,0xFF},{0xFF,0x00,0xFF},
};

/**
  * @��  ��  �ƹ�Ч������ɫЧ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void Light_Effect3(void)
{

	AX_RGB_SetPixelColor1(light3_pixel);
	
}

//���
const uint8_t light4_pixel_red[8][3] =
{
	{0x00,0x00,0x00},{0xFF,0x00,0x00},{0xFF,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
};
//����
const uint8_t light4_pixel_blue[8][3] =
{
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0xFF},{0x00,0x00,0xFF},{0x00,0x00,0x00},
};
/**
  * @��  ��  �ƹ�Ч��������Ч��
  * @��  ��  ��
  * @����ֵ  ��
  */
void Light_Effect4(void)
{

	 static uint8_t cnt;


	 if(cnt<3)  //���
	 {
		 AX_RGB_SetPixelColor1(light4_pixel_red);
	 }
	 else if (cnt<6)  //Ϩ��
	 {
		 AX_RGB_SetFullColor( 0, 0, 0 );
	 }
	 else if (cnt<10)  //���
	 {
		 AX_RGB_SetPixelColor1(light4_pixel_red);
	 }
	 else if (cnt<20)  //Ϩ��ʱ��
	 {
		AX_RGB_SetFullColor( 0, 0, 0 );
	 }
	 else if(cnt<23)  //����
	 {
		 AX_RGB_SetPixelColor1(light4_pixel_blue);
	 }
	 else if (cnt<26)  //Ϩ��
	 {
		 AX_RGB_SetFullColor( 0, 0, 0 );
	 }
	 else if (cnt<30)  //����
	 {
		 AX_RGB_SetPixelColor1(light4_pixel_blue);
	 }
	 else if (cnt<40)  //Ϩ��ʱ��
	 {
		AX_RGB_SetFullColor( 0, 0, 0 );
	 }
	 else
	 {
		 cnt = 0;
	 }
	 
	 cnt++;
}


/**
  * @��  ��  �ƹ�Ч�����۾�Ч��
  * @��  ��  ��
  * @����ֵ  ��
  */
void Light_Effect5(void)
{
	uint8_t i;
	
	light_pixel[0][0] = R_Light.R;
	light_pixel[0][1] = R_Light.G;
	light_pixel[0][2] = R_Light.B;
	
	light_pixel[7][0] = R_Light.R;
	light_pixel[7][1] = R_Light.G;
	light_pixel[7][2] = R_Light.B;
	
	for(i=1;i<7;i++)
	{
		light_pixel[i][0] = 0;
		light_pixel[i][1] = 0;
		light_pixel[i][2] = 0;
	}
	
	AX_RGB_SetPixelColor(light_pixel);
}


/**
  * @��  ��  �ƹ�Ч��������Ч��
  * @��  ��  ��
  * @����ֵ  ��
  */
void Light_Effect6(void)
{
	static uint8_t i;
	
	//������
	if(i<8) 
	{
		//Ϩ��ǰһ����
		if(i!=0)
		{
			light_pixel[i-1][0] = 0;
			light_pixel[i-1][1] = 0;
			light_pixel[i-1][2] = 0;	
		}

		//������ǰ��
		light_pixel[i][0] = R_Light.R;
		light_pixel[i][1] = R_Light.G;
		light_pixel[i][2] = R_Light.B;	
		
		i++;
	}
	//������
	else if(i<16)
	{
		//Ϩ��ǰһ����
		if(i!=8)
		{
			light_pixel[15-i+1][0] = 0;
			light_pixel[15-i+1][1] = 0;
			light_pixel[15-i+1][2] = 0;		
		}
		
		//������ǰ��
		light_pixel[15-i][0] = R_Light.R;
		light_pixel[15-i][1] = R_Light.G;
		light_pixel[15-i][2] = R_Light.B;	
		
		i++;
	}
	else
	{
		i = 0;
	}
		
	//��������
	AX_RGB_SetPixelColor(light_pixel);
}


/******************* (C) ��Ȩ 2023 XTARK **************************************/

