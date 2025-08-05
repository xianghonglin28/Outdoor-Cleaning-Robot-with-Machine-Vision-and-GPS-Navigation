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
  * @��  ��  �����˿��ƴ����ļ�
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ax_control.h"
#include "ax_robot.h"

/**
  * @��  ��  ����PS2�ֱ���������
  * @��  ��  ��
  * @����ֵ  ��
  */
//void AX_CTL_Ps2(void)
//{
//	static uint8_t btn_select_flag = 0;
//	static uint8_t btn_joyl_flag = 0;
//	static uint8_t btn_joyr_flag = 0;
//	static uint8_t speed = 4;
//	
//	//���̵�ģʽ�£�ִ�п��Ʋ���
//	if(my_joystick.mode ==  0x73)
//	{
//		R_Vel.TG_IX = (int16_t)(speed*(0x7F - my_joystick.RJoy_UD));
//		R_Vel.TG_IY = (int16_t)(speed*(0x80 - my_joystick.RJoy_LR));

//		//����ǰ�����������
//		#if (ROBOT_TYPE == ROBOT_AKM)
//			ax_akm_angle = (int16_t)(4*(0x80 - my_joystick.LJoy_LR));
//		#else
//			R_Vel.TG_IW = (int16_t)(4*speed*(0x80 - my_joystick.LJoy_LR));
//		#endif	
//		
//		//SELECT�������л�RGB��Чģʽ
//		if(my_joystick.btn1 & PS2_BT1_SELECT)
//		{
//			btn_select_flag = 1;
//		}
//		else
//		{
//			if(btn_select_flag)
//			{
//				//�ƹ�Ч���л�
//				if(R_Light.M < LEFFECT6)
//					R_Light.M++;
//				else
//					R_Light.M = LEFFECT1;
//				
//				//��λ���
//				btn_select_flag = 0;
//			}
//		}
//		
//		//��ҡ�˰���������
//		if(my_joystick.btn1 & PS2_BT1_JOY_L)
//		{
//			btn_joyl_flag = 1;
//		}
//		else
//		{
//			if(btn_joyl_flag)
//			{
//				
//				//�ٶȼ�С
//				if(speed > 2)
//				{
//					speed--;
//				}
//				else
//				{
//					speed = 2;
//					
//					//������������ʾ
//					ax_beep_ring = BEEP_SHORT;
//				}
//					
//				//��λ���
//				btn_joyl_flag = 0;
//			}
//		}
//		
//		//��ҡ�˰���������
//		if(my_joystick.btn1 & PS2_BT1_JOY_R)
//		{
//			btn_joyr_flag = 1;
//		}
//		else
//		{
//			if(btn_joyr_flag)
//			{
//				//�ٶ�����
//				if(speed < 9)
//				{
//					speed++;
//				}
//				else
//				{
//					speed = 9;
//					
//					//������������ʾ
//					ax_beep_ring = BEEP_SHORT;					
//				}
//					
//				//��λ���
//				btn_joyr_flag = 0;
//			}
//		}
//	}
//}	

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
			
			//����ǰ�����������
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
			
			
			//����ǰ�����������
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
			
			//����ǰ�����������
			#if (ROBOT_TYPE == ROBOT_AKM)
				ax_akm_angle = (int16_t)(-6*(int8_t)comdata[2] );
			#else
				R_Vel.TG_IW = (int16_t)(-30*(int8_t)comdata[2] );
			#endif
		}
		
		//�ƹ�Ч������֡
		if(comdata[0] == ID_BLERX_LG)
		{
			R_Light.M  = comdata[1];
			R_Light.S  = comdata[2];
			R_Light.T  = comdata[3];
			R_Light.R  = comdata[4];
			R_Light.G  = comdata[5];
			R_Light.B  = comdata[6];
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

///**
//  * @��  ��  ����SBUS��ģң������������
//  * @��  ��  ��
//  * @����ֵ  ��
//  */
//void AX_CTL_RemoteSbus(void)
//{
//	static uint16_t sbusdata[8];
//	static float speed;
//	
//	//��ȡSBUS��������
//	if(AX_SBUS_GetRxData(sbusdata))
//	{
//		//SWD-8��ͨ��7�����˵��Ϸ���Ч
//		if(sbusdata[7] < 500)
//		{
//			//SWA-5���������ٶ�
//			if(sbusdata[4] < 500)          speed = 1.5;
//			else if(sbusdata[4] < 1000)    speed = 1.0;
//			else                           speed = 0.7;
//			
//			//�˶�����
//			R_Vel.TG_IX = (int16_t)(  speed * (sbusdata[1] - 992));
//			R_Vel.TG_IY = (int16_t)( -speed * (sbusdata[0] - 992));
//			
//			//����ǰ�����������
//			#if (ROBOT_TYPE == ROBOT_AKM)
//				ax_akm_angle = (int16_t)( -0.6 * (sbusdata[3] - 992));
//			#else
//				R_Vel.TG_IW = (int16_t)( -5.0 * speed * (sbusdata[3] - 992));
//			#endif
//		}
//		
//		//�鿴����ͨ����ֵ
//		//printf("c1=%04d c2=%04d c3=%04d ch4=%04d ch5=%04d ch6=%04d ch7=%04d ch8=%04d \r\n",sbusdata[0],
//	    //       sbusdata[1],sbusdata[2],sbusdata[3],sbusdata[4],sbusdata[5],sbusdata[6],sbusdata[7]);
//	}		
//		
//	
//}

/******************* (C) ��Ȩ 2023 XTARK **************************************/

