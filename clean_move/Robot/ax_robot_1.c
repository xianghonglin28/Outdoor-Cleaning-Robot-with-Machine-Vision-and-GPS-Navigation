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
  * @��  ��  �����˿���������
  * 
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "ax_robot.h"
#include "ax_light.h"
#include "ax_kinematics.h"
#include "ax_control.h"
#include "ax_servo.h"
#include "ax_uart2.h"
#include "ax_uart5.h"
#include "gps.h"
#include "ax_openmv.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

//������Ʊ�־λ
volatile uint8_t servo_flag = 0;


//�������ٶ�����
ROBOT_Velocity  R_Vel;

//��������������
ROBOT_Wheel  R_Wheel_A,R_Wheel_B,R_Wheel_C,R_Wheel_D;

//������IMU����
ROBOT_Imu  R_Imu;

//������RGB����
ROBOT_Light  R_Light;

//�������ٶ�����
ROBOT_Velocity  R_Vel;

//�����˵�ص�ѹ����
uint16_t R_Bat_Vol;  

u8 USART1_TX_BUF[UART5_MAX_RECV_LEN]; 
nmea_msg gpsx;

//IMU����
int16_t ax_imu_acc_data[3];  
int16_t ax_imu_gyro_data[3]; 
int16_t ax_imu_gyro_offset[3]; 

//���PID���Ʋ���
int16_t ax_motor_kp=800;      
int16_t ax_motor_kd=1000; 

//�������˶�ʹ�ܿ���,Ĭ�ϴ�״̬
uint8_t ax_robot_move_enable = 1;

//IMUУ׼��־λ
int8_t ax_imu_calibrate_flag = 0;

//�ƹ⿪��ʧ�ܣ�Ĭ�ϴ�״̬
uint8_t ax_light_enable = 1;

//�ƹ�Ч����EEPROM���ݸ���
uint8_t ax_light_save_flag = 0;

//����������һ��
uint8_t ax_beep_ring = 0;

////PS2�ֱ���ֵ�ṹ��
//JOYSTICK_TypeDef my_joystick;  

//���Ʒ�ʽѡ��
uint8_t ax_control_mode = CTL_ROS;

//������������ǰ��ת��Ƕ�
int16_t ax_akm_angle = 0;

//������������ǰ����ƫ�Ƕ�
int16_t ax_akm_offset = 0;


//��������
void ROBOT_IMUHandle(void);     //IMU���ݴ���
void ROBOT_SendDataToRos(void);  //��������


/**
  * @��  ��  �����˹�������
  * @��  ��  ��
  * @����ֵ  ��
  */
void Robot_Task(void* parameter)
{	

	//���ڱ����ϴ�ʱ�䡣���ú�ϵͳ�Զ�����
	static portTickType PreviousWakeTime;

	//������ʱʱ��20ms����ʱ��תΪ������ 
	const portTickType TimeIncrement = pdMS_TO_TICKS(20);
	
	//��ȡ��ǰϵͳʱ�� 
	PreviousWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		
		//���þ�����ʱ����20ms,ִ��Ƶ��50HZ
		vTaskDelayUntil(&PreviousWakeTime, TimeIncrement );
		
		//���Ʒ�ʽѡ��
		if(ax_control_mode != 0)
		{
//			if      (ax_control_mode == CTL_PS2)    AX_CTL_Ps2();    //PS2�ֱ�����
			 if (ax_control_mode == CTL_APP)    AX_CTL_App();    //APP����
//			else if (ax_control_mode == CTL_RMS)    AX_CTL_RemoteSbus();   //SBUS��ģң��������
		}
		
		//�жϻ������˶��Ƿ���
		if(ax_robot_move_enable != 0)
		{
			//�������˶�ѧ����
			AX_ROBOT_Kinematics();				
		}
		else
		{
			//�����˾�ֹ����
			AX_ROBOT_Stop();				
		}
		
		//��ȡPMU6050���ٶ�����
        ROBOT_IMUHandle();
		
		//���ݷ���
		ROBOT_SendDataToRos();
	}
}

/**
  * @��  ��  ������IMU���ݴ���
  * @��  ��  ��
  * @����ֵ  ��
  */
void ROBOT_IMUHandle(void)
{
		
	//��ȡPMU6050���ٶ�����
	AX_MPU6050_GetAccData(ax_imu_acc_data);
	
	//IMU�����������ROS����任
	R_Imu.ACC_X =  ax_imu_acc_data[1];  //ROS����X���ӦIMU��Y��
	R_Imu.ACC_Y = -ax_imu_acc_data[0];  //ROS����Y���ӦIMU��X�ᷴ��
	R_Imu.ACC_Z =  ax_imu_acc_data[2];  //ROS����Z���ӦIMU��Z��
	
	//��ȡPMU6050����������
	AX_MPU6050_GetGyroData(ax_imu_gyro_data);
	
	//�����Ǽ�����ƱУ׼����
	ax_imu_gyro_data[0] += ax_imu_gyro_offset[0];
	ax_imu_gyro_data[1] += ax_imu_gyro_offset[1];
	ax_imu_gyro_data[2] += ax_imu_gyro_offset[2];
	
	//IMU�����������ROS����任
	R_Imu.GYRO_X =  ax_imu_gyro_data[1];  //ROS����X���ӦIMU��Y��
	R_Imu.GYRO_Y = -ax_imu_gyro_data[0];  //ROS����Y���ӦIMU��X�ᷴ��
	R_Imu.GYRO_Z =  ax_imu_gyro_data[2];  //ROS����Z���ӦIMU��Z��
	
	//�۲�����
    //printf("@ %d  \r\n",R_Imu.ACC_X);
}


/**
  * @��  ��  �����˷������ݵ�ROS
  * @��  ��  ��
  * @����ֵ  ��
  */
void ROBOT_SendDataToRos(void)
{
    //���ڷ�������
	static uint8_t comdata[20]; 	

	//���ٶ� = (ax_acc/32768) * 2G  
	comdata[0] = (u8)( R_Imu.ACC_X >> 8 );  
	comdata[1] = (u8)( R_Imu.ACC_X );
	comdata[2] = (u8)( R_Imu.ACC_Y >> 8 );
	comdata[3] = (u8)( R_Imu.ACC_Y );
	comdata[4] = (u8)( R_Imu.ACC_Z >> 8 );
	comdata[5] = (u8)( R_Imu.ACC_Z );
	
	//�����ǽ��ٶ� = (ax_gyro/32768) * 500
	comdata[6] = (u8)( R_Imu.GYRO_X >> 8 );
	comdata[7] = (u8)( R_Imu.GYRO_X );
	comdata[8] = (u8)( R_Imu.GYRO_Y >> 8 );
	comdata[9] = (u8)( R_Imu.GYRO_Y );
	comdata[10] = (u8)( R_Imu.GYRO_Z	>> 8 );
	comdata[11] = (u8)( R_Imu.GYRO_Z );
	
	//�������ٶ�ֵ ��λΪm/s���Ŵ�1000��
	comdata[12] = (u8)( R_Vel.RT_IX >> 8 );
	comdata[13] = (u8)( R_Vel.RT_IX );
	comdata[14] = (u8)( R_Vel.RT_IY >> 8 );
	comdata[15] = (u8)( R_Vel.RT_IY );
	comdata[16] = (u8)( R_Vel.RT_IW >> 8 );
	comdata[17] = (u8)( R_Vel.RT_IW );
	
	//��ص�ѹ
	comdata[18] = (u8)( R_Bat_Vol >> 8 );
	comdata[19] = (u8)( R_Bat_Vol );

	//TTL���ڷ�������
  //AX_UART2_SendPacket(comdata, 20, ID_UTX_DATA);		
	
	//USB���ڷ�������
    AX_UART3_SendPacket(comdata, 20, ID_UTX_DATA);		
	
	//CAN���ݷ���
	AX_CAN_SendPacket(ID_CANTX_MSG1, 6, &comdata[0]);
	AX_CAN_SendPacket(ID_CANTX_MSG2, 6, &comdata[6]);
	AX_CAN_SendPacket(ID_CANTX_MSG3, 8, &comdata[12]);
}

/**
  * @��  ��  ���¹�������
  * @��  ��  ��
  * @����ֵ  ��
  */
void Trivia_Task(void* parameter)
{	
	uint8_t i;	
	
	//��������
	static uint16_t ax_bat_vol_cnt = 0; 
	
	//������У׼����
	static int16_t gyro_data[3]; 
	

	while (1)
	{	
		
		/*****��ع���***********************************/
		
		//�ɼ���ص�ѹ
	    R_Bat_Vol = AX_VIN_GetVol_X100();
		
		//���������ص�ѹ����
        //printf("@ %d  \r\n",R_Bat_Vol);		
		
		//��������40%
		if(R_Bat_Vol < VBAT_40P)  
		{
			//��ƿ�ʼ��˸��ʾ
			AX_LED_Red_Toggle();
			
			//��������20%
			if(R_Bat_Vol < VBAT_20P)
			{
				//��Ƴ���
				AX_LED_Red_On();
				
				//��������10%���ر�ϵͳ���뱣��״̬
				if(R_Bat_Vol < VBAT_10P) //990
				{
					//��ѹʱ�����
					ax_bat_vol_cnt++;
					
					//����10�Σ�����ر�״̬
					if(ax_bat_vol_cnt > 10 )
					{
						//�ر��̵ƣ���Ƴ���
						AX_LED_Green_Off();
						AX_LED_Red_On();
						
						//�������
						vTaskSuspend(Robot_Task_Handle);
						vTaskSuspend(Disp_Task_Handle);
						
						//����ٶ�����Ϊ0
						AX_MOTOR_A_SetSpeed(0);
						AX_MOTOR_B_SetSpeed(0);  
						AX_MOTOR_C_SetSpeed(0);
						AX_MOTOR_D_SetSpeed(0); 
						
						//���OLED����������ʾ
						AX_OLED_ClearScreen();  //
						
						//���������б���
						while(1)
						{	
							//��ʾ������ֹͣ��Ϣ
							AX_OLED_DispStr(0, 3, "     Low power      ", 0);	
							AX_OLED_DispStr(0, 5, "  Robot has stopped ", 0);	
							AX_BEEP_On();
							vTaskDelay(30);
							AX_BEEP_Off();
							
							vTaskDelay(1000);	
							AX_OLED_ClearScreen(); 
							vTaskDelay(1000);						
						}								
					}
				}
				else
				{
					ax_bat_vol_cnt = 0;
				}				
			}
		}
		else
		{
			//��ƹر�
			AX_LED_Red_Off();
		}
		
		/*****IMUУ׼***********************************/
		
		//���IMUУ׼��־λ
		if(ax_imu_calibrate_flag > 0)
		{
            //����IMUУ׼����
			ax_imu_gyro_offset[0] = 0;
			ax_imu_gyro_offset[1] = 0;
			ax_imu_gyro_offset[2] = 0;
			
			//�������������
			vTaskSuspend(Robot_Task_Handle);
			
			//��������ʾ
			AX_BEEP_On();
			vTaskDelay(50);
			AX_BEEP_Off();
			vTaskDelay(1000);			
			
			//���������ʾ
			AX_LED_Red_On();
			
			//������У׼
			for(i=0; i<10; i++) 
			{
				//��ʱ����
				vTaskDelay(20);
				
				//��ȡPMU6050����������
				AX_MPU6050_GetGyroData(gyro_data);
				
				//����ƫ���
				ax_imu_gyro_offset[0] += gyro_data[0];
				ax_imu_gyro_offset[1] += gyro_data[1];
				ax_imu_gyro_offset[2] += gyro_data[2]; 		
			}
			
			//����ƽ��ƫ��ֵ
			ax_imu_gyro_offset[0] = -ax_imu_gyro_offset[0]/10;
			ax_imu_gyro_offset[1] = -ax_imu_gyro_offset[1]/10;
			ax_imu_gyro_offset[2] = -ax_imu_gyro_offset[2]/10;
			
			//�رպ��
			AX_LED_Red_Off();
			
			//����������ָ�
			vTaskResume(Robot_Task_Handle);
			
			//��λIMUУ׼��־λ			
			ax_imu_calibrate_flag = 0;

		}   	
		
		/*****�ƹ�Ч��EEPROM����***********************************/
		
		//����EEPROM�еƹ�Ч������
		if(ax_light_save_flag > 0)
		{
			//�������������
			vTaskSuspend(Robot_Task_Handle);
			
			//��������
			AX_EEPROM_WriteOneByte(0x20, R_Light.M);
			AX_EEPROM_WriteOneByte(0x21, R_Light.S);
			AX_EEPROM_WriteOneByte(0x22, R_Light.T);
			AX_EEPROM_WriteOneByte(0x23, R_Light.R);
			AX_EEPROM_WriteOneByte(0x24, R_Light.G);
			AX_EEPROM_WriteOneByte(0x25, R_Light.B);
 
			//��������ʾ
			AX_BEEP_On();
			vTaskDelay(200);
			AX_BEEP_Off();
			vTaskDelay(1000);	
			
			//����������ָ�
			vTaskResume(Robot_Task_Handle);
			
			//��λ�ƹⱣ���־λ			
			ax_light_save_flag = 0;
			
		}			
		
		/*****����������һ��***********************************/
		if(ax_beep_ring != 0)
		{
			if(ax_beep_ring == BEEP_SHORT)
			{
				//������һ��
				AX_BEEP_On();
				vTaskDelay(200); 
				AX_BEEP_Off();
				
				//����һ����־��λ
				ax_beep_ring = 0;
			}
			else //
			{
				//������һ��
				AX_BEEP_On();
				vTaskDelay(1000); 
				AX_BEEP_Off();
				
				//����һ����־��λ
				ax_beep_ring = 0;				
			}
			
		}
		
		//LEDϵͳ����ָʾ
		AX_LED_Green_Toggle();	
		
		
        //ѭ������500ms
		vTaskDelay(500); 
	}			
}

/**
  * @��  ��  ������������
  * @��  ��  ��
  * @����ֵ  ��
  */
void Key_Task(void* parameter)
{	
	uint8_t  i;
	//int16_t  akm_offset;
	
	while (1)
	{		
		//����ɨ��
		if(AX_KEY_Scan() != 0)
		{
			//�����ʱ
			vTaskDelay(50);  
			
			//ȷ����������
			if(AX_KEY_Scan() != 0)
			{
				//�ȴ�����̧��
				for(i=0; i<200; i++)
				{

					vTaskDelay(50);
					
					if(AX_KEY_Scan() == 0)
					{
						break;
					}
					
					//����ʱ��3Sʱ��������ʾ��
					if(i == 60)
					{
						AX_BEEP_On();
						vTaskDelay(200);
						AX_BEEP_Off();
					}
				}

				//�̰����,С��1S
				if(i < 20)
				{
					AX_EEPROM_WriteOneByte(0x00, 0x00);
					//�������
					vTaskSuspend(Robot_Task_Handle);
					vTaskSuspend(Disp_Task_Handle);
					AX_MOTOR_A_SetSpeed(0);
					AX_MOTOR_B_SetSpeed(0);  
					AX_MOTOR_C_SetSpeed(0);
					AX_MOTOR_D_SetSpeed(0); 
					
					//���OLED����������ʾ
					AX_OLED_ClearScreen(); 
					//�ƹ�Ч���л�
	        
//					if(R_Light.M < LEFFECT6)
//						R_Light.M++;
//					else
//						R_Light.M = LEFFECT1;
					while(1)
					{
						
					
					}
					
					//IMU������У׼
					//ax_imu_calibrate_flag = 1;
				}
				
				//�а����,����3S��С��10S
				if(i>60 && i<200)
				{
					//�ж��Ƿ���AKMģʽ��
					#if (ROBOT_TYPE == ROBOT_AKM)
					
					//�������˶�����ֹͣ�������˾�ֹ
					ax_robot_move_enable = 0;
					
					//������У׼	
					AX_OLED_DispStr(0, 4, "                     ", 0);							
					AX_OLED_DispStr(0, 4, " AKM offset:", 1);
					
					//�ȴ���������
					while(1)
					{
						
						//��ʾʵʱ��ƫ�����Ƕ�
						AX_OLED_DispValue(78, 4, (ax_akm_offset), 4, 0, 0);
						
						//����B������ٶ���
						akm_offset = ((int16_t)AX_ENCODER_B_GetCounter()*AKM_WHEEL_SCALE*10);
						AX_ENCODER_B_SetCounter(0);	
						
						//ƫ�ýǶ�������
						if(akm_offset > 0)
						{
							if(ax_akm_offset<120)
							{
								ax_akm_offset += akm_offset;
							}
							else
							{
								//������������ʾ
								AX_BEEP_On();
								vTaskDelay(30);
								AX_BEEP_Off();
							}
						}
						
						if(akm_offset < 0)
						{
							if(ax_akm_offset>-120)
							{
								ax_akm_offset += akm_offset;
							}
							else
							{
								//������������ʾ
								AX_BEEP_On();
								vTaskDelay(30);
								AX_BEEP_Off();
							}
						}
						
						//���ö���Ƕ�
						AX_SERVO_S1_SetAngle(ax_akm_offset);
						AX_SERVO_S2_SetAngle(ax_akm_offset);	
						
						vTaskDelay(50);
						
						//�������£��Ƴ�У׼
						if(AX_KEY_Scan() != 0)
						{
							break;
						}
					}
					
					//У׼���ݱ��浽EEPROM�У�
					AX_EEPROM_WriteOneByte(0x50, ax_akm_offset);						
					
					//������������ʾ
					AX_BEEP_On();
					vTaskDelay(500);
					AX_BEEP_Off();	
					vTaskDelay(800);	
					
					//�������˶���������
					ax_robot_move_enable = 0;
					
					//�ָ�OLED��ʾ
					AX_OLED_DispStr(0, 4, "---------------------", 0);	
						
					#endif

				}				
				
					
				//������⣬����10S
				if(i == 200)
				{
					//���EEPROM������ǣ��ָ�Ĭ������
					AX_EEPROM_WriteOneByte(0x00, 0x00);
					
					//�������
					vTaskSuspend(Robot_Task_Handle);
					vTaskSuspend(Disp_Task_Handle);
					
					//��������ʾ
					AX_BEEP_On();
					vTaskDelay(3000);
					AX_BEEP_Off();					
						
					//����ٶ�����Ϊ0
					AX_MOTOR_A_SetSpeed(0);
					AX_MOTOR_B_SetSpeed(0);  
					AX_MOTOR_C_SetSpeed(0);
					AX_MOTOR_D_SetSpeed(0); 
					
					//���OLED����������ʾ
					AX_OLED_ClearScreen(); 
					
					//���������б���
					while(1)
					{	
						//��ʾ������ֹͣ��Ϣ
						AX_OLED_DispStr(0, 3, "  Robot has stopped  ", 0);	
						AX_OLED_DispStr(0, 5, "    Please Restart   ", 0);	
						
						AX_BEEP_On();
						vTaskDelay(10);
						AX_BEEP_Off();
							
						vTaskDelay(1500);	
						AX_OLED_ClearScreen(); 
						vTaskDelay(500);						
					}					
				}
			}
		}
		
		//ѭ������
		vTaskDelay(50);    
	}			
}


/**
  * @��  ��  ָʾ�ƴ�������
  * @��  ��  ��
  * @����ֵ  ��
  */
void Light_Task(void* parameter)
{	
	while (1)
	{	
        //��ʱ30ms��30HZ��ʾƵ��		
		vTaskDelay(30); 
		
		//�ж�ǰ���Ƿ��
		if(ax_light_enable != 0)
		{
			//������ʾ
			AX_LIGHT_Show();
		}
		else
		{
			//�ر�״̬
			AX_RGB_SetFullColor(0x00, 0x00, 0x00);
		}
	}			
}


/**
  * @��  ��  ������������
  * @��  ��  ��
  * @����ֵ  ��
  */
void Disp_Task(void* parameter)
{	
	while (1)
	{	
		//��ʱ	
		vTaskDelay(100); 
		
		//��ʾ����ģʽ
		if(ax_control_mode)
		{
			if      (ax_control_mode == CTL_PS2)    AX_OLED_DispStr(90, 2, "PS2", 0);   //PS2�ֱ�����
			else if (ax_control_mode == CTL_APP)    AX_OLED_DispStr(90, 2, "APP", 0);   //APP����
			else if (ax_control_mode == CTL_RMS)    AX_OLED_DispStr(90, 2, "RMS", 0);   //SBUS��ģң��������			
		}
		else
		{
			//ROS����ģʽ
			AX_OLED_DispStr(90, 2, "ROS", 0);
		}
		
		//��ʾ��ص�ѹ��������Z������,���ƫ��,
		AX_OLED_DispValue(30, 3, (R_Bat_Vol*0.1), 2, 1, 0);
		AX_OLED_DispValue(90, 3, (R_Imu.GYRO_Z ), 6, 0, 0);	
		
		vTaskDelay(100); 
		//��ʾ����ʵʱ�ٶ�
		AX_OLED_DispValue(30, 6, (R_Wheel_A.RT*100), 2, 2, 0);
		AX_OLED_DispValue(90, 6, (R_Wheel_B.RT*100), 2, 2, 0);
		AX_OLED_DispValue(30, 7, (R_Wheel_C.RT*100), 2, 2, 0);
		AX_OLED_DispValue(90, 7, (R_Wheel_D.RT*100), 2, 2, 0);	
	}			
}

/**
  * @��  ��  PS2���ݻ�ȡ����
  * @��  ��  ��
  * @����ֵ  ��
  */
//void Ps2_Task(void* parameter)
//{	

//	//���ڱ����ϴ�ʱ�䡣���ú�ϵͳ�Զ�����
//	static portTickType PreviousWakeTime1;

//	//������ʱʱ��20ms����ʱ��תΪ������ 
//	const portTickType TimeIncrement1 = pdMS_TO_TICKS(20);
//	
//	//��ȡ��ǰϵͳʱ�� 
//	PreviousWakeTime1 = xTaskGetTickCount();
//	
//	while(1)
//	{
//		
//		//���þ�����ʱ����20ms,ִ��Ƶ��50HZ
//		vTaskDelayUntil(&PreviousWakeTime1, TimeIncrement1 );
//		
//		//��ȡPS2�ֱ���ֵ
////		AX_PS2_ScanKey(&my_joystick);
//		
//		//����PS2����ģʽ��
//		if(ax_control_mode != CTL_PS2)
//		{
//			//�ж��Ƿ���PS2�ֱ�����
//			//START���������º����ҡ�����ƣ�����PS2����ģʽ
//			if((my_joystick.btn1 == PS2_BT1_START) && (my_joystick.LJoy_UD == 0x00))
//			{
//				//�л���PS2ģʽ
//				ax_control_mode = CTL_PS2;	

//				//ִ�з�����������ʾ
//				ax_beep_ring = BEEP_SHORT;
//			}
//		}
//		
////		//��ӡ�ֱ���ֵ
////		printf("MODE:%2x BTN1:%2x BTN2:%2x RJOY_LR:%2x RJOY_UD:%2x LJOY_LR:%2x LJOY_UD:%2x\r\n",
////		my_joystick.mode, my_joystick.btn1, my_joystick.btn2, 
////		my_joystick.RJoy_LR, my_joystick.RJoy_UD, my_joystick.LJoy_LR, my_joystick.LJoy_UD);
//	}
//}

/**
  * @brief ESP8266ͨ�����������
  * @param Parameter: FreeRTOS�������
  * @retval ��
  */
void ESP8266_Task(void *Parameter)
{
    // ��ʼ����Ϣ
    USART3_SendString("\r\n[ESP8266] Communication Task Started!\r\n");
    
    uint8_t rxByte = 0;
    TickType_t lastActionTime = 0;
    const TickType_t actionCooldown = pdMS_TO_TICKS(500); // ������ȴʱ��(ms)
    
    while (1)
    {
        // ���USART1�Ƿ������ݣ�����ESP8266��
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
        {
            rxByte = USART_ReceiveData(USART1);
            
            // ��ӡԭʼ��������
            char rawMsg[50];
            snprintf(rawMsg, sizeof(rawMsg), "[ESP8266] Raw: 0x%02X\r\n", rxByte);
            USART3_SendString(rawMsg);
            
            TickType_t currentTime = xTaskGetTickCount();
            
            // ������Чresetֵ (0,1,2,3) �������ȴʱ��
            if (rxByte <= RESET_3)
            {
                if (currentTime - lastActionTime >= actionCooldown)
                {
                    lastActionTime = currentTime;
                    
                    // ��ӡ���յ���resetֵ
                    char rxMsg[50];
                    snprintf(rxMsg, sizeof(rxMsg), "Received reset: %d\r\n", rxByte);
                    USART3_SendString(rxMsg);
                    
                    // ����reset����
                    processReset(rxByte);
                    
                    // ���ͽ����ESP8266
                    sendToESP8266(rxByte); // ����ִ�гɹ�������ֵͬ
                }
                else
                {
                    USART3_SendString("[WARN] Action too frequent! Ignored.\r\n");
                }
            }
            else
            {
                // ��ӡ��Ч����
                char errMsg[50];
                snprintf(errMsg, sizeof(errMsg), "[ERROR] Invalid reset: %d\r\n", rxByte);
                USART3_SendString(errMsg);
            }
        }
        
        // ���������ʱ
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
  * @brief ������յ���reset����
  * @param reset_value: resetֵ (0,1,2,3)
  */
void processReset(uint8_t reset_value)
{
    char msg[50];
    
    switch (reset_value)
    {
        case RESET_0:
            snprintf(msg, sizeof(msg), "Processing RESET_0 action\r\n");
            AX_SERVO_INIT(); // ִ�и�λ����
            break;
            
        case RESET_1:
            snprintf(msg, sizeof(msg), "Processing RESET_1 action\r\n");
            //AX_SERVO_up(); // ִ��ץȡ����
						AX_SERVO_Grab();
            break;
            
        case RESET_2:
            snprintf(msg, sizeof(msg), "Processing RESET_2 action\r\n");
            //AX_SERVO_down(); // ִ�з��ö���
						AX_SERVO_Release();
            break;
            
        case RESET_3:
            snprintf(msg, sizeof(msg), "Processing RESET_3 action\r\n");
            //AX_SERVO_custom(); // ִ���Զ��嶯��
            break;
            
        default:
            snprintf(msg, sizeof(msg), "[ERROR] Unknown reset value: %d\r\n", reset_value);
            break;
    }
    
    // ���ʹ�����Ϣ�����Դ���
    USART3_SendString(msg);
}

/**
  * @brief ����reset�����ESP8266
  * @param result: ִ�н�� (0,1,2,3)
  */
void sendToESP8266(uint8_t result)
{
    // ��ӷ��ͼ������
    static TickType_t lastSendTime = 0;
    TickType_t currentTime = xTaskGetTickCount();
    const TickType_t minSendInterval = pdMS_TO_TICKS(200); // ��С���ͼ��
    
    if (currentTime - lastSendTime < minSendInterval) {
        return;
    }
    lastSendTime = currentTime;
    
    // ���͵����ֽڽ��
    USART_SendData(USART1, result);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    
    // ��ӡ������Ϣ�����Դ���
    char txMsg[50];
    snprintf(txMsg, sizeof(txMsg), "Sent reset result: %d to ESP8266\r\n", result);
    USART3_SendString(txMsg);
}



void GPS_Task(void *pvParameters)
{
	u16 i,rxlen;
  u8 upload=1;
//	 printf("Hello\r\n");
		while(1) 
	{	
   
//    delay_ms(1000);		
//		delay_ms(1);
		if(UART5_RX_STA&0X8000)		//���յ�һ�������� ��15λ
		{
			rxlen=UART5_RX_STA&0X7FFF;	//�õ����ݳ��� 0��14λ
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=UART5_RX_BUF[i];	   
 			UART5_RX_STA=0;		   	//������һ�ν���
			USART1_TX_BUF[i]=0;			//�Զ���ӽ�����
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//�����ַ���
			if(upload)printf("\r\n%s\r\n",USART1_TX_BUF);//���ͽ��յ������ݵ�����1
 		}
	  vTaskDelay(pdMS_TO_TICKS(200));

}
	}

void OPENMV_Task(void *pvParameters)
{
	while(1)
	{
		 //printf("[OpenMV] Distance: %d mm, X: %d, Y: %d",
      //             distance, x_coordinate, y_coordinate);

		vTaskDelay(pdMS_TO_TICKS(300));
	
	}		

}


void SERVO_Task(void *pvParameters)
{
	while(1)
	{
		switch(servo_flag)
		{
			case 0x01:
				//AX_SERVO_up();
			  break;
		  case 0x02:
				//AX_SERVO_down();
			  break;
			default:
        break;				
		}
		servo_flag = 0;
	}
}





///**
//  * @brief SysTick�жϷ�����
//  */
//void SysTick_Handler(void)
//{
//    systemTime++;
//}

///**
//  * @brief ��ȡϵͳʱ���(����)
//  * @retval ��ǰϵͳʱ��(����)
//  */
//uint32_t GetSystemTime(void)
//{
//    return systemTime;
//}

/******************* (C) ��Ȩ 2023 XTARK **************************************/

