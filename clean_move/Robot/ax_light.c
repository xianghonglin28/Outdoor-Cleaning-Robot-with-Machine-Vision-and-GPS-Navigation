#include "ax_light.h"
#include "ax_robot.h"

//灯的像素动态数组
uint8_t light_pixel[8][3] =
{
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
};

//灯效模式预定义
void Light_Effect1(void);
void Light_Effect2(void);
void Light_Effect3(void);
void Light_Effect4(void);
void Light_Effect5(void);
void Light_Effect6(void);


/**
  * @简  述  灯效显示函数
  * @参  数  无
  * @返回值  无
  */
void AX_LIGHT_Show(void)
{
		 Light_Effect1();

}		
	

/**
  * @简  述  灯光效果，单色效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect1(void)
{
	 
}


/**
  * @简  述  灯光效果，呼吸效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect2(void)
{
	 static uint8_t cnt;
	 uint8_t ar,ag,ab;
	
	 uint8_t light;
	
	
		 cnt = 2;
	 
	 
	 cnt++;
}

 //自定义彩色
const uint8_t light3_pixel[8][3] =
{
	{0xFF,0x00,0x00},{0xFF,0xFF,0x00},{0x00,0xFF,0x00},{0xFF,0xFF,0xFF},
	{0xFF,0xFF,0xFF},{0x00,0x00,0xFF},{0x00,0xFF,0xFF},{0xFF,0x00,0xFF},
};

/**
  * @简  述  灯光效果，彩色效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect3(void)
{
	
}

//红灯
const uint8_t light4_pixel_red[8][3] =
{
	{0x00,0x00,0x00},{0xFF,0x00,0x00},{0xFF,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
};
//蓝灯
const uint8_t light4_pixel_blue[8][3] =
{
	{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},{0x00,0x00,0x00},
	{0x00,0x00,0x00},{0x00,0x00,0xFF},{0x00,0x00,0xFF},{0x00,0x00,0x00},
};
/**
  * @简  述  灯光效果，警灯效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect4(void)
{

	 static uint8_t cnt;


	 cnt = 0;
	 
	 cnt++;
}


/**
  * @简  述  灯光效果，眼睛效果
  * @参  数  无
  * @返回值  无
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
  * @简  述  灯光效果，闪电效果
  * @参  数  无
  * @返回值  无
  */
void Light_Effect6(void)
{
	static uint8_t i;
	
	//正方向
	if(i<8) 
	{
		//熄灭前一个灯
		if(i!=0)
		{
			light_pixel[i-1][0] = 0;
			light_pixel[i-1][1] = 0;
			light_pixel[i-1][2] = 0;	
		}
		
		i++;
	}
	//反方向
	else if(i<16)
	{
		//熄灭前一个灯
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

