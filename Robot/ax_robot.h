#ifndef __AX_ROBOT_H
#define __AX_ROBOT_H

/* Includes ------------------------------------------------------------------*/	 
#include "stm32f4xx.h"

//C库函数的相关头文件
#include <stdio.h> 
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

//FreeRTOS头文件
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "timers.h"
#include "semphr.h"
 
//外设相关头文件
#include "ax_sys.h"      //系统设置
#include "ax_delay.h"    //软件延时
#include "ax_led.h"      //LED灯控制
#include "ax_beep.h"     //蜂鸣器控制
#include "ax_uart1.h"    //调试串口
#include "ax_vin.h"      //输入电压检测
#include "ax_key.h"      //按键检测 
#include "ax_switch.h"   //按键检测 

#include "ax_uart1.h"    //调试串口
#include "ax_uart2.h"    //TTL串口串口
#include "ax_uart3.h"    //USB通信串口
#include "ax_uart4.h"    //蓝牙串口
#include "ax_uart5.h"

#include "ax_servo.h"    //舵机控制
#include "ax_motor.h"    //直流电机调速控制
#include "ax_encoder.h"  //编码器控制

#include "ax_eeprom.h"   //EEPROM存储
#include "ax_mpu6050.h"  //IMU加速度陀螺仪测量
#include "ax_can.h"      //CAN控制

#include "ax_oled.h"     //OLED显示

#include "ax_akm.h"      //阿克曼机器人处理函数


//机器人轮子速度数据结构体
typedef struct  
{
	double  RT;       //车轮实时速度，单位m/s
	float  TG;       //车轮目标速度，单位m/s
	short  PWM;      //车轮PWM控制速度
	
}ROBOT_Wheel;

//机器人速度结构体
typedef struct  
{
	short  RT_IX;     //实时X轴速度（16位整数）
	short  RT_IY;     //实时Y轴速度（16位整数）
	short  RT_IW;     //实时Yaw旋转轴速度（16位整数）
	
	short  TG_IX;     //目标X轴速度（16位整数）
	short  TG_IY;     //目标Y轴速度（16位整数）
	short  TG_IW;     //目标Yaw旋转轴速度（16位整数）
	
	float  RT_FX;     //实时X轴速度（浮点）
	float  RT_FY;     //实时Y轴速度（浮点）
	float  RT_FW;     //实时Yaw旋转轴速度（浮点）
	
	float  TG_FX;     //目标X轴速度（浮点）
	float  TG_FY;     //目标Y轴速度（浮点）
	float  TG_FW;     //目标Yaw旋转轴速度（浮点）
	
}ROBOT_Velocity;


//机器人IMU数据
typedef struct  
{
	short  ACC_X;     //X轴
	short  ACC_Y;     //Y轴
	short  ACC_Z;     //Z轴
	
	short  GYRO_X;     //X轴
	short  GYRO_Y;     //Y轴
	short  GYRO_Z;     //Z轴
	
}ROBOT_Imu;


typedef struct  
{
	uint8_t  M;    //灯效主模式
	uint8_t  S;    //灯效从模式
	uint8_t  T;    //灯效时间参数
	
	uint8_t  R;     //灯效颜色 R
	uint8_t  G;     //灯效颜色 G
	uint8_t  B;     //灯效颜色 B
	
}ROBOT_Light;

//杂类
#define  PI           3.1416     //圆周率PI
#define  SQRT3        1.732      //3开平方
#define  PID_RATE     50         //PID频率

//电池
#define  VBAT_40P    1065     //电池40%电压
#define  VBAT_20P    1012     //电池20%电压
#define  VBAT_10P    984      //电池10%电压

//机器人软件版本
#define  ROBOT_FW_VER   "V3.02"

/******机器人类型***********************************/

//机器人类型定义
#define ROBOT_MEC   0x01   //麦克纳姆轮底盘
#define ROBOT_FWD   0x02   //四轮差速底盘
#define ROBOT_AKM   0x03   //阿克曼转向底盘
#define ROBOT_TWD   0x04   //两轮差速转向底盘 
#define ROBOT_TNK   0x05   //履带坦克底盘
#define ROBOT_OMT   0x06   //三轮全向底盘

//机器人定义类型
#define ROBOT_TYPE   ROBOT_MEC

/******机器人参数*************************************/

//麦轮机器人参数
#define  MEC_WHEEL_BASE           0.196	      //轮距，左右轮的距离
#define  MEC_ACLE_BASE            0.160       //轴距，前后轮的距离
#define  MEC_WHEEL_DIAMETER	      0.098		  //轮子直径
#define  MEC_WHEEL_RESOLUTION     1560.0      //编码器分辨率(13线),减速比30,13x30x4=1560
#define  MEC_WHEEL_SCALE          (PI*MEC_WHEEL_DIAMETER*PID_RATE/MEC_WHEEL_RESOLUTION) //轮子速度m/s与编码器转换系数

//四轮差速机器人参数
//由于轮胎摩擦力较大，采用60减速比电机，电机转向与30减速比相反
#define  FWD_WHEEL_BASE           0.200	     //轮距，左右轮的距离
#define  FWD_WB_SCALE             1.75       //轮距系数，轮距系数与机器人的总负载、轮胎与地面的相对摩擦系数、转弯半径及质心位置都是有关系是一个非常复杂的参数，所以常用的方法就是做实验
#define  FWD_WHEEL_DIAMETER	      0.100		 //轮子直径
#define  FWD_WHEEL_RESOLUTION     3120.0     //编码器分辨率(13线),减速比60,13x60x4=3120
#define  FWD_WHEEL_SCALE          (PI*FWD_WHEEL_DIAMETER*PID_RATE/FWD_WHEEL_RESOLUTION) //轮子速度m/s与编码器转换系数

//阿克曼机器人参数
#define  AKM_WHEEL_BASE           0.165	     //轮距，左右轮的距离
#define  AKM_ACLE_BASE            0.175f     //轴距，前后轮的距离
#define  AKM_WHEEL_DIAMETER	      0.075		 //轮子直径
#define  AKM_WHEEL_RESOLUTION     1560.0      //编码器分辨率(13线),减速比30,13x30x4=1560
#define  AKM_TURN_R_MINI          0.35f      //最小转弯半径( L*cot30-W/2)
#define  AKM_WHEEL_SCALE          (PI*AKM_WHEEL_DIAMETER*PID_RATE/AKM_WHEEL_RESOLUTION)  //轮子速度m/s与编码器转换系数

//两轮差速机器人参数 
#define  TWD_WHEEL_DIAMETER	      0.0724	 //轮子直径
#define  TWD_WHEEL_BASE           0.206	     //轮距，左右轮的距离
#define  TWD_WHEEL_RESOLUTION     1560.0      //编码器分辨率(13线),减速比30,13x30x4=1560
#define  TWD_WHEEL_SCALE          (PI*TWD_WHEEL_DIAMETER*PID_RATE/TWD_WHEEL_RESOLUTION)  //轮子速度m/s与编码器转换系数

//履带式机器人参数 
#define  TNK_WHEEL_DIAMETER	      0.0724	 //轮子直径
#define  TNK_WHEEL_BASE           0.206	     //轮距，左右轮的距离
#define  TNK_WHEEL_RESOLUTION     1560.0      //编码器分辨率(13线),减速比30,13x30x4=1560
#define  TNK_WHEEL_SCALE          (PI*TWD_WHEEL_DIAMETER*PID_RATE/TNK_WHEEL_RESOLUTION)  //轮子速度m/s与编码器转换系数

//三轮全向机器人参数 
#define  OMT_WHEEL_DIAMETER	      0.058	     //轮子直径
#define  OMT_WHEEL_RADIUS         0.206	     //机器人半径，轮子与机器人中心距离
#define  OMT_WHEEL_RESOLUTION     1560.0     //编码器分辨率(13线),减速比30,13x30x4=1560
#define  OMT_WHEEL_SCALE          (PI*TWD_WHEEL_DIAMETER*PID_RATE/OMT_WHEEL_RESOLUTION)  //轮子速度m/s与编码器转换系数

/******机器人通信协议*************************************/

//串口通信帧头定义
#define  ID_UTX_DATA    0x10    //发送的综合数据
#define  ID_URX_VEL     0x50    //接收的速度数据
#define  ID_URX_IMU     0x51    //接收的IMU陀螺仪零偏校准指令
#define  ID_URX_LG      0x52    //接收的灯光控制指令
#define  ID_URX_LS      0x53    //接收的灯光效果保存指令
#define  ID_URX_BP      0x54    //接收蜂鸣器鸣叫指令
#define  ID_URX_RTY     0x5A    //接收的机器人型号选择

//蓝牙APP通信帧头定义
#define  ID_BLERX_CM      0x30    //APP蓝牙发送 蓝牙连接指令
#define  ID_BLERX_YG      0x31    //APP蓝牙发送 摇杆模式 控制指令
#define  ID_BLERX_SB      0x32    //APP蓝牙发送 手柄模式 控制指令
#define  ID_BLERX_ZL      0x33    //APP蓝牙发送 重力模式 控制指令
#define  ID_BLERX_TK      0x34    //APP蓝牙发送 坦克模式 控制指令
#define  ID_BLERX_AM      0x3A    //APP蓝牙发送 机械臂模式 控制指令
#define  ID_BLERX_LG      0x41    //APP蓝牙发送 灯光控制指令
#define  ID_BLERX_LS      0x42    //APP蓝牙发送 保存灯光效果指令

//CAN通信帧头定义
#define  ID_CANRX_VEL     0x0181    //接收的CAN速度数据
#define  ID_CANTX_MSG1    0x0121    //发送的机器人数据1
#define  ID_CANTX_MSG2    0x0122    //发送的机器人数据2
#define  ID_CANTX_MSG3    0x0123    //发送的机器人数据3

//机器人速度限制
#define R_VX_LIMIT  1500   //X轴速度限值 m/s*1000
#define R_VY_LIMIT  1200   //Y轴速度限值 m/s*1000
#define R_VW_LIMIT  6280   //W旋转角速度限值 rad/s*1000

//灯光模式
#define  LEFFECT1    0x01    //单色模式  
#define  LEFFECT2    0x02    //呼吸模式  
#define  LEFFECT3    0x03    //彩色模式  
#define  LEFFECT4    0x04    //警灯模式 
#define  LEFFECT5    0x05    //眼睛模式  
#define  LEFFECT6    0x06    //跑马灯模式

//机器人控制方式
#define  CTL_ROS    0x00    //ROS控制，包含串口和CAN
#define  CTL_PS2    0x01    //PS2手柄控制
#define  CTL_APP    0x02    //APP控制
#define  CTL_RMS    0x03    //SBUS航模遥控器控制	

//蜂鸣器鸣长短
#define  BEEP_SHORT   0x01    //蜂鸣器短鸣叫一声(200ms)
#define  BEEP_LONG    0x02    //蜂鸣器长鸣叫一声(1000ms)

//机器人关键全局变量
extern  ROBOT_Velocity  R_Vel; //机器人速度数据
extern  ROBOT_Wheel  R_Wheel_A,R_Wheel_B,R_Wheel_C,R_Wheel_D; //机器人轮子数据
extern  ROBOT_Imu  R_Imu; //机器人IMU数据
extern  ROBOT_Light  R_Light; //机器人RGB数据
extern  uint16_t R_Bat_Vol;  //机器人电池电压数据

//全局变量
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

////PS2手柄键值结构体
//extern JOYSTICK_TypeDef my_joystick;  

//阿克曼机器人
extern int16_t ax_akm_offset;
extern int16_t ax_akm_angle;



//任务句柄
extern TaskHandle_t Robot_Task_Handle;
extern TaskHandle_t ESP8266_Task_Handle;
extern TaskHandle_t Key_Task_Handle;
extern TaskHandle_t Disp_Task_Handle;
extern TaskHandle_t Trivia_Task_Handle;
extern TaskHandle_t GPS_Task_Handle;
extern TaskHandle_t OPENMV_Task_Handle;
// 互斥锁定义
extern SemaphoreHandle_t arm_state_mutex;



#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_usart.h"
#include "ax_uart1.h" 
#include <stdio.h>
#include <string.h>
#include "FreeRTOS.h"
#include "task.h"

// 动作定义
#define RESET_0 0
#define RESET_1 1
#define RESET_2 2
#define RESET_3 3


/* 函数声明 */
void USART_Configuration(void);

void processReset(uint8_t reset_value);
void sendToESP8266(uint8_t action);
void ESP8266_Task(void *Parameter);

// 全局变量
extern uint8_t uart1_rx_ok;
extern uint8_t uart1_rx_buf[40];

#endif
