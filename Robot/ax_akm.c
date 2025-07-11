#include "ax_akm.h"
#include "ax_robot.h"

/**
  * @简  述  根据阿克曼前进和转向速度转换为前轮转向角度
  * @参  数  vx  前进速度，单位m/s 
  *          vw  转向速度，单位rad/s
  * @返回值  右前轮转向角度，单位rad
  */
int16_t AX_AKM_WToAngle(int16_t vx, int16_t vw) 
{
    float akm_angle; //前轮转向角度
	float radius;  //转弯半径

	if(vw!=0 && vx!=0)
	{
		//计算转弯半径
		radius =  (float)vx/(float)vw;
		
		//转弯半径小于最小转弯
		if(radius>0 && radius<AKM_TURN_R_MINI)
		{
			radius = AKM_TURN_R_MINI; 
		}
			
		if(radius<0 && radius>(-AKM_TURN_R_MINI))
		{
			radius = -AKM_TURN_R_MINI;
		}		

		//计算机器人前轮转向角度,单位弧度
		akm_angle = atan(AKM_ACLE_BASE/(radius));
	}
	else
	{
		akm_angle = 0;
	}
	
	return (akm_angle*1000);
}
