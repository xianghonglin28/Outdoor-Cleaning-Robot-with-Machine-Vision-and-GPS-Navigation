#ifndef __AX_ROBOT_H
#define __AX_ROBOT_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//C�⺯�������ͷ�ļ�
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//FreeRTOSͷ�ļ�
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
 
//�������ͷ�ļ�
#include "ax_sys.h"      //ϵͳ����
#include "ax_delay.h"    //�����ʱ
#include "ax_led.h"      //LED�ƿ���
#include "ax_beep.h"     //����������
#include "ax_uart1.h"    //���Դ���
#include "ax_vin.h"      //�����ѹ���
#include "ax_key.h"      //������� 
#include "ax_switch.h"   //������� 

#include "ax_uart1.h"    //���Դ���
#include "ax_uart2.h"    //TTL���ڴ���
#include "ax_uart3.h"    //USBͨ�Ŵ���
#include "ax_uart4.h"    //��������
#include "ax_uart5.h"

#include "ax_servo.h"    //�������
#include "ax_motor.h"    //ֱ��������ٿ���
#include "ax_encoder.h"  //����������

#include "ax_eeprom.h"   //EEPROM�洢
#include "ax_mpu6050.h"  //IMU���ٶ������ǲ���
#include "ax_can.h"      //CAN����

#include "ax_oled.h"     //OLED��ʾ

#include "ax_akm.h"      //�����������˴�����


//�����������ٶ����ݽṹ��
typedef struct  
{
	double  RT;       //����ʵʱ�ٶȣ���λm/s
	float  TG;       //����Ŀ���ٶȣ���λm/s
	short  PWM;      //����PWM�����ٶ�
	
}ROBOT_Wheel;

//�������ٶȽṹ��
typedef struct  
{
	short  RT_IX;     //ʵʱX���ٶȣ�16λ������
	short  RT_IY;     //ʵʱY���ٶȣ�16λ������
	short  RT_IW;     //ʵʱYaw��ת���ٶȣ�16λ������
	
	short  TG_IX;     //Ŀ��X���ٶȣ�16λ������
	short  TG_IY;     //Ŀ��Y���ٶȣ�16λ������
	short  TG_IW;     //Ŀ��Yaw��ת���ٶȣ�16λ������
	
	float  RT_FX;     //ʵʱX���ٶȣ����㣩
	float  RT_FY;     //ʵʱY���ٶȣ����㣩
	float  RT_FW;     //ʵʱYaw��ת���ٶȣ����㣩
	
	float  TG_FX;     //Ŀ��X���ٶȣ����㣩
	float  TG_FY;     //Ŀ��Y���ٶȣ����㣩
	float  TG_FW;     //Ŀ��Yaw��ת���ٶȣ����㣩
	
}ROBOT_Velocity;


//������IMU����
typedef struct  
{
	short  ACC_X;     //X��
	short  ACC_Y;     //Y��
	short  ACC_Z;     //Z��
	
	short  GYRO_X;     //X��
	short  GYRO_Y;     //Y��
	short  GYRO_Z;     //Z��
	
}ROBOT_Imu;


typedef struct  
{
	uint8_t  M;    //��Ч��ģʽ
	uint8_t  S;    //��Ч��ģʽ
	uint8_t  T;    //��Чʱ�����
	
	uint8_t  R;     //��Ч��ɫ R
	uint8_t  G;     //��Ч��ɫ G
	uint8_t  B;     //��Ч��ɫ B
	
}ROBOT_Light;

//����
#define  PI           3.1416     //Բ����PI
#define  SQRT3        1.732      //3��ƽ��
#define  PID_RATE     50         //PIDƵ��

//���
#define  VBAT_40P    1065     //���40%��ѹ
#define  VBAT_20P    1012     //���20%��ѹ
#define  VBAT_10P    984      //���10%��ѹ

//����������汾
#define  ROBOT_FW_VER   "V3.02"

/******����������***********************************/

//���������Ͷ���
#define ROBOT_MEC   0x01   //�����ķ�ֵ���
#define ROBOT_FWD   0x02   //���ֲ��ٵ���
#define ROBOT_AKM   0x03   //������ת�����
#define ROBOT_TWD   0x04   //���ֲ���ת����� 
#define ROBOT_TNK   0x05   //�Ĵ�̹�˵���
#define ROBOT_OMT   0x06   //����ȫ�����

//�����˶�������
#define ROBOT_TYPE   ROBOT_MEC

/******�����˲���*************************************/

//���ֻ����˲���
#define  MEC_WHEEL_BASE           0.196	      //�־࣬�����ֵľ���
#define  MEC_ACLE_BASE            0.160       //��࣬ǰ���ֵľ���
#define  MEC_WHEEL_DIAMETER	      0.098		  //����ֱ��
#define  MEC_WHEEL_RESOLUTION     1560.0      //�������ֱ���(13��),���ٱ�30,13x30x4=1560
#define  MEC_WHEEL_SCALE          (PI*MEC_WHEEL_DIAMETER*PID_RATE/MEC_WHEEL_RESOLUTION) //�����ٶ�m/s�������ת��ϵ��

//���ֲ��ٻ����˲���
//������̥Ħ�����ϴ󣬲���60���ٱȵ�������ת����30���ٱ��෴
#define  FWD_WHEEL_BASE           0.200	     //�־࣬�����ֵľ���
#define  FWD_WB_SCALE             1.75       //�־�ϵ�����־�ϵ��������˵��ܸ��ء���̥���������Ħ��ϵ����ת��뾶������λ�ö����й�ϵ��һ���ǳ����ӵĲ��������Գ��õķ���������ʵ��
#define  FWD_WHEEL_DIAMETER	      0.100		 //����ֱ��
#define  FWD_WHEEL_RESOLUTION     3120.0     //�������ֱ���(13��),���ٱ�60,13x60x4=3120
#define  FWD_WHEEL_SCALE          (PI*FWD_WHEEL_DIAMETER*PID_RATE/FWD_WHEEL_RESOLUTION) //�����ٶ�m/s�������ת��ϵ��

//�����������˲���
#define  AKM_WHEEL_BASE           0.165	     //�־࣬�����ֵľ���
#define  AKM_ACLE_BASE            0.175f     //��࣬ǰ���ֵľ���
#define  AKM_WHEEL_DIAMETER	      0.075		 //����ֱ��
#define  AKM_WHEEL_RESOLUTION     1560.0      //�������ֱ���(13��),���ٱ�30,13x30x4=1560
#define  AKM_TURN_R_MINI          0.35f      //��Сת��뾶( L*cot30-W/2)
#define  AKM_WHEEL_SCALE          (PI*AKM_WHEEL_DIAMETER*PID_RATE/AKM_WHEEL_RESOLUTION)  //�����ٶ�m/s�������ת��ϵ��

//���ֲ��ٻ����˲��� 
#define  TWD_WHEEL_DIAMETER	      0.0724	 //����ֱ��
#define  TWD_WHEEL_BASE           0.206	     //�־࣬�����ֵľ���
#define  TWD_WHEEL_RESOLUTION     1560.0      //�������ֱ���(13��),���ٱ�30,13x30x4=1560
#define  TWD_WHEEL_SCALE          (PI*TWD_WHEEL_DIAMETER*PID_RATE/TWD_WHEEL_RESOLUTION)  //�����ٶ�m/s�������ת��ϵ��

//�Ĵ�ʽ�����˲��� 
#define  TNK_WHEEL_DIAMETER	      0.0724	 //����ֱ��
#define  TNK_WHEEL_BASE           0.206	     //�־࣬�����ֵľ���
#define  TNK_WHEEL_RESOLUTION     1560.0      //�������ֱ���(13��),���ٱ�30,13x30x4=1560
#define  TNK_WHEEL_SCALE          (PI*TWD_WHEEL_DIAMETER*PID_RATE/TNK_WHEEL_RESOLUTION)  //�����ٶ�m/s�������ת��ϵ��

//����ȫ������˲��� 
#define  OMT_WHEEL_DIAMETER	      0.058	     //����ֱ��
#define  OMT_WHEEL_RADIUS         0.206	     //�����˰뾶����������������ľ���
#define  OMT_WHEEL_RESOLUTION     1560.0     //�������ֱ���(13��),���ٱ�30,13x30x4=1560
#define  OMT_WHEEL_SCALE          (PI*TWD_WHEEL_DIAMETER*PID_RATE/OMT_WHEEL_RESOLUTION)  //�����ٶ�m/s�������ת��ϵ��

/******������ͨ��Э��*************************************/

//����ͨ��֡ͷ����
#define  ID_UTX_DATA    0x10    //���͵��ۺ�����
#define  ID_URX_VEL     0x50    //���յ��ٶ�����
#define  ID_URX_IMU     0x51    //���յ�IMU��������ƫУ׼ָ��
#define  ID_URX_LG      0x52    //���յĵƹ����ָ��
#define  ID_URX_LS      0x53    //���յĵƹ�Ч������ָ��
#define  ID_URX_BP      0x54    //���շ���������ָ��
#define  ID_URX_RTY     0x5A    //���յĻ������ͺ�ѡ��

//����APPͨ��֡ͷ����
#define  ID_BLERX_CM      0x30    //APP�������� ��������ָ��
#define  ID_BLERX_YG      0x31    //APP�������� ҡ��ģʽ ����ָ��
#define  ID_BLERX_SB      0x32    //APP�������� �ֱ�ģʽ ����ָ��
#define  ID_BLERX_ZL      0x33    //APP�������� ����ģʽ ����ָ��
#define  ID_BLERX_TK      0x34    //APP�������� ̹��ģʽ ����ָ��
#define  ID_BLERX_AM      0x3A    //APP�������� ��е��ģʽ ����ָ��
#define  ID_BLERX_LG      0x41    //APP�������� �ƹ����ָ��
#define  ID_BLERX_LS      0x42    //APP�������� ����ƹ�Ч��ָ��

//CANͨ��֡ͷ����
#define  ID_CANRX_VEL     0x0181    //���յ�CAN�ٶ�����
#define  ID_CANTX_MSG1    0x0121    //���͵Ļ���������1
#define  ID_CANTX_MSG2    0x0122    //���͵Ļ���������2
#define  ID_CANTX_MSG3    0x0123    //���͵Ļ���������3

//�������ٶ�����
#define R_VX_LIMIT  1500   //X���ٶ���ֵ m/s*1000
#define R_VY_LIMIT  1200   //Y���ٶ���ֵ m/s*1000
#define R_VW_LIMIT  6280   //W��ת���ٶ���ֵ rad/s*1000

//�ƹ�ģʽ
#define  LEFFECT1    0x01    //��ɫģʽ  
#define  LEFFECT2    0x02    //����ģʽ  
#define  LEFFECT3    0x03    //��ɫģʽ  
#define  LEFFECT4    0x04    //����ģʽ 
#define  LEFFECT5    0x05    //�۾�ģʽ  
#define  LEFFECT6    0x06    //�����ģʽ

//�����˿��Ʒ�ʽ
#define  CTL_ROS    0x00    //ROS���ƣ��������ں�CAN
#define  CTL_PS2    0x01    //PS2�ֱ�����
#define  CTL_APP    0x02    //APP����
#define  CTL_RMS    0x03    //SBUS��ģң��������	

//������������
#define  BEEP_SHORT   0x01    //������������һ��(200ms)
#define  BEEP_LONG    0x02    //������������һ��(1000ms)

//�����˹ؼ�ȫ�ֱ���
extern  ROBOT_Velocity  R_Vel; //�������ٶ�����
extern  ROBOT_Wheel  R_Wheel_A,R_Wheel_B,R_Wheel_C,R_Wheel_D; //��������������
extern  ROBOT_Imu  R_Imu; //������IMU����
extern  ROBOT_Light  R_Light; //������RGB����
extern  uint16_t R_Bat_Vol;  //�����˵�ص�ѹ����

//ȫ�ֱ���
extern int16_t ax_imu_acc_data[3];  
extern int16_t ax_imu_gyro_data[3];
extern int16_t ax_imu_gyro_offset[3];   

extern int16_t ax_motor_kp;  
extern int16_t ax_motor_ki;    
extern int16_t ax_motor_kd; 

extern int8_t ax_imu_calibrate_flag;
extern int8_t ax_uart_flag;
extern int8_t ax_model_flag;
extern uint8_t ax_light_enable;
extern uint8_t ax_light_save_flag;
extern uint8_t ax_beep_ring;  
extern uint8_t ax_beep_enable;  
extern uint8_t ax_control_mode;


extern volatile uint8_t servo_flag; 

////PS2�ֱ���ֵ�ṹ��
//extern JOYSTICK_TypeDef my_joystick;  

//������������
extern int16_t ax_akm_offset;
extern int16_t ax_akm_angle;



//������
extern TaskHandle_t Robot_Task_Handle;
extern TaskHandle_t ESP8266_Task_Handle;
extern TaskHandle_t Key_Task_Handle;
extern TaskHandle_t Disp_Task_Handle;
extern TaskHandle_t Trivia_Task_Handle;
extern TaskHandle_t GPS_Task_Handle;
extern TaskHandle_t OPENMV_Task_Handle;
// ����������
extern SemaphoreHandle_t arm_state_mutex;



#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "ax_uart1.h" 
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

// ��������
#define RESET_0 0
#define RESET_1 1
#define RESET_2 2
#define RESET_3 3


/* �������� */
void USART_Configuration(void);

void processReset(uint8_t reset_value);
void sendToESP8266(uint8_t action);
void ESP8266_Task(void *Parameter);

// ȫ�ֱ���
extern uint8_t uart1_rx_ok;
extern uint8_t uart1_rx_buf[40];

#endif
