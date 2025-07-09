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
  * @��  ��  �������˶�ѧ����
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ax_kinematics.h"
#include "ax_robot.h"
#include "ax_speed.h"

//���ݻ��������ͣ�ѡ�����������˶�ѧ������


#if (ROBOT_TYPE == ROBOT_MEC)
/**
  * @��  ��  �������˶�ѧ����-�����ķ��1
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_ROBOT_Kinematics(void)
{

	//ͨ����������ȡ����ʵʱת��m/s
	R_Wheel_A.RT = (float)-((int16_t)AX_ENCODER_A_GetCounter()*MEC_WHEEL_SCALE);
	AX_ENCODER_A_SetCounter(0);
	R_Wheel_B.RT = (float)-((int16_t)AX_ENCODER_B_GetCounter()*MEC_WHEEL_SCALE);
	AX_ENCODER_B_SetCounter(0);		
	R_Wheel_C.RT = (float) ((int16_t)AX_ENCODER_C_GetCounter()*MEC_WHEEL_SCALE);
	AX_ENCODER_C_SetCounter(0);
	R_Wheel_D.RT = (float) ((int16_t)AX_ENCODER_D_GetCounter()*MEC_WHEEL_SCALE);
	AX_ENCODER_D_SetCounter(0);
	
	//�����������ת��
	//printf("@%f  %f   \r\n",R_Wheel_A.RT,R_Wheel_B.RT);
	
	//�˶�ѧ���������ɻ����������ٶȼ���������ٶ�
	R_Vel.RT_IX = (( R_Wheel_A.RT + R_Wheel_B.RT + R_Wheel_C.RT + R_Wheel_D.RT)/4)*1000;
	R_Vel.RT_IY = ((-R_Wheel_A.RT + R_Wheel_B.RT + R_Wheel_C.RT - R_Wheel_D.RT)/4)*1000;
	R_Vel.RT_IW = ((-R_Wheel_A.RT + R_Wheel_B.RT - R_Wheel_C.RT + R_Wheel_D.RT)/4/(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2))*1000;	
	
	//������Ŀ���ٶ�����
	if( R_Vel.TG_IX > R_VX_LIMIT )    R_Vel.TG_IX = R_VX_LIMIT;
	if( R_Vel.TG_IX < (-R_VX_LIMIT))  R_Vel.TG_IX = (-R_VX_LIMIT);
	if( R_Vel.TG_IY > R_VY_LIMIT)     R_Vel.TG_IY = R_VY_LIMIT;
	if( R_Vel.TG_IY < (-R_VY_LIMIT))  R_Vel.TG_IY = (-R_VY_LIMIT);
	if( R_Vel.TG_IW > R_VW_LIMIT)     R_Vel.TG_IW = R_VW_LIMIT;
	if( R_Vel.TG_IW < (-R_VW_LIMIT))  R_Vel.TG_IW = (-R_VW_LIMIT);
	
	//Ŀ���ٶ�ת��Ϊ��������
	R_Vel.TG_FX = R_Vel.TG_IX/1000.0;
	R_Vel.TG_FY = R_Vel.TG_IY/1000.0;
	R_Vel.TG_FW = R_Vel.TG_IW/1000.0;
	
	//�˶�ѧ��������ɻ�����Ŀ���ٶȼ����������ٶȣ�m/s��
	R_Wheel_A.TG = R_Vel.TG_FX - R_Vel.TG_FY - R_Vel.TG_FW*(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2);
	R_Wheel_B.TG = R_Vel.TG_FX + R_Vel.TG_FY + R_Vel.TG_FW*(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2);
	R_Wheel_C.TG = R_Vel.TG_FX + R_Vel.TG_FY - R_Vel.TG_FW*(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2);
	R_Wheel_D.TG = R_Vel.TG_FX - R_Vel.TG_FY + R_Vel.TG_FW*(MEC_WHEEL_BASE/2+MEC_ACLE_BASE/2);
	
	//����PID�㷨������PWMֵ
	R_Wheel_A.PWM = AX_SPEED_PidCtlA(R_Wheel_A.TG, R_Wheel_A.RT);   
	R_Wheel_B.PWM = AX_SPEED_PidCtlB(R_Wheel_B.TG, R_Wheel_B.RT);  
	R_Wheel_C.PWM = AX_SPEED_PidCtlC(R_Wheel_C.TG, R_Wheel_C.RT);  
	R_Wheel_D.PWM = AX_SPEED_PidCtlD(R_Wheel_D.TG, R_Wheel_D.RT);  
	
	//���õ��PWMֵ
	AX_MOTOR_A_SetSpeed(-R_Wheel_A.PWM);
	AX_MOTOR_B_SetSpeed(-R_Wheel_B.PWM);  
	AX_MOTOR_C_SetSpeed( R_Wheel_C.PWM);
	AX_MOTOR_D_SetSpeed( R_Wheel_D.PWM); 
	
	//printf("A%f B%f C%f D%f \r\n ",MOTOR_A.Wheel_RT, MOTOR_B.Wheel_RT, MOTOR_C.Wheel_RT, MOTOR_D.Wheel_RT  );
	//printf("A%d B%d C%d  \r\n ",R_Vel.I_X, R_Vel.I_Y, R_Vel.I_W );
}
#endif


#if (ROBOT_TYPE == ROBOT_FWD)
/**
  * @��  ��  �������˶�ѧ����-���ֲ���
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_ROBOT_Kinematics(void)
{
	
	//���ֲ���������̥Ħ�����ϴ󣬲���60���ٱȵ����
	//ע�⣬60���ٱȵ��ת����30���ٱȸպ÷���

	//ͨ����������ȡ����ʵʱת��m/s
	R_Wheel_A.RT = (float)-((int16_t)AX_ENCODER_A_GetCounter()*FWD_WHEEL_SCALE);
	AX_ENCODER_A_SetCounter(0);
	R_Wheel_B.RT = (float) ((int16_t)AX_ENCODER_B_GetCounter()*FWD_WHEEL_SCALE);
	AX_ENCODER_B_SetCounter(0);		
	R_Wheel_C.RT = (float)-((int16_t)AX_ENCODER_C_GetCounter()*FWD_WHEEL_SCALE);
	AX_ENCODER_C_SetCounter(0);
	R_Wheel_D.RT = (float) ((int16_t)AX_ENCODER_D_GetCounter()*FWD_WHEEL_SCALE);
	AX_ENCODER_D_SetCounter(0);
	
	//�����������ת��
	//printf("@%f  %f   \r\n",R_Wheel_A.RT,R_Wheel_B.RT);
	
	//�˶�ѧ���������ɻ����������ٶȼ���������ٶ�
	R_Vel.RT_IX = (( R_Wheel_A.RT + R_Wheel_B.RT + R_Wheel_C.RT + R_Wheel_D.RT)/4)*1000;
	R_Vel.RT_IY = 0;
	R_Vel.RT_IW = ((-R_Wheel_A.RT + R_Wheel_B.RT - R_Wheel_C.RT + R_Wheel_D.RT)/(2*FWD_WB_SCALE*FWD_WHEEL_BASE))*1000;	
	
	//������Ŀ���ٶ�����
	if( R_Vel.TG_IX > R_VX_LIMIT )    R_Vel.TG_IX = R_VX_LIMIT;
	if( R_Vel.TG_IX < (-R_VX_LIMIT))  R_Vel.TG_IX = (-R_VX_LIMIT);
	if( R_Vel.TG_IY > R_VY_LIMIT)     R_Vel.TG_IY = R_VY_LIMIT;
	if( R_Vel.TG_IY < (-R_VY_LIMIT))  R_Vel.TG_IY = (-R_VY_LIMIT);
	if( R_Vel.TG_IW > R_VW_LIMIT)     R_Vel.TG_IW = R_VW_LIMIT;
	if( R_Vel.TG_IW < (-R_VW_LIMIT))  R_Vel.TG_IW = (-R_VW_LIMIT);
	
	//Ŀ���ٶ�ת��Ϊ��������
	R_Vel.TG_FX = R_Vel.TG_IX/1000.0;
	R_Vel.TG_FY = 0;
	R_Vel.TG_FW = R_Vel.TG_IW/1000.0;
	
	//�˶�ѧ��������ɻ�����Ŀ���ٶȼ����������ٶȣ�m/s��
	R_Wheel_A.TG = R_Vel.TG_FX - R_Vel.TG_FW*(FWD_WHEEL_BASE*FWD_WB_SCALE/2);
	R_Wheel_B.TG = R_Vel.TG_FX + R_Vel.TG_FW*(FWD_WHEEL_BASE*FWD_WB_SCALE/2);
	R_Wheel_C.TG = R_Vel.TG_FX - R_Vel.TG_FW*(FWD_WHEEL_BASE*FWD_WB_SCALE/2);
	R_Wheel_D.TG = R_Vel.TG_FX + R_Vel.TG_FW*(FWD_WHEEL_BASE*FWD_WB_SCALE/2);
	
	
	//����PID�㷨������PWMֵ
	R_Wheel_A.PWM = AX_SPEED_PidCtlA(R_Wheel_A.TG, R_Wheel_A.RT);   
	R_Wheel_B.PWM = AX_SPEED_PidCtlB(R_Wheel_B.TG, R_Wheel_B.RT);  
	R_Wheel_C.PWM = AX_SPEED_PidCtlC(R_Wheel_C.TG, R_Wheel_C.RT);  
	R_Wheel_D.PWM = AX_SPEED_PidCtlD(R_Wheel_D.TG, R_Wheel_D.RT);  
	
	if(R_Vel.TG_IX ==0 && R_Vel.TG_IW ==0)
	{
		R_Wheel_A.PWM = 0;   
		R_Wheel_B.PWM = 0;  
		R_Wheel_C.PWM = 0;  
		R_Wheel_D.PWM = 0;  	
	}
	
	//���õ��PWMֵ
	AX_MOTOR_A_SetSpeed(  R_Wheel_A.PWM);
	AX_MOTOR_B_SetSpeed(  R_Wheel_B.PWM);  
	AX_MOTOR_C_SetSpeed( -R_Wheel_C.PWM);
	AX_MOTOR_D_SetSpeed( -R_Wheel_D.PWM); 
	
	//���������Ϣ
	//printf("@%f  %f  %f %f  \r\n",R_Wheel_A.RT,R_Wheel_B.RT, R_Wheel_C.RT ,R_Wheel_D.RT);
	//printf("A%d B%d C%d  \r\n ",R_Vel.I_X, R_Vel.I_Y, R_Vel.I_W );
}
#endif


#if (ROBOT_TYPE == ROBOT_AKM)
/**
  * @��  ��  �������˶�ѧ����-������
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_ROBOT_Kinematics(void)
{

	float akm_spd;  //�ٶ�
    float akm_angle; //ǰ��ת��Ƕ�
	float akm_angle_r;  //��ǰ��ת��Ƕ�
	float akm_angle_s;  //���ת��Ƕ�
	float radius;  //ת��뾶
	int16_t servo_angle; //������ƽǶ�
	
	//ͨ����������ȡ����ʵʱת��m/s
	R_Wheel_A.RT = (float) ((int16_t)AX_ENCODER_A_GetCounter()*AKM_WHEEL_SCALE);
	AX_ENCODER_A_SetCounter(0);
	R_Wheel_B.RT = (float)-((int16_t)AX_ENCODER_B_GetCounter()*AKM_WHEEL_SCALE);
	AX_ENCODER_B_SetCounter(0);	
	
	//�����������ת��
	//printf("@%f  %f   \r\n",R_Wheel_A.RT,R_Wheel_B.RT);
	
	/*******�����˶�ѧ����(����ǰ���ٶȺͶ��ת�Ǽ���)*********************************/
	
	//������Ŀ���ٶ�����
	if( R_Vel.TG_IX > R_VX_LIMIT )    R_Vel.TG_IX = R_VX_LIMIT;
	if( R_Vel.TG_IX < (-R_VX_LIMIT))  R_Vel.TG_IX = (-R_VX_LIMIT);
	
	//ת��Ƕ�����
//	if( ax_akm_angle > R_VX_LIMIT )    ax_akm_angle = R_VX_LIMIT;
//	if( ax_akm_angle < (-R_VX_LIMIT))  ax_akm_angle = (-R_VX_LIMIT);
	
	//ת��Ϊ����������
	akm_spd = R_Vel.TG_IX/1000.0;
	akm_angle = ax_akm_angle/1000.0;
	
	//����ת��Ƕȼ���ת��뾶
	radius = AKM_ACLE_BASE/tan(akm_angle);		
	
	//����ǰ�ֽǶȼ�����ǰ�ֽǶ�
	if(ax_akm_angle !=0 )
	{

		//�˶�ѧ��������ɻ�����Ŀ���ٶȼ����������ٶȣ�m/s��
		R_Wheel_A.TG = akm_spd*(radius-0.5*AKM_WHEEL_BASE)/radius;
		R_Wheel_B.TG = akm_spd*(radius+0.5*AKM_WHEEL_BASE)/radius;					
		
		//������ǰ�ֽǶ�
		akm_angle_r = (atan(AKM_ACLE_BASE/((AKM_ACLE_BASE/tan(akm_angle*0.01745))+0.5*AKM_WHEEL_BASE)))*(180.0/PI);
	}
	else
	{
		//�˶�ѧ��������ɻ�����Ŀ���ٶȼ����������ٶȣ�m/s��
		R_Wheel_A.TG = akm_spd;
		R_Wheel_B.TG = akm_spd;	
		
		//������ǰ�ֽǶ�
		akm_angle_r = 0;
	}	
	
    //������ǰ�ֽǶȣ�������ת��Ƕ�
	akm_angle_s = 	-(0.0041*akm_angle_r*akm_angle_r + 1.2053*akm_angle_r)*180/PI;
	
	//���ݶ��ת��Ƕȣ�������PWM������
	servo_angle = (akm_angle_s*10 + ax_akm_offset);  
	
	//���ö���Ƕȣ�S1,S2����һ��������
	AX_SERVO_S1_SetAngle(servo_angle);
	AX_SERVO_S2_SetAngle(servo_angle);	
	
	//����PID�㷨������PWMֵ
	R_Wheel_A.PWM = AX_SPEED_PidCtlA(R_Wheel_A.TG, R_Wheel_A.RT);   
	R_Wheel_B.PWM = AX_SPEED_PidCtlB(R_Wheel_B.TG, R_Wheel_B.RT);   
		
	//���õ��PWMֵ
	AX_MOTOR_A_SetSpeed(-R_Wheel_A.PWM);
	AX_MOTOR_B_SetSpeed(-R_Wheel_B.PWM);  
	AX_MOTOR_C_SetSpeed(0);
	AX_MOTOR_D_SetSpeed(0);	
	
	//�˶�ѧ���������ɻ����������ٶȼ���������ٶ�
	R_Vel.RT_IX = ((R_Wheel_A.RT + R_Wheel_B.RT)/2)*1000;
	R_Vel.RT_IY = 0;
	R_Vel.RT_IW = ((-R_Wheel_A.RT + R_Wheel_B.RT)/AKM_WHEEL_BASE)*1000;	
}
#endif


#if (ROBOT_TYPE == ROBOT_TWD)
/**
  * @��  ��  �������˶�ѧ����-���ֲ���
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_ROBOT_Kinematics(void)
{
	
	//ͨ����������ȡ����ʵʱת��m/s
	R_Wheel_A.RT = (float) ((int16_t)AX_ENCODER_A_GetCounter()*TWD_WHEEL_SCALE);
	AX_ENCODER_A_SetCounter(0);
	R_Wheel_B.RT = (float)-((int16_t)AX_ENCODER_B_GetCounter()*TWD_WHEEL_SCALE);
	AX_ENCODER_B_SetCounter(0);			
	
	//�����������ת��
	//printf("@%f  %f   \r\n",R_Wheel_A.RT,R_Wheel_B.RT);
	
	//�˶�ѧ���������ɻ����������ٶȼ���������ٶ�
	R_Vel.RT_IX = ((R_Wheel_A.RT + R_Wheel_B.RT)/2)*1000;
	R_Vel.RT_IY = 0;
	R_Vel.RT_IW = ((-R_Wheel_A.RT + R_Wheel_B.RT)/TWD_WHEEL_BASE)*1000;		
	
	//������Ŀ���ٶ�����
	if( R_Vel.TG_IX > R_VX_LIMIT )    R_Vel.TG_IX = R_VX_LIMIT;
	if( R_Vel.TG_IX < (-R_VX_LIMIT))  R_Vel.TG_IX = (-R_VX_LIMIT);
	if( R_Vel.TG_IY > R_VY_LIMIT)     R_Vel.TG_IY = R_VY_LIMIT;
	if( R_Vel.TG_IY < (-R_VY_LIMIT))  R_Vel.TG_IY = (-R_VY_LIMIT);
	if( R_Vel.TG_IW > R_VW_LIMIT)     R_Vel.TG_IW = R_VW_LIMIT;
	if( R_Vel.TG_IW < (-R_VW_LIMIT))  R_Vel.TG_IW = (-R_VW_LIMIT);
	
	//Ŀ���ٶ�ת��Ϊ��������
	R_Vel.TG_FX = R_Vel.TG_IX/1000.0;
	R_Vel.TG_FY = 0;
	R_Vel.TG_FW = R_Vel.TG_IW/1000.0;
	
	//�˶�ѧ��������ɻ�����Ŀ���ٶȼ����������ٶȣ�m/s��
	R_Wheel_A.TG = R_Vel.TG_FX - R_Vel.TG_FW*(TWD_WHEEL_BASE/2);
	R_Wheel_B.TG = R_Vel.TG_FX + R_Vel.TG_FW*(TWD_WHEEL_BASE/2);	
	

	//����PID�㷨������PWMֵ
	R_Wheel_A.PWM = AX_SPEED_PidCtlA(R_Wheel_A.TG, R_Wheel_A.RT);   
	R_Wheel_B.PWM = AX_SPEED_PidCtlB(R_Wheel_B.TG, R_Wheel_B.RT);  
 
	//���õ��PWMֵ
	AX_MOTOR_A_SetSpeed(-R_Wheel_A.PWM);
	AX_MOTOR_B_SetSpeed(-R_Wheel_B.PWM); 
	AX_MOTOR_C_SetSpeed(0);
	AX_MOTOR_D_SetSpeed(0);	
 
	
	//printf("A%f B%f  \r\n ",MOTOR_A.Wheel_RT, MOTOR_B.Wheel_RT  );
	//printf("A%d B%d C%d  \r\n ",R_Vel.I_X, R_Vel.I_Y, R_Vel.I_W );	
	
}
#endif


#if (ROBOT_TYPE == ROBOT_TNK)
/**
  * @��  ��  �������˶�ѧ����-�Ĵ�ʽ
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_ROBOT_Kinematics(void)
{
	
	//ͨ����������ȡ����ʵʱת��m/s
	R_Wheel_A.RT = (float) ((int16_t)AX_ENCODER_A_GetCounter()*TNK_WHEEL_SCALE);
	AX_ENCODER_A_SetCounter(0);
	R_Wheel_B.RT = (float)-((int16_t)AX_ENCODER_B_GetCounter()*TNK_WHEEL_SCALE);
	AX_ENCODER_B_SetCounter(0);			
	
	//�����������ת��
	//printf("@%f  %f   \r\n",R_Wheel_A.RT,R_Wheel_B.RT);
	
	//�˶�ѧ���������ɻ����������ٶȼ���������ٶ�
	R_Vel.RT_IX = ((R_Wheel_A.RT + R_Wheel_B.RT)/2)*1000;
	R_Vel.RT_IY = 0;
	R_Vel.RT_IW = ((-R_Wheel_A.RT + R_Wheel_B.RT)/TNK_WHEEL_BASE)*1000;		
	
	//������Ŀ���ٶ�����
	if( R_Vel.TG_IX > R_VX_LIMIT )    R_Vel.TG_IX = R_VX_LIMIT;
	if( R_Vel.TG_IX < (-R_VX_LIMIT))  R_Vel.TG_IX = (-R_VX_LIMIT);
	if( R_Vel.TG_IY > R_VY_LIMIT)     R_Vel.TG_IY = R_VY_LIMIT;
	if( R_Vel.TG_IY < (-R_VY_LIMIT))  R_Vel.TG_IY = (-R_VY_LIMIT);
	if( R_Vel.TG_IW > R_VW_LIMIT)     R_Vel.TG_IW = R_VW_LIMIT;
	if( R_Vel.TG_IW < (-R_VW_LIMIT))  R_Vel.TG_IW = (-R_VW_LIMIT);
	
	//Ŀ���ٶ�ת��Ϊ��������
	R_Vel.TG_FX = R_Vel.TG_IX/1000.0;
	R_Vel.TG_FY = 0;
	R_Vel.TG_FW = R_Vel.TG_IW/1000.0;
	
	//�˶�ѧ��������ɻ�����Ŀ���ٶȼ����������ٶȣ�m/s��
	R_Wheel_A.TG = R_Vel.TG_FX - R_Vel.TG_FW*(TNK_WHEEL_BASE/2);
	R_Wheel_B.TG = R_Vel.TG_FX + R_Vel.TG_FW*(TNK_WHEEL_BASE/2);	

	//����PID�㷨������PWMֵ
	R_Wheel_A.PWM = AX_SPEED_PidCtlA(R_Wheel_A.TG, R_Wheel_A.RT);   
	R_Wheel_B.PWM = AX_SPEED_PidCtlB(R_Wheel_B.TG, R_Wheel_B.RT);  
 
	//���õ��PWMֵ
	AX_MOTOR_A_SetSpeed(-R_Wheel_A.PWM);
	AX_MOTOR_B_SetSpeed(-R_Wheel_B.PWM); 
	AX_MOTOR_C_SetSpeed(0);
	AX_MOTOR_D_SetSpeed(0);	
	
}
#endif


#if (ROBOT_TYPE == ROBOT_OMT)
/**
  * @��  ��  �������˶�ѧ����-����ȫ��
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_ROBOT_Kinematics(void)
{
	
	//ͨ����������ȡ����ʵʱת��m/s
	R_Wheel_A.RT = (float)-((int16_t)AX_ENCODER_A_GetCounter()*MEC_WHEEL_SCALE);
	AX_ENCODER_A_SetCounter(0);
	R_Wheel_B.RT = (float) ((int16_t)AX_ENCODER_B_GetCounter()*MEC_WHEEL_SCALE);
	AX_ENCODER_B_SetCounter(0);		
	R_Wheel_C.RT = (float)-((int16_t)AX_ENCODER_C_GetCounter()*MEC_WHEEL_SCALE);
	AX_ENCODER_C_SetCounter(0);

	
	//�����������ת��
	//printf("@%f  %f   \r\n",R_Wheel_A.RT,R_Wheel_B.RT);
	
	//�˶�ѧ���������ɻ����������ٶȼ���������ٶ�
	R_Vel.RT_IX = ((-R_Wheel_A.RT + R_Wheel_B.RT)/SQRT3)*1000;
	R_Vel.RT_IY = ((-R_Wheel_A.RT + R_Wheel_B.RT - 2.0*R_Wheel_C.RT)/3)*1000;
	R_Vel.RT_IW = (( R_Wheel_A.RT + R_Wheel_B.RT + R_Wheel_C.RT)/(3*OMT_WHEEL_RADIUS))*1000;	
	
	//������Ŀ���ٶ�����
	if( R_Vel.TG_IX > R_VX_LIMIT )    R_Vel.TG_IX = R_VX_LIMIT;
	if( R_Vel.TG_IX < (-R_VX_LIMIT))  R_Vel.TG_IX = (-R_VX_LIMIT);
	if( R_Vel.TG_IY > R_VY_LIMIT)     R_Vel.TG_IY = R_VY_LIMIT;
	if( R_Vel.TG_IY < (-R_VY_LIMIT))  R_Vel.TG_IY = (-R_VY_LIMIT);
	if( R_Vel.TG_IW > R_VW_LIMIT)     R_Vel.TG_IW = R_VW_LIMIT;
	if( R_Vel.TG_IW < (-R_VW_LIMIT))  R_Vel.TG_IW = (-R_VW_LIMIT);
	
	//Ŀ���ٶ�ת��Ϊ��������
	R_Vel.TG_FX = R_Vel.TG_IX/1000.0;
	R_Vel.TG_FY = R_Vel.TG_IY/1000.0;
	R_Vel.TG_FW = R_Vel.TG_IW/1000.0;
	
	//�˶�ѧ��������ɻ�����Ŀ���ٶȼ����������ٶȣ�m/s��
	R_Wheel_A.TG = -0.5*R_Vel.TG_FX + 0.5*R_Vel.TG_FY + OMT_WHEEL_RADIUS*R_Vel.TG_FW;
	R_Wheel_B.TG =  0.5*R_Vel.TG_FX + 0.5*R_Vel.TG_FY + OMT_WHEEL_RADIUS*R_Vel.TG_FW;
	R_Wheel_C.TG = -R_Vel.TG_FY + OMT_WHEEL_RADIUS*R_Vel.TG_FW;

	
	//����PID�㷨������PWMֵ
	R_Wheel_A.PWM = AX_SPEED_PidCtlA(R_Wheel_A.TG, R_Wheel_A.RT);   
	R_Wheel_B.PWM = AX_SPEED_PidCtlB(R_Wheel_B.TG, R_Wheel_B.RT);  
	R_Wheel_C.PWM = AX_SPEED_PidCtlC(R_Wheel_C.TG, R_Wheel_C.RT);  
 
	
	//���õ��PWMֵ
	AX_MOTOR_A_SetSpeed( R_Wheel_A.PWM);
	AX_MOTOR_B_SetSpeed( R_Wheel_B.PWM);  
	AX_MOTOR_C_SetSpeed(-R_Wheel_C.PWM);
	AX_MOTOR_D_SetSpeed(0); 
	
	//printf("A%f B%f C%f D%f \r\n ",MOTOR_A.Wheel_RT, MOTOR_B.Wheel_RT, MOTOR_C.Wheel_RT, MOTOR_D.Wheel_RT  );
	//printf("A%d B%d C%d  \r\n ",R_Vel.I_X, R_Vel.I_Y, R_Vel.I_W );
}
#endif

/**
  * @��  ��  ������ֹͣ�˶�״̬
  * @��  ��  ��
  * @����ֵ  ��
  */
void AX_ROBOT_Stop(void)
{
	
	//���õ��PWMֵ
	AX_MOTOR_A_SetSpeed(0);
	AX_MOTOR_B_SetSpeed(0);  
	AX_MOTOR_C_SetSpeed(0);
	AX_MOTOR_D_SetSpeed(0);  
	
}


/******************* (C) ��Ȩ 2023 XTARK **************************************/

