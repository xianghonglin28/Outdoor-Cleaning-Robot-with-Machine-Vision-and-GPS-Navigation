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
		 Light_Effect1();

}		
	

/**
  * @��  ��  �ƹ�Ч������ɫЧ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void Light_Effect1(void)
{
	 
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
	
	
		 cnt = 2;
	 
	 
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


	 cnt = 0;
	 
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
	
	for(i=1;i<7;i++)
	{
		light_pixel[i][0] = 0;
		light_pixel[i][1] = 0;
		light_pixel[i][2] = 0;
	}
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
		

		
		i++;
	}
	else
	{
		i = 0;
	}

}

