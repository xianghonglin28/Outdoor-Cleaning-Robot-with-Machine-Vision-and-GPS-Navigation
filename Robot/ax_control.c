#include "ax_control.h"
#include "ax_robot.h"

/**
  * @��  ��  �����ֻ�APP��������
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_CTL_App(void)
{
	static uint8_t comdata[16];
	
	//��������APP��������
	if(AX_UART4_GetData(comdata))
	{
		//ҡ��ģʽ�˶�����֡
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
		
		//�ֱ�ģʽ�˶�����֡
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
		
		//����ģʽ�˶�����֡
		if(comdata[0] == ID_BLERX_ZL)
		{
			R_Vel.TG_IX = (int16_t)(  6*(int8_t)comdata[4] );
		
			#if (ROBOT_TYPE == ROBOT_AKM)
				ax_akm_angle = (int16_t)(-6*(int8_t)comdata[2] );
			#else
				R_Vel.TG_IW = (int16_t)(-30*(int8_t)comdata[2] );
			#endif
		}
		
		//�ƹ�Ч������֡
		if(comdata[0] == ID_BLERX_LG)
		{
		}
		
		//�ƹ�Ч������
		if(comdata[0] == ID_BLERX_LS)
		{
			if(comdata[1] == 0x55)
			{
				//ִ�еƹ�Ч�����涯��
				ax_light_save_flag = 1;
			}
		}
	}
}
