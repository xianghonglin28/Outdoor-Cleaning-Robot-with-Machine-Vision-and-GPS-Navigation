#include "ax_control.h"
#include "ax_robot.h"

/**
  * @简  述  处理手机APP控制命令
  * @参  数  无
  * @返回值  无
  */
void AX_CTL_App(void)
{
	static uint8_t comdata[16];
	
	//接收蓝牙APP串口数据
	if(AX_UART4_GetData(comdata))
	{
		//摇杆模式运动控制帧
		if((comdata[0] == ID_BLERX_YG))
		{
			R_Vel.TG_IX = (int16_t)( -6*(int8_t)comdata[3] );
			R_Vel.TG_IY = (int16_t)(  6*(int8_t)comdata[4] );
			
			#if (ROBOT_TYPE == ROBOT_AKM)
				ax_akm_angle = (int16_t)(-5*(int8_t)comdata[1] );
			#else
				R_Vel.TG_IW = (int16_t)(-20*(int8_t)comdata[1] );
			#endif	
		}
		
		//手柄模式运动控制帧
		if((comdata[0] == ID_BLERX_SB))
		{

			R_Vel.TG_IX = (int16_t)( -6*(int8_t)comdata[3] );
			R_Vel.TG_IY = (int16_t)(  6*(int8_t)comdata[4] );
			
			
			#if (ROBOT_TYPE == ROBOT_AKM)
				ax_akm_angle = (int16_t)(-5*(int8_t)comdata[1] );
			#else
				R_Vel.TG_IW = (int16_t)(-20*(int8_t)comdata[1] );
			#endif
		}
		
		//重力模式运动控制帧
		if(comdata[0] == ID_BLERX_ZL)
		{
			R_Vel.TG_IX = (int16_t)(  6*(int8_t)comdata[4] );
		
			#if (ROBOT_TYPE == ROBOT_AKM)
				ax_akm_angle = (int16_t)(-6*(int8_t)comdata[2] );
			#else
				R_Vel.TG_IW = (int16_t)(-30*(int8_t)comdata[2] );
			#endif
		}
		
		//灯光效果控制帧
		if(comdata[0] == ID_BLERX_LG)
		{
		}
		
		//灯光效果保存
		if(comdata[0] == ID_BLERX_LS)
		{
			if(comdata[1] == 0x55)
			{
				//执行灯光效果保存动作
				ax_light_save_flag = 1;
			}
		}
	}
}
