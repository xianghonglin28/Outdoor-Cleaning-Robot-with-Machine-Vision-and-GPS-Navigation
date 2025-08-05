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

//舵机控制标志位
volatile uint8_t servo_flag = 0;


//机器人速度数据
ROBOT_Velocity  R_Vel;

//机器人轮子数据
ROBOT_Wheel  R_Wheel_A,R_Wheel_B,R_Wheel_C,R_Wheel_D;

//机器人IMU数据
ROBOT_Imu  R_Imu;

//机器人速度数据
ROBOT_Velocity  R_Vel;

//机器人电池电压数据
uint16_t R_Bat_Vol;  

u8 USART1_TX_BUF[UART5_MAX_RECV_LEN]; 
nmea_msg gpsx;

//IMU数据
int16_t ax_imu_acc_data[3];  
int16_t ax_imu_gyro_data[3]; 
int16_t ax_imu_gyro_offset[3]; 

//电机PID控制参数
int16_t ax_motor_kp=800;      
int16_t ax_motor_kd=1000; 

//机器人运动使能开关,默认打开状态
uint8_t ax_robot_move_enable = 1;

//IMU校准标志位
int8_t ax_imu_calibrate_flag = 0;

//灯光开关失能，默认打开状态
uint8_t ax_light_enable = 1;

//灯光效果，EEPROM数据更新
uint8_t ax_light_save_flag = 0;

//蜂鸣器鸣叫一声
uint8_t ax_beep_ring = 0;

////PS2手柄键值结构体
//JOYSTICK_TypeDef my_joystick;  

//控制方式选择
uint8_t ax_control_mode = CTL_ROS;

//阿克曼机器人前轮转向角度
int16_t ax_akm_angle = 0;

//阿克曼机器人前轮零偏角度
int16_t ax_akm_offset = 0;


//函数定义
void ROBOT_IMUHandle(void);     //IMU数据处理
void ROBOT_SendDataToRos(void);  //发送数据


/**
  * @简  述  机器人管理任务
  * @参  数  无
  * @返回值  无
  */
void Robot_Task(void* parameter)
{	

	//用于保存上次时间。调用后系统自动更新
	static portTickType PreviousWakeTime;

	//设置延时时间20ms，将时间转为节拍数 
	const portTickType TimeIncrement = pdMS_TO_TICKS(20);
	
	//获取当前系统时间 
	PreviousWakeTime = xTaskGetTickCount();
	
	while(1)
	{
		
		//调用绝对延时函数20ms,执行频率50HZ
		vTaskDelayUntil(&PreviousWakeTime, TimeIncrement );
		
		//控制方式选择
		if(ax_control_mode != 0)
		{
//			if      (ax_control_mode == CTL_PS2)    AX_CTL_Ps2();    //PS2手柄控制
			 if (ax_control_mode == CTL_APP)    AX_CTL_App();    //APP控制
//			else if (ax_control_mode == CTL_RMS)    AX_CTL_RemoteSbus();   //SBUS航模遥控器控制
		}
		
		//判断机器人运动是否开启
		if(ax_robot_move_enable != 0)
		{
			//机器人运动学处理
			AX_ROBOT_Kinematics();				
		}
		else
		{
			//机器人静止不动
			AX_ROBOT_Stop();				
		}
		
		//获取PMU6050加速度数据
        ROBOT_IMUHandle();
		
		//数据发送
		ROBOT_SendDataToRos();
	}
}

/**
  * @简  述  机器人IMU数据处理
  * @参  数  无
  * @返回值  无
  */
void ROBOT_IMUHandle(void)
{
		
	//获取PMU6050加速度数据
	AX_MPU6050_GetAccData(ax_imu_acc_data);
	
	//IMU坐标向机器人ROS坐标变换
	R_Imu.ACC_X =  ax_imu_acc_data[1];  //ROS坐标X轴对应IMU的Y轴
	R_Imu.ACC_Y = -ax_imu_acc_data[0];  //ROS坐标Y轴对应IMU的X轴反向
	R_Imu.ACC_Z =  ax_imu_acc_data[2];  //ROS坐标Z轴对应IMU的Z轴
	
	//获取PMU6050陀螺仪数据
	AX_MPU6050_GetGyroData(ax_imu_gyro_data);
	
	//陀螺仪加入零票校准数据
	ax_imu_gyro_data[0] += ax_imu_gyro_offset[0];
	ax_imu_gyro_data[1] += ax_imu_gyro_offset[1];
	ax_imu_gyro_data[2] += ax_imu_gyro_offset[2];
	
	//IMU坐标向机器人ROS坐标变换
	R_Imu.GYRO_X =  ax_imu_gyro_data[1];  //ROS坐标X轴对应IMU的Y轴
	R_Imu.GYRO_Y = -ax_imu_gyro_data[0];  //ROS坐标Y轴对应IMU的X轴反向
	R_Imu.GYRO_Z =  ax_imu_gyro_data[2];  //ROS坐标Z轴对应IMU的Z轴
	
	//观察数据
    //printf("@ %d  \r\n",R_Imu.ACC_X);
}


/**
  * @简  述  机器人发送数据到ROS
  * @参  数  无
  * @返回值  无
  */
void ROBOT_SendDataToRos(void)
{
    //串口发送数据
	static uint8_t comdata[20]; 	

	//加速度 = (ax_acc/32768) * 2G  
	comdata[0] = (u8)( R_Imu.ACC_X >> 8 );  
	comdata[1] = (u8)( R_Imu.ACC_X );
	comdata[2] = (u8)( R_Imu.ACC_Y >> 8 );
	comdata[3] = (u8)( R_Imu.ACC_Y );
	comdata[4] = (u8)( R_Imu.ACC_Z >> 8 );
	comdata[5] = (u8)( R_Imu.ACC_Z );
	
	//陀螺仪角速度 = (ax_gyro/32768) * 500
	comdata[6] = (u8)( R_Imu.GYRO_X >> 8 );
	comdata[7] = (u8)( R_Imu.GYRO_X );
	comdata[8] = (u8)( R_Imu.GYRO_Y >> 8 );
	comdata[9] = (u8)( R_Imu.GYRO_Y );
	comdata[10] = (u8)( R_Imu.GYRO_Z	>> 8 );
	comdata[11] = (u8)( R_Imu.GYRO_Z );
	
	//机器人速度值 单位为m/s，放大1000倍
	comdata[12] = (u8)( R_Vel.RT_IX >> 8 );
	comdata[13] = (u8)( R_Vel.RT_IX );
	comdata[14] = (u8)( R_Vel.RT_IY >> 8 );
	comdata[15] = (u8)( R_Vel.RT_IY );
	comdata[16] = (u8)( R_Vel.RT_IW >> 8 );
	comdata[17] = (u8)( R_Vel.RT_IW );
	
	//电池电压
	comdata[18] = (u8)( R_Bat_Vol >> 8 );
	comdata[19] = (u8)( R_Bat_Vol );

	//TTL串口发送数据
  //AX_UART2_SendPacket(comdata, 20, ID_UTX_DATA);		
	
	//USB串口发送数据
    AX_UART3_SendPacket(comdata, 20, ID_UTX_DATA);		
	
	//CAN数据发送
	AX_CAN_SendPacket(ID_CANTX_MSG1, 6, &comdata[0]);
	AX_CAN_SendPacket(ID_CANTX_MSG2, 6, &comdata[6]);
	AX_CAN_SendPacket(ID_CANTX_MSG3, 8, &comdata[12]);
}

/**
  * @简  述  琐事管理任务
  * @参  数  无
  * @返回值  无
  */
void Trivia_Task(void* parameter)
{	
	uint8_t i;	
	
	//计数变量
	static uint16_t ax_bat_vol_cnt = 0; 
	
	//陀螺仪校准变量
	static int16_t gyro_data[3]; 
	

	while (1)
	{	
		
		/*****电池管理***********************************/
		
		//采集电池电压
	    R_Bat_Vol = AX_VIN_GetVol_X100();
		
		//调试输出电池电压数据
        //printf("@ %d  \r\n",R_Bat_Vol);		
		
		//电量低于40%
		if(R_Bat_Vol < VBAT_40P)  
		{
			//红灯开始闪烁警示
			AX_LED_Red_Toggle();
			
			//电量低于20%
			if(R_Bat_Vol < VBAT_20P)
			{
				//红灯常亮
				AX_LED_Red_On();
				
				//电量低于10%，关闭系统进入保护状态
				if(R_Bat_Vol < VBAT_10P) //990
				{
					//低压时间计数
					ax_bat_vol_cnt++;
					
					//超过10次，进入关闭状态
					if(ax_bat_vol_cnt > 10 )
					{
						//关闭绿灯，红灯常亮
						AX_LED_Green_Off();
						AX_LED_Red_On();
						
						//任务挂起
						vTaskSuspend(Robot_Task_Handle);
						vTaskSuspend(Disp_Task_Handle);
						
						//电机速度设置为0
						AX_MOTOR_A_SetSpeed(0);
						AX_MOTOR_B_SetSpeed(0);  
						AX_MOTOR_C_SetSpeed(0);
						AX_MOTOR_D_SetSpeed(0); 
						
						//清除OLED启动画面显示
						AX_OLED_ClearScreen();  //
						
						//蜂鸣器鸣叫报警
						while(1)
						{	
							//显示机器人停止信息
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
			//红灯关闭
			AX_LED_Red_Off();
		}
		
		/*****IMU校准***********************************/
		
		//检测IMU校准标志位
		if(ax_imu_calibrate_flag > 0)
		{
            //付零IMU校准变量
			ax_imu_gyro_offset[0] = 0;
			ax_imu_gyro_offset[1] = 0;
			ax_imu_gyro_offset[2] = 0;
			
			//机器人任务挂起
			vTaskSuspend(Robot_Task_Handle);
			
			//蜂鸣器提示
			AX_BEEP_On();
			vTaskDelay(50);
			AX_BEEP_Off();
			vTaskDelay(1000);			
			
			//点亮红灯提示
			AX_LED_Red_On();
			
			//陀螺仪校准
			for(i=0; i<10; i++) 
			{
				//延时函数
				vTaskDelay(20);
				
				//获取PMU6050陀螺仪数据
				AX_MPU6050_GetGyroData(gyro_data);
				
				//计算偏差和
				ax_imu_gyro_offset[0] += gyro_data[0];
				ax_imu_gyro_offset[1] += gyro_data[1];
				ax_imu_gyro_offset[2] += gyro_data[2]; 		
			}
			
			//计算平均偏差值
			ax_imu_gyro_offset[0] = -ax_imu_gyro_offset[0]/10;
			ax_imu_gyro_offset[1] = -ax_imu_gyro_offset[1]/10;
			ax_imu_gyro_offset[2] = -ax_imu_gyro_offset[2]/10;
			
			//关闭红灯
			AX_LED_Red_Off();
			
			//机器人任务恢复
			vTaskResume(Robot_Task_Handle);
			
			//复位IMU校准标志位			
			ax_imu_calibrate_flag = 0;

		}   	
		
		/*****灯光效果EEPROM保存***********************************/
		
		//更新EEPROM中灯光效果数据
		if(ax_light_save_flag > 0)
		{
			//机器人任务挂起
			vTaskSuspend(Robot_Task_Handle);
 
			//蜂鸣器提示
			AX_BEEP_On();
			vTaskDelay(200);
			AX_BEEP_Off();
			vTaskDelay(1000);	
			
			//机器人任务恢复
			vTaskResume(Robot_Task_Handle);
			
			//复位灯光保存标志位			
			ax_light_save_flag = 0;
			
		}			
		
		/*****蜂鸣器鸣叫一声***********************************/
		if(ax_beep_ring != 0)
		{
			if(ax_beep_ring == BEEP_SHORT)
			{
				//短鸣叫一声
				AX_BEEP_On();
				vTaskDelay(200); 
				AX_BEEP_Off();
				
				//鸣叫一声标志复位
				ax_beep_ring = 0;
			}
			else //
			{
				//长鸣叫一声
				AX_BEEP_On();
				vTaskDelay(1000); 
				AX_BEEP_Off();
				
				//鸣叫一声标志复位
				ax_beep_ring = 0;				
			}
			
		}
		
		//LED系统心跳指示
		AX_LED_Green_Toggle();	
		
		
        //循环周期500ms
		vTaskDelay(500); 
	}			
}

/**
  * @简  述  按键处理任务
  * @参  数  无
  * @返回值  无
  */
void Key_Task(void* parameter)
{	
	uint8_t  i;
	//int16_t  akm_offset;
	
	while (1)
	{		
		//按键扫描
		if(AX_KEY_Scan() != 0)
		{
			//软件延时
			vTaskDelay(50);  
			
			//确定按键按下
			if(AX_KEY_Scan() != 0)
			{
				//等待按键抬起
				for(i=0; i<200; i++)
				{

					vTaskDelay(50);
					
					if(AX_KEY_Scan() == 0)
					{
						break;
					}
					
					//按键时长3S时，给出提示音
					if(i == 60)
					{
						AX_BEEP_On();
						vTaskDelay(200);
						AX_BEEP_Off();
					}
				}

				//短按检测,小于1S
				if(i < 20)
				{
					AX_EEPROM_WriteOneByte(0x00, 0x00);
					//任务挂起
					vTaskSuspend(Robot_Task_Handle);
					vTaskSuspend(Disp_Task_Handle);
					AX_MOTOR_A_SetSpeed(0);
					AX_MOTOR_B_SetSpeed(0);  
					AX_MOTOR_C_SetSpeed(0);
					AX_MOTOR_D_SetSpeed(0); 
					
					//清除OLED启动画面显示
					AX_OLED_ClearScreen(); 
	    
					while(1)
					{
						
					
					}
				}
				
				//中按检测,大于3S，小于10S
				if(i>60 && i<200)
				{
					#if (ROBOT_TYPE == ROBOT_AKM)
					
					#endif

				}				
					
				//长按检测，大于10S
				if(i == 200)
				{
					//清除EEPROM参数标记，恢复默认设置
					AX_EEPROM_WriteOneByte(0x00, 0x00);
					
					//任务挂起
					vTaskSuspend(Robot_Task_Handle);
					vTaskSuspend(Disp_Task_Handle);
					
					//蜂鸣器提示
					AX_BEEP_On();
					vTaskDelay(3000);
					AX_BEEP_Off();					
						
					//电机速度设置为0
					AX_MOTOR_A_SetSpeed(0);
					AX_MOTOR_B_SetSpeed(0);  
					AX_MOTOR_C_SetSpeed(0);
					AX_MOTOR_D_SetSpeed(0); 
					
					//清除OLED启动画面显示
					AX_OLED_ClearScreen(); 
					
					//蜂鸣器鸣叫报警
					while(1)
					{	
						//显示机器人停止信息
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
		
		//循环周期
		vTaskDelay(50);    
	}			
}


/**
  * @简  述  指示灯处理任务
  * @参  数  无
  * @返回值  无
  */
void Light_Task(void* parameter)
{	
	while (1)
	{	
        //延时30ms，30HZ显示频率		
		vTaskDelay(30); 
		
		//判断前灯是否打开
		if(ax_light_enable != 0)
		{
			//正常显示
			AX_LIGHT_Show();
		}
		else
		{
		}
	}			
}


/**
  * @简  述  按键处理任务
  * @参  数  无
  * @返回值  无
  */
void Disp_Task(void* parameter)
{	
	while (1)
	{	
		//延时	
		vTaskDelay(100); 
		
		//显示控制模式
		if(ax_control_mode)
		{
			if      (ax_control_mode == CTL_PS2)    AX_OLED_DispStr(90, 2, "PS2", 0);   //PS2手柄控制
			else if (ax_control_mode == CTL_APP)    AX_OLED_DispStr(90, 2, "APP", 0);   //APP控制
			else if (ax_control_mode == CTL_RMS)    AX_OLED_DispStr(90, 2, "RMS", 0);   //SBUS航模遥控器控制			
		}
		else
		{
			//ROS控制模式
			AX_OLED_DispStr(90, 2, "ROS", 0);
		}
		
		//显示电池电压，陀螺仪Z轴数据,舵机偏角,
		AX_OLED_DispValue(30, 3, (R_Bat_Vol*0.1), 2, 1, 0);
		AX_OLED_DispValue(90, 3, (R_Imu.GYRO_Z ), 6, 0, 0);	
		
		vTaskDelay(100); 
		//显示轮子实时速度
		AX_OLED_DispValue(30, 6, (R_Wheel_A.RT*100), 2, 2, 0);
		AX_OLED_DispValue(90, 6, (R_Wheel_B.RT*100), 2, 2, 0);
		AX_OLED_DispValue(30, 7, (R_Wheel_C.RT*100), 2, 2, 0);
		AX_OLED_DispValue(90, 7, (R_Wheel_D.RT*100), 2, 2, 0);	
	}			
}

/**
  * @brief ESP8266通信与控制任务
  * @param Parameter: FreeRTOS任务参数
  * @retval 无
  */
void ESP8266_Task(void *Parameter)
{
    // 初始化消息
    USART3_SendString("\r\n[ESP8266] Communication Task Started!\r\n");
    
    uint8_t rxByte = 0;
    TickType_t lastActionTime = 0;

	
    const TickType_t actionCooldown = pdMS_TO_TICKS(500); // 动作冷却时间(ms)
    
	
    while (1)
    {
			  
			
        // 检查USART1是否有数据（来自ESP8266）
        if (USART_GetFlagStatus(USART1, USART_FLAG_RXNE) == SET)
        {
            rxByte = USART_ReceiveData(USART1);
            
            // 打印原始接收数据
            char rawMsg[50];
            snprintf(rawMsg, sizeof(rawMsg), "[ESP8266] Raw: 0x%02X\r\n", rxByte);
            USART3_SendString(rawMsg);
            
            TickType_t currentTime = xTaskGetTickCount();
            
            // 处理有效reset值 (0,1,2,3) 并检查冷却时间
            if (rxByte <= RESET_3)
            {
                if (currentTime - lastActionTime >= actionCooldown)
                {
                    lastActionTime = currentTime;
                    
                    // 打印接收到的reset值
                    char rxMsg[50];
                    snprintf(rxMsg, sizeof(rxMsg), "Received reset: %d\r\n", rxByte);
                    USART3_SendString(rxMsg);
                    
                    // 处理reset命令
                    processReset(rxByte);
                    
                    // 发送结果回ESP8266
                    sendToESP8266(rxByte); // 假设执行成功返回相同值
                }
                else
                {
                    USART3_SendString("[WARN] Action too frequent! Ignored.\r\n");
                }
            }
            else
            {
                // 打印无效数据
                char errMsg[50];
                snprintf(errMsg, sizeof(errMsg), "[ERROR] Invalid reset: %d\r\n", rxByte);
                USART3_SendString(errMsg);
            }
        }
	
        // 添加任务延时
        vTaskDelay(pdMS_TO_TICKS(10));
    }
}

/**
  * @brief 处理接收到的reset命令
  * @param reset_value: reset值 (0,1,2,3)
  */
void processReset(uint8_t reset_value)
{
    char msg[50];
    
    switch (reset_value)
    {
        case RESET_0:
            snprintf(msg, sizeof(msg), "Processing RESET_0 action\r\n");
            AX_SERVO_INIT(); // 执行复位动作
            break;
            
        case RESET_1:
            snprintf(msg, sizeof(msg), "Processing RESET_1 action\r\n");
            //AX_SERVO_up(); // 执行抓取动作
						AX_SERVO_Grab();
            break;
            
        case RESET_2:
            snprintf(msg, sizeof(msg), "Processing RESET_2 action\r\n");
            //AX_SERVO_down(); // 执行放置动作
						AX_SERVO_Release();
            break;
            
        case RESET_3:
            snprintf(msg, sizeof(msg), "Processing RESET_3 action\r\n");
            //AX_SERVO_custom(); // 执行自定义动作
            break;
            
        default:
            snprintf(msg, sizeof(msg), "[ERROR] Unknown reset value: %d\r\n", reset_value);
            break;
    }
    
    // 发送处理信息到调试串口
    USART3_SendString(msg);
}

/**
  * @brief 发送reset结果到ESP8266
  * @param result: 执行结果 (0,1,2,3)
  */
void sendToESP8266(uint8_t result)
{
    // 添加发送间隔控制
    static TickType_t lastSendTime = 0;
    TickType_t currentTime = xTaskGetTickCount();
    const TickType_t minSendInterval = pdMS_TO_TICKS(200); // 最小发送间隔
    
    if (currentTime - lastSendTime < minSendInterval) {
        return;
    }
    lastSendTime = currentTime;
    
    // 发送单个字节结果
    USART_SendData(USART1, result);
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    
    // 打印发送信息到调试串口
    char txMsg[50];
    snprintf(txMsg, sizeof(txMsg), "Sent reset result: %d to ESP8266\r\n", result);
    USART3_SendString(txMsg);
}


void sendToESP8266_string(uint8_t *data, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++)
    {
        USART_SendData(USART1, data[i]);
        while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
    }

    USART_SendData(USART1, '\n'); // 结尾符方便识别
    while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
}


void GPS_Task(void *pvParameters)
{
	u16 i,rxlen;
	TickType_t lastUploadTime = 0;
  const TickType_t uploadInterval = pdMS_TO_TICKS(3000);
	
		while(1) 
	{	
		if(UART5_RX_STA&0X8000)		//接收到一次数据了 第15位
		{
			rxlen=UART5_RX_STA&0X7FFF;	//得到数据长度 0到14位
			for(i=0;i<rxlen;i++)USART1_TX_BUF[i]=UART5_RX_BUF[i];	   
 			UART5_RX_STA=0;		   	//启动下一次接收
			USART1_TX_BUF[i]=0;			//自动添加结束符
			GPS_Analysis(&gpsx,(u8*)USART1_TX_BUF);//分析字符串
		}
		
		TickType_t now = xTaskGetTickCount();
		if (now - lastUploadTime >= uploadInterval)
        {
            lastUploadTime = now;

            char gpsJson[200];
            snprintf(gpsJson, sizeof(gpsJson),
                     "{\"lat\":%.6f,\"lon\":%.6f}",
                     gpsx.latitude / 1000000.0, gpsx.longitude / 1000000.0);

            // 发送到ESP8266 → 云端
            sendToESP8266_string((uint8_t *)gpsJson, strlen(gpsJson));

            USART3_SendString("[GPS] JSON uploaded to ESP8266:\r\n");
            USART3_SendString(gpsJson);
            USART3_SendString("\r\n");

        }

        vTaskDelay(pdMS_TO_TICKS(3000));


  }
}

void OPENMV_Task(void *pvParameters)
{
    TickType_t lastUploadTime = 0;
    const TickType_t uploadInterval = pdMS_TO_TICKS(3000); // 每 3 秒上传一次

    while (1)
    {
        // 打印本地信息
        printf("[OpenMV] Distance: %d mm, X: %d, Y: %d\r\n",
               distance, x_coordinate, y_coordinate);

        // 判断是否到上传时间
        TickType_t now = xTaskGetTickCount();
        if (now - lastUploadTime >= uploadInterval)
        {
            lastUploadTime = now;

            // 构建 JSON 字符串
            char openmvJson[200];
            snprintf(openmvJson, sizeof(openmvJson),
                     "{\"distance\":%d,\"x\":%d,\"y\":%d}",
                     distance, x_coordinate, y_coordinate);

            // 发送到 ESP8266 → 云端
            sendToESP8266_string((uint8_t *)openmvJson, strlen(openmvJson));

            // 本地串口打印发送的内容（调试用）
            USART3_SendString("[OpenMV] JSON uploaded to ESP8266:\r\n");
            USART3_SendString(openmvJson);
            USART3_SendString("\r\n");
        }

        vTaskDelay(pdMS_TO_TICKS(3000)); // 稍微延时，避免过度占用 CPU
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

