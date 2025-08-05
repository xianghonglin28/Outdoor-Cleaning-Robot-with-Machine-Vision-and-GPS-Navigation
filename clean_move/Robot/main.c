#include "stm32f4xx.h"
#include "stm32f4xx_it.h"
#include <stdio.h>
#include "ax_robot.h"
#include "gps.h"
#include "ax_oled_chinese.h" //OLED汉字库
#include "ax_oled_picture.h" //OLED 图片库


//任务句柄
//启动任务
#define START_TASK_PRIO		1
#define START_STK_SIZE 		128  
TaskHandle_t StartTask_Handler = NULL;
void Start_Task(void *pvParameters);


//ESP8266通信与控制任务
#define ESP8266_TASK_PRIO		3     
#define ESP8266_STK_SIZE 		512 
TaskHandle_t ESP8266_Task_Handle = NULL;
void ESP8266_Task(void *pvParameters);

//GPS模块打印数据
#define GPS_TASK_PRIO		1     
#define GPS_STK_SIZE 		256 
TaskHandle_t GPS_Task_Handle = NULL;
void GPS_Task(void *pvParameters);

//OPENMV模块打印数据
#define OPENMV_TASK_PRIO		3     
#define OPENMV_STK_SIZE 		256 
TaskHandle_t OPENMV_Task_Handle = NULL;
void OPENMV_Task(void *pvParameters);

//机器人运行主任务
#define ROBOT_TASK_PRIO		3     
#define ROBOT_STK_SIZE 		256 
TaskHandle_t Robot_Task_Handle = NULL;
void Robot_Task(void *pvParameters);

//按键处理任务
#define KEY_TASK_PRIO		4     
#define KEY_STK_SIZE 		128   
TaskHandle_t Key_Task_Handle = NULL;
void Key_Task(void *pvParameters);

//OLED显示任务
#define DISP_TASK_PRIO		6     
#define DISP_STK_SIZE 		128   
TaskHandle_t Disp_Task_Handle = NULL;
void Disp_Task(void *pvParameters);

//琐事管理任务
#define TRIVIA_TASK_PRIO		10     
#define TRIVIA_STK_SIZE 		128   
TaskHandle_t Trivia_Task_Handle = NULL;
void Trivia_Task(void *pvParameters);

//舵机控制任务
#define SERVO_TASK_PRIO		1     
#define SERVO_STK_SIZE 		128 
TaskHandle_t SERVO_Task_Handle = NULL;
void SERVO_Task(void *pvParameters);

/**
  * @简  述  程序主函数
  * @参  数  无
  * @返回值  无
  */
int main(void)
{	
	//设置中断优先级分组
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);  
	
	//延时函数初始化
	AX_DELAY_Init();
	
	
	//调试串口初始化
	AX_UART3_Init(115200);
	
  //gps模块串口初始化	
	uart5_init(115200);
	
	//舵机接口初始化
	AX_SERVO_S1_Init();
	AX_SERVO_S2_Init();
	AX_SERVO_S3456_Init();
	
	//电机初始化，频率20KHZ
	AX_MOTOR_AB_Init();
	AX_MOTOR_CD_Init();  
	
	//LED初始化
	AX_LED_Init();  
	
	//KEY按键检测初始化
	AX_KEY_Init();
	
	//电池电压检测初始化
	AX_VIN_Init();
	
	//蜂鸣器初始化
	AX_BEEP_Init();  
	
	//EEPROM初始化
	AX_EEPROM_Init();  
	
	//esp8266初始化
	AX_UART1_Init(115200);
	
	// 初始化完成消息
  USART3_SendString("\r\nSTM32F407VET6 Initialized!\r\n");
  USART3_SendString("USART1: 115200 baud, 8N1\r\n");
  USART3_SendString("Waiting for ESP8266 commands...\r\n");
	
	//摄像头初始化
	UART2_Init();

	//蓝牙串口初始化
	AX_UART4_Init(115200);
	
	//CAN初始化
	AX_CAN_Init();
	
	//编码器初始化，
	AX_ENCODER_A_Init();  
	AX_ENCODER_B_Init();  
	AX_ENCODER_C_Init();  
	AX_ENCODER_D_Init(); 
	
	//OLED屏幕初始化
	AX_OLED_Init();	
	AX_OLED_DispPicture(0, 0, 128, 8, PIC64X128_XTARK, 0); 
	
	//拨码开关初始化，暂时不用
	AX_SW_Init();
	
	
	//开机提示信息
	AX_BEEP_On();
	AX_Delayms(100);	
	AX_BEEP_Off();
	AX_Delayms(1900);
	
    //MPU6050初始化
	AX_MPU6050_Init();      
	AX_MPU6050_SetAccRange(AX_ACC_RANGE_2G);    //设置加速度量程
	AX_MPU6050_SetGyroRange(AX_GYRO_RANGE_500); //设置陀螺仪量程
	AX_MPU6050_SetGyroSmplRate(200);            //设置陀螺仪采样率
	AX_MPU6050_SetDLPF(AX_DLPF_ACC94_GYRO98);   //设置低通滤波器带宽
	
	//创建AppTaskCreate任务
	xTaskCreate((TaskFunction_t )Start_Task,  /* 任务入口函数 */
								 (const char*    )"Start_Task",/* 任务名字 */
								 (uint16_t       )START_STK_SIZE,  /* 任务栈大小 */
								 (void*          )NULL,/* 任务入口函数参数 */
								 (UBaseType_t    )START_TASK_PRIO, /* 任务的优先级 */
								 (TaskHandle_t*  )&StartTask_Handler);/* 任务控制块指针 */ 
	
	// 调试输出
  printf("System Start...\n");
  printf("UART1 Test Message\n");
								 
	//启动任务，开启调度						 
	vTaskStartScheduler(); 

	//循环
	while (1);

	}


/**
  * @简  介  创建任务函数
  * @参  数  无
  * @返回值  无
  */
void Start_Task(void *pvParameters)
{	
	uint8_t cnt;
	uint8_t temp = 0x55;
	
	//陀螺仪校准变量
	int16_t gyro_data[3]; 
	
	/******机器人启动流程************************************************/
	
	//读取机器人EEPROM参数，并设置
	if(AX_EEPROM_ReadOneByte(0x00) == 0x88)
	{

	}
	else  
	{
		
		AX_SERVO_INIT();

		//标记参数已初始化过
		AX_EEPROM_WriteOneByte(0x00,0x88);  
		
	}
	
	//陀螺仪零点校准
	for(int i=0; i<10; i++) 
	{
		//红灯闪烁，指示陀螺仪校准
		AX_LED_Green_On();
		vTaskDelay(30); 
		
		AX_LED_Green_Off();
		vTaskDelay(20); 

		//获取PMU6050陀螺仪数据
        AX_MPU6050_GetGyroData(gyro_data);
		
		ax_imu_gyro_offset[0] += gyro_data[0];
		ax_imu_gyro_offset[1] += gyro_data[1];
		ax_imu_gyro_offset[2] += gyro_data[2]; 		
	}
	
	//计算平均偏差值
	ax_imu_gyro_offset[0] = -ax_imu_gyro_offset[0]/10;
	ax_imu_gyro_offset[1] = -ax_imu_gyro_offset[1]/10;
	ax_imu_gyro_offset[2] = -ax_imu_gyro_offset[2]/10;	
	

	//开机等待，展示等待灯效
	for(int i=0; i<3; i++) 
	{
		//呼吸效果, 1S 
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
	
	
	//开机启动完成，绿灯点亮，蜂鸣器提示
	AX_LED_Green_On();	
	AX_BEEP_On();
	AX_Delayms(100);	
	AX_BEEP_Off();	
	
	//显示主窗口界面
	AX_OLED_ClearScreen();  //清除OLED启动画面显示
	AX_OLED_DispStr(0, 0, "   * TARKBOT MEC *   ", 0);	
	AX_OLED_DispStr(0, 1, "---------------------", 0);	

	AX_OLED_DispStr(0, 2, " Ver:V0.00 Mod:ROS   ", 0);
	AX_OLED_DispStr(30, 2, ROBOT_FW_VER, 0);		
	AX_OLED_DispStr(0, 3, " Vol:12.2V Gyz:00000 ", 0);
	AX_OLED_DispStr(0, 4, "---------------------", 0);	
	AX_OLED_DispStr(0, 5, "---------------------", 0);	
	AX_OLED_DispStr(0, 6, " MTA:00.00 MTB:-0.00 ", 0);
	AX_OLED_DispStr(0, 7, " MTC:00.00 MTD:-0.00 ", 0);
	
	//根据宏定义，显示底盘类型
	#if (ROBOT_TYPE == ROBOT_MEC)
	AX_OLED_DispStr(78, 0, "MEC", 0);	
	#endif
	
	//进入临界区
	taskENTER_CRITICAL();   

	//创建机器人控制任务
	xTaskCreate((TaskFunction_t )Robot_Task, /* 任务入口函数 */
			 (const char*    )"Robot_Task",/* 任务名字 */
			 (uint16_t       )ROBOT_STK_SIZE,   /* 任务栈大小 */
			 (void*          )NULL,	/* 任务入口函数参数 */
			 (UBaseType_t    )ROBOT_TASK_PRIO,	    /* 任务的优先级 */
			 (TaskHandle_t*  )&Robot_Task_Handle);/* 任务控制块指针 */

  //创建ESP8266通信与控制任务
	xTaskCreate((TaskFunction_t )ESP8266_Task, /* 任务入口函数 */
			 (const char*    )"ESP8266_Task",/* 任务名字 */
			 (uint16_t       )ESP8266_STK_SIZE,   /* 任务栈大小 */
			 (void*          )NULL,	/* 任务入口函数参数 */
			 (UBaseType_t    )ESP8266_TASK_PRIO,	    /* 任务的优先级 */
			 (TaskHandle_t*  )&ESP8266_Task_Handle);/* 任务控制块指针 */
			 
	//创建按键处理任务
	xTaskCreate((TaskFunction_t )Key_Task, /* 任务入口函数 */
			 (const char*    )"Key_Task",/* 任务名字 */
			 (uint16_t       )KEY_STK_SIZE,   /* 任务栈大小 */
			 (void*          )NULL,	/* 任务入口函数参数 */
			 (UBaseType_t    )KEY_TASK_PRIO,	    /* 任务的优先级 */
			 (TaskHandle_t*  )&Key_Task_Handle);/* 任务控制块指针 */
			 
	//OLED屏显示任务
	xTaskCreate((TaskFunction_t )Disp_Task, /* 任务入口函数 */
			 (const char*    )"Disp_Task",/* 任务名字 */
			 (uint16_t       )DISP_STK_SIZE,   /* 任务栈大小 */
			 (void*          )NULL,	/* 任务入口函数参数 */
			 (UBaseType_t    )DISP_TASK_PRIO,	    /* 任务的优先级 */
			 (TaskHandle_t*  )&Disp_Task_Handle);/* 任务控制块指针 */	
			 
	//琐事管理任务
	xTaskCreate((TaskFunction_t )Trivia_Task, /* 任务入口函数 */
			 (const char*    )"Trivia_Task",/* 任务名字 */
			 (uint16_t       )TRIVIA_STK_SIZE,   /* 任务栈大小 */
			 (void*          )NULL,	/* 任务入口函数参数 */
			 (UBaseType_t    )TRIVIA_TASK_PRIO,	    /* 任务的优先级 */
			 (TaskHandle_t*  )&Trivia_Task_Handle);/* 任务控制块指针 */			 
						  
	xTaskCreate((TaskFunction_t )GPS_Task, /* 任务入口函数 */
			 (const char*    )"GPS_Task",/* 任务名字 */
			 (uint16_t       )GPS_STK_SIZE,   /* 任务栈大小 */
			 (void*          )NULL,	/* 任务入口函数参数 */
			 (UBaseType_t    )GPS_TASK_PRIO,	    /* 任务的优先级 */
			 (TaskHandle_t*  )&GPS_Task_Handle);/* 任务控制块指针 */	
			 
	xTaskCreate((TaskFunction_t )OPENMV_Task, /* 任务入口函数 */
			 (const char*    )"OPENMV_Task",/* 任务名字 */
			 (uint16_t       )OPENMV_STK_SIZE,   /* 任务栈大小 */
			 (void*          )NULL,	/* 任务入口函数参数 */
			 (UBaseType_t    )OPENMV_TASK_PRIO,	    /* 任务的优先级 */
			 (TaskHandle_t*  )&OPENMV_Task_Handle);/* 任务控制块指针 */
			 
  xTaskCreate((TaskFunction_t )SERVO_Task, /* 任务入口函数 */
			 (const char*    )"SERVO_Task",/* 任务名字 */
			 (uint16_t       )SERVO_STK_SIZE,   /* 任务栈大小 */
			 (void*          )NULL,	/* 任务入口函数参数 */
			 (UBaseType_t    )SERVO_TASK_PRIO,	    /* 任务的优先级 */
			 (TaskHandle_t*  )&SERVO_Task_Handle);/* 任务控制块指针 */	 
	//删除AppTaskCreate任务				
	vTaskDelete(StartTask_Handler); 

	//退出临界区
	taskEXIT_CRITICAL();           
						 							
}
