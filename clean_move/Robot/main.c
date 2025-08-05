#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include <stdio.h>
#include "ax_robot.h"
#include "gps.h"
#include "ax_oled_chinese.h" //OLED���ֿ�
#include "ax_oled_picture.h" //OLED ͼƬ��


//������
//��������
#define START_TASK_PRIO		1
#define START_STK_SIZE 		128  
TaskHandle_t StartTask_Handler = NULL;
void Start_Task(void *pvParameters);


//ESP8266ͨ�����������
#define ESP8266_TASK_PRIO		3     
#define ESP8266_STK_SIZE 		512 
TaskHandle_t ESP8266_Task_Handle = NULL;
void ESP8266_Task(void *pvParameters);

//GPSģ���ӡ����
#define GPS_TASK_PRIO		1     
#define GPS_STK_SIZE 		256 
TaskHandle_t GPS_Task_Handle = NULL;
void GPS_Task(void *pvParameters);

//OPENMVģ���ӡ����
#define OPENMV_TASK_PRIO		3     
#define OPENMV_STK_SIZE 		256 
TaskHandle_t OPENMV_Task_Handle = NULL;
void OPENMV_Task(void *pvParameters);

//����������������
#define ROBOT_TASK_PRIO		3     
#define ROBOT_STK_SIZE 		256 
TaskHandle_t Robot_Task_Handle = NULL;
void Robot_Task(void *pvParameters);

//������������
#define KEY_TASK_PRIO		4     
#define KEY_STK_SIZE 		128   
TaskHandle_t Key_Task_Handle = NULL;
void Key_Task(void *pvParameters);

//OLED��ʾ����
#define DISP_TASK_PRIO		6     
#define DISP_STK_SIZE 		128   
TaskHandle_t Disp_Task_Handle = NULL;
void Disp_Task(void *pvParameters);

//���¹�������
#define TRIVIA_TASK_PRIO		10     
#define TRIVIA_STK_SIZE 		128   
TaskHandle_t Trivia_Task_Handle = NULL;
void Trivia_Task(void *pvParameters);

//�����������
#define SERVO_TASK_PRIO		1     
#define SERVO_STK_SIZE 		128 
TaskHandle_t SERVO_Task_Handle = NULL;
void SERVO_Task(void *pvParameters);

/**
  * @��  ��  ����������
  * @��  ��  ��
  * @����ֵ  ��
  */
int main(void)
{	
	//�����ж����ȼ�����
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	
	//��ʱ������ʼ��
	AX_DELAY_Init();
	
	
	//���Դ��ڳ�ʼ��
	AX_UART3_Init(115200);
	
  //gpsģ�鴮�ڳ�ʼ��	
	uart5_init(115200);
	
	//����ӿڳ�ʼ��
	AX_SERVO_S1_Init();
	AX_SERVO_S2_Init();
	AX_SERVO_S3456_Init();
	
	//�����ʼ����Ƶ��20KHZ
	AX_MOTOR_AB_Init();
	AX_MOTOR_CD_Init();  
	
	//LED��ʼ��
	AX_LED_Init();  
	
	//KEY��������ʼ��
	AX_KEY_Init();
	
	//��ص�ѹ����ʼ��
	AX_VIN_Init();
	
	//��������ʼ��
	AX_BEEP_Init();  
	
	//EEPROM��ʼ��
	AX_EEPROM_Init();  
	
	//esp8266��ʼ��
	AX_UART1_Init(115200);
	
	// ��ʼ�������Ϣ
  USART3_SendString("\r\nSTM32F407VET6 Initialized!\r\n");
  USART3_SendString("USART1: 115200 baud, 8N1\r\n");
  USART3_SendString("Waiting for ESP8266 commands...\r\n");
	
	//����ͷ��ʼ��
	UART2_Init();

	//�������ڳ�ʼ��
	AX_UART4_Init(115200);
	
	//CAN��ʼ��
	AX_CAN_Init();
	
	//��������ʼ����
	AX_ENCODER_A_Init();  
	AX_ENCODER_B_Init();  
	AX_ENCODER_C_Init();  
	AX_ENCODER_D_Init(); 
	
	//OLED��Ļ��ʼ��
	AX_OLED_Init();	
	AX_OLED_DispPicture(0, 0, 128, 8, PIC64X128_XTARK, 0); 
	
	//���뿪�س�ʼ������ʱ����
	AX_SW_Init();
	
	
	//������ʾ��Ϣ
	AX_BEEP_On();
	AX_Delayms(100);	
	AX_BEEP_Off();
	AX_Delayms(1900);
	
    //MPU6050��ʼ��
	AX_MPU6050_Init();      
	AX_MPU6050_SetAccRange(AX_ACC_RANGE_2G);    //���ü��ٶ�����
	AX_MPU6050_SetGyroRange(AX_GYRO_RANGE_500); //��������������
	AX_MPU6050_SetGyroSmplRate(200);            //���������ǲ�����
	AX_MPU6050_SetDLPF(AX_DLPF_ACC94_GYRO98);   //���õ�ͨ�˲�������
	
	//����AppTaskCreate����
	xTaskCreate((TaskFunction_t )Start_Task,  /* ������ں��� */
								 (const char*    )"Start_Task",/* �������� */
								 (uint16_t       )START_STK_SIZE,  /* ����ջ��С */
								 (void*          )NULL,/* ������ں������� */
								 (UBaseType_t    )START_TASK_PRIO, /* ��������ȼ� */
								 (TaskHandle_t*  )&StartTask_Handler);/* ������ƿ�ָ�� */ 
	
	// �������
  printf("System Start...\n");
  printf("UART1 Test Message\n");
								 
	//�������񣬿�������						 
	vTaskStartScheduler(); 

	//ѭ��
	while (1);

	}


/**
  * @��  ��  ����������
  * @��  ��  ��
  * @����ֵ  ��
  */
void Start_Task(void *pvParameters)
{	
	uint8_t cnt;
	uint8_t temp = 0x55;
	
	//������У׼����
	int16_t gyro_data[3]; 
	
	/******��������������************************************************/
	
	//��ȡ������EEPROM������������
	if(AX_EEPROM_ReadOneByte(0x00) == 0x88)
	{

	}
	else  
	{
		
		AX_SERVO_INIT();

		//��ǲ����ѳ�ʼ����
		AX_EEPROM_WriteOneByte(0x00,0x88);  
		
	}
	
	//���������У׼
	for(int i=0; i<10; i++) 
	{
		//�����˸��ָʾ������У׼
		AX_LED_Green_On();
		vTaskDelay(30); 
		
		AX_LED_Green_Off();
		vTaskDelay(20); 

		//��ȡPMU6050����������
        AX_MPU6050_GetGyroData(gyro_data);
		
		ax_imu_gyro_offset[0] += gyro_data[0];
		ax_imu_gyro_offset[1] += gyro_data[1];
		ax_imu_gyro_offset[2] += gyro_data[2]; 		
	}
	
	//����ƽ��ƫ��ֵ
	ax_imu_gyro_offset[0] = -ax_imu_gyro_offset[0]/10;
	ax_imu_gyro_offset[1] = -ax_imu_gyro_offset[1]/10;
	ax_imu_gyro_offset[2] = -ax_imu_gyro_offset[2]/10;	
	

	//�����ȴ���չʾ�ȴ���Ч
	for(int i=0; i<3; i++) 
	{
		//����Ч��, 1S 
		for(cnt= 0; cnt<30; cnt++)
		{
			 temp = cnt*8;
			 AX_LED_Green_On();
			 vTaskDelay(20); 
		}
		for(cnt= 20; cnt>0; cnt--)
		{
			 temp = cnt*12;
			 AX_LED_Green_Off();
			  vTaskDelay(20); 
		}		
	}
	
	
	//����������ɣ��̵Ƶ�������������ʾ
	AX_LED_Green_On();	
	AX_BEEP_On();
	AX_Delayms(100);	
	AX_BEEP_Off();	
	
	//��ʾ�����ڽ���
	AX_OLED_ClearScreen();  //���OLED����������ʾ
	AX_OLED_DispStr(0, 0, "   * TARKBOT MEC *   ", 0);	
	AX_OLED_DispStr(0, 1, "---------------------", 0);	

	AX_OLED_DispStr(0, 2, " Ver:V0.00 Mod:ROS   ", 0);
	AX_OLED_DispStr(30, 2, ROBOT_FW_VER, 0);		
	AX_OLED_DispStr(0, 3, " Vol:12.2V Gyz:00000 ", 0);
	AX_OLED_DispStr(0, 4, "---------------------", 0);	
	AX_OLED_DispStr(0, 5, "---------------------", 0);	
	AX_OLED_DispStr(0, 6, " MTA:00.00 MTB:-0.00 ", 0);
	AX_OLED_DispStr(0, 7, " MTC:00.00 MTD:-0.00 ", 0);
	
	//���ݺ궨�壬��ʾ��������
	#if (ROBOT_TYPE == ROBOT_MEC)
	AX_OLED_DispStr(78, 0, "MEC", 0);	
	#endif
	
	//�����ٽ���
	taskENTER_CRITICAL();   

	//���������˿�������
	xTaskCreate((TaskFunction_t )Robot_Task, /* ������ں��� */
			 (const char*    )"Robot_Task",/* �������� */
			 (uint16_t       )ROBOT_STK_SIZE,   /* ����ջ��С */
			 (void*          )NULL,	/* ������ں������� */
			 (UBaseType_t    )ROBOT_TASK_PRIO,	    /* ��������ȼ� */
			 (TaskHandle_t*  )&Robot_Task_Handle);/* ������ƿ�ָ�� */

  //����ESP8266ͨ�����������
	xTaskCreate((TaskFunction_t )ESP8266_Task, /* ������ں��� */
			 (const char*    )"ESP8266_Task",/* �������� */
			 (uint16_t       )ESP8266_STK_SIZE,   /* ����ջ��С */
			 (void*          )NULL,	/* ������ں������� */
			 (UBaseType_t    )ESP8266_TASK_PRIO,	    /* ��������ȼ� */
			 (TaskHandle_t*  )&ESP8266_Task_Handle);/* ������ƿ�ָ�� */
			 
	//����������������
	xTaskCreate((TaskFunction_t )Key_Task, /* ������ں��� */
			 (const char*    )"Key_Task",/* �������� */
			 (uint16_t       )KEY_STK_SIZE,   /* ����ջ��С */
			 (void*          )NULL,	/* ������ں������� */
			 (UBaseType_t    )KEY_TASK_PRIO,	    /* ��������ȼ� */
			 (TaskHandle_t*  )&Key_Task_Handle);/* ������ƿ�ָ�� */
			 
	//OLED����ʾ����
	xTaskCreate((TaskFunction_t )Disp_Task, /* ������ں��� */
			 (const char*    )"Disp_Task",/* �������� */
			 (uint16_t       )DISP_STK_SIZE,   /* ����ջ��С */
			 (void*          )NULL,	/* ������ں������� */
			 (UBaseType_t    )DISP_TASK_PRIO,	    /* ��������ȼ� */
			 (TaskHandle_t*  )&Disp_Task_Handle);/* ������ƿ�ָ�� */	
			 
	//���¹�������
	xTaskCreate((TaskFunction_t )Trivia_Task, /* ������ں��� */
			 (const char*    )"Trivia_Task",/* �������� */
			 (uint16_t       )TRIVIA_STK_SIZE,   /* ����ջ��С */
			 (void*          )NULL,	/* ������ں������� */
			 (UBaseType_t    )TRIVIA_TASK_PRIO,	    /* ��������ȼ� */
			 (TaskHandle_t*  )&Trivia_Task_Handle);/* ������ƿ�ָ�� */			 
						  
	xTaskCreate((TaskFunction_t )GPS_Task, /* ������ں��� */
			 (const char*    )"GPS_Task",/* �������� */
			 (uint16_t       )GPS_STK_SIZE,   /* ����ջ��С */
			 (void*          )NULL,	/* ������ں������� */
			 (UBaseType_t    )GPS_TASK_PRIO,	    /* ��������ȼ� */
			 (TaskHandle_t*  )&GPS_Task_Handle);/* ������ƿ�ָ�� */	
			 
	xTaskCreate((TaskFunction_t )OPENMV_Task, /* ������ں��� */
			 (const char*    )"OPENMV_Task",/* �������� */
			 (uint16_t       )OPENMV_STK_SIZE,   /* ����ջ��С */
			 (void*          )NULL,	/* ������ں������� */
			 (UBaseType_t    )OPENMV_TASK_PRIO,	    /* ��������ȼ� */
			 (TaskHandle_t*  )&OPENMV_Task_Handle);/* ������ƿ�ָ�� */
			 
  xTaskCreate((TaskFunction_t )SERVO_Task, /* ������ں��� */
			 (const char*    )"SERVO_Task",/* �������� */
			 (uint16_t       )SERVO_STK_SIZE,   /* ����ջ��С */
			 (void*          )NULL,	/* ������ں������� */
			 (UBaseType_t    )SERVO_TASK_PRIO,	    /* ��������ȼ� */
			 (TaskHandle_t*  )&SERVO_Task_Handle);/* ������ƿ�ָ�� */	 
	//ɾ��AppTaskCreate����				
	vTaskDelete(StartTask_Handler); 

	//�˳��ٽ���
	taskEXIT_CRITICAL();           
						 							
}
