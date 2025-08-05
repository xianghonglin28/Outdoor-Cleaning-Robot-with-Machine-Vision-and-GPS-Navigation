/**			                                                    
		   ____                    _____ _______ _____       XTARK@塔克创新
		  / __ \                  / ____|__   __|  __ \ 
		 | |  | |_ __   ___ _ __ | |       | |  | |__) |
		 | |  | | '_ \ / _ \ '_ \| |       | |  |  _  / 
		 | |__| | |_) |  __/ | | | |____   | |  | | \ \ 
		  \____/| .__/ \___|_| |_|\_____|  |_|  |_|  \_\
				| |                                     
				|_|                OpenCTR   机器人控制器
									 
  ****************************************************************************** 
  *           
  * 版权所有： XTARK@塔克创新  版权所有，盗版必究
  * 公司网站： www.xtark.cn   www.tarkbot.com
  * 淘宝店铺： https://xtark.taobao.com  
  * 塔克微信： 塔克创新（关注公众号，获取最新更新资讯）
  *      
  ******************************************************************************
  * @作  者  Musk Han@XTARK
  * @版  本  V1.0
  * @内  容  PWM接口舵机控制
  *
  ******************************************************************************
  * @说  明
  *
  ******************************************************************************
  */

#include "ax_servo.h" 
#include "ax_robot.h"

/**
  * @简  述  舵机接口初始化	
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S1_Init(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure; 
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;   

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);/*使能GPIOF时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM10,ENABLE);/*使能定时器11时钟*/

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource8,GPIO_AF_TIM10);/*复用*/

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; /*复用*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; /*推挽输出*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8; /*PF7*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*上拉*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; /**/
	GPIO_Init(GPIOB,&GPIO_InitStructure); /*初始化IO*/
	
	TIM_TimeBaseStructure.TIM_Prescaler=168-1;  //定时器分频，分频后的频率为1M
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_Period=20000-1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    TIM_TimeBaseInit(TIM10, &TIM_TimeBaseStructure);
	
	//PWM1 Mode configuration: Channel1 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;	    //占空比初始化
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	
	
    TIM_OC1Init(TIM10, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM10, TIM_OCPreload_Enable);
	

	TIM_ARRPreloadConfig(TIM10,ENABLE);/*自动重载预装载使能*/

	TIM_Cmd(TIM10,ENABLE);/*计数使能*/	
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S1_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare1(TIM10,(1.111f*angle + 1500));
	
}

/**
  * @简  述  释放舵机控制
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S1_Release(void)
{
	TIM_SetCompare1(TIM10, 0);
}

/**
  * @简  述  舵机接口初始化	
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S2_Init(void)
{ 
	
    GPIO_InitTypeDef GPIO_InitStructure; 
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;   

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB,ENABLE);/*使能GPIOF时钟*/
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM11,ENABLE);/*使能定时器11时钟*/

	GPIO_PinAFConfig(GPIOB,GPIO_PinSource9,GPIO_AF_TIM11);/*复用*/

	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; /*复用*/
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; /*推挽输出*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_9; /*PF7*/
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; /*上拉*/
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; /**/
	GPIO_Init(GPIOB,&GPIO_InitStructure); /*初始化IO*/
	
	TIM_TimeBaseStructure.TIM_Prescaler=168-1;  //定时器分频，分频后的频率为1M
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
	TIM_TimeBaseStructure.TIM_Period=20000-1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    TIM_TimeBaseInit(TIM11, &TIM_TimeBaseStructure);
	
	//PWM1 Mode configuration: Channel1 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;	    //占空比初始化
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;	
	
    TIM_OC1Init(TIM11, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM11, TIM_OCPreload_Enable);
	

	TIM_ARRPreloadConfig(TIM11,ENABLE);/*自动重载预装载使能*/

	TIM_Cmd(TIM11,ENABLE);/*计数使能*/	
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S2_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare1(TIM11,(1.111f*angle + 1500));
	
}

/**
  * @简  述  释放舵机控制
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S2_Release(void)
{
	TIM_SetCompare1(TIM11, 0);
}


/**
  * @简  述  舵机接口初始化	
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S3456_Init(void)
{ 
    GPIO_InitTypeDef GPIO_InitStructure; 
	
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
    TIM_OCInitTypeDef  TIM_OCInitStructure;   
	
	//时钟使能
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_TIM8, ENABLE);

	//复用功能配置
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource6,GPIO_AF_TIM8); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource7,GPIO_AF_TIM8); 	
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource8,GPIO_AF_TIM8); 
	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM8); 
	
	//配置IO口为复用功能-定时器通道
	GPIO_InitStructure.GPIO_Pin =  GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;        //复用功能
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	//速度100MHz
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;      //推挽复用输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;        //上拉
    GPIO_Init(GPIOC, &GPIO_InitStructure);
	
    //定时器配置	PWM工作模式，频率50Hz,周期20ms  占空比调节范围：0-1.5ms-2.5ms 0-1500-2500	初始化为1500
    //-----------------------------------------------------------------------
	TIM_TimeBaseStructure.TIM_Prescaler=168-1;  //定时器分频，分频后的频率为1M
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
	TIM_TimeBaseStructure.TIM_Period=20000-1;   //自动重装载值
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
    TIM_TimeBaseInit(TIM8, &TIM_TimeBaseStructure);

    //PWM1 Mode configuration: Channel1 
    TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1;
    TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable;
    TIM_OCInitStructure.TIM_Pulse = 0;	    //占空比初始化
    TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
    TIM_OC1Init(TIM8, &TIM_OCInitStructure);
    TIM_OC1PreloadConfig(TIM8, TIM_OCPreload_Enable);

	//PWM1 Mode configuration: Channel2
    TIM_OC2Init(TIM8, &TIM_OCInitStructure);
    TIM_OC2PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
	//PWM1 Mode configuration: Channel3
    TIM_OC3Init(TIM8, &TIM_OCInitStructure);
    TIM_OC3PreloadConfig(TIM8, TIM_OCPreload_Enable);

    //PWM1 Mode configuration: Channel4
    TIM_OC4Init(TIM8, &TIM_OCInitStructure);
    TIM_OC4PreloadConfig(TIM8, TIM_OCPreload_Enable);
	
    TIM_ARRPreloadConfig(TIM8, ENABLE);

    //使能定时器
    TIM_Cmd(TIM8, ENABLE);  
	
    //使能MOE位
	TIM_CtrlPWMOutputs(TIM8,ENABLE);
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S3_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare1(TIM8,(1.111f*angle + 1500));
	
}

/**
  * @简  述  释放舵机控制
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S3_Release(void)
{
	TIM_SetCompare1(TIM8, 0);
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S4_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare2(TIM8,(1.111f*angle + 1500));
}


/**
  * @简  述  释放舵机控制
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S4_Release(void)
{
	TIM_SetCompare2(TIM8, 0);
}


/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S5_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare3(TIM8,(1.111f*angle + 1500));
	
}

/**
  * @简  述  释放舵机控制
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S5_Release(void)
{
	TIM_SetCompare3(TIM8, 0);
}

/**
  * @简  述  舵机控制
  * @参  数  angle 舵机的角度，范围：-900~900，比例系数0.1,
  *          特别说明，部分舵机实际控制角度小于90度，请注意范围保护
  * @返回值  无
  */
void AX_SERVO_S6_SetAngle(int16_t angle)
{
	if(angle >  900) angle =  900;
	if(angle < -900) angle = -900;
	
	TIM_SetCompare4(TIM8,(1.111f*angle + 1500));
	
}

/**
  * @简  述  释放舵机控制
  * @参  数  无
  * @返回值  无
  */
void AX_SERVO_S6_Release(void)
{
	TIM_SetCompare4(TIM8, 0);
}

////机械臂初始状态
//void AX_SERVO_INIT(void)
//{
//	AX_SERVO_S1_SetAngle(550);//固定
//	AX_SERVO_S2_SetAngle(-200);
//	AX_SERVO_S3_SetAngle(-900);
//	AX_SERVO_S4_SetAngle(450);
//	AX_SERVO_S5_SetAngle(650);//固定
//	AX_SERVO_S6_SetAngle(-450);//微张
//}

////机械臂抓取垃圾
//void AX_SERVO_up(void)
//{
//	AX_SERVO_S1_SetAngle(550);//固定
//	AX_SERVO_S2_SetAngle(-200);
//	AX_SERVO_S3_SetAngle(-900);
//	AX_SERVO_S4_SetAngle(450);
//	AX_SERVO_S5_SetAngle(650);//固定
//	AX_SERVO_S6_SetAngle(-900);//全开
//	//->
//	AX_SERVO_S6_SetAngle(-200);//微微张开
//	
//}

////机械臂放下垃圾
//void AX_SERVO_down(void)
//{
//	AX_SERVO_S1_SetAngle(550);//固定
//	AX_SERVO_S2_SetAngle(-200);
//	AX_SERVO_S3_SetAngle(-900);
//	AX_SERVO_S4_SetAngle(450);
//	AX_SERVO_S5_SetAngle(650);//固定
//	AX_SERVO_S6_SetAngle(-200);//微微张开
//	//->
//	AX_SERVO_S6_SetAngle(-900);//全开
//}


// 设置机械臂准备姿态 (S1-S5固定位置)
static void SetReadyPose(void) {
    AX_SERVO_S1_SetAngle(550);  // 底座固定
    AX_SERVO_S2_SetAngle(-200); // 大臂角度
    AX_SERVO_S3_SetAngle(-900); // 小臂角度
    AX_SERVO_S4_SetAngle(450);  // 腕部角度
    AX_SERVO_S5_SetAngle(650);  // 旋转关节固定
		
		vTaskDelay(800);
}

// 机械臂初始状态
void AX_SERVO_INIT(void) {
    SetReadyPose();
    AX_SERVO_S6_SetAngle(-900); // 夹爪初始微张状态
    vTaskDelay(500);              // 等待动作完成
}

// 移动到垃圾上方位置
static void MoveAboveTrash(void) {
	
		AX_SERVO_S2_SetAngle(-300); // 大臂角度
    AX_SERVO_S3_SetAngle(-850); // 小臂角度
	
		AX_SERVO_S2_SetAngle(-400); // 大臂角度
    AX_SERVO_S3_SetAngle(-810); // 小臂角度
	
		AX_SERVO_S2_SetAngle(-500); // 大臂角度
    AX_SERVO_S3_SetAngle(-780); // 小臂角度
	
    AX_SERVO_S2_SetAngle(-650);    // 大臂抬起
    AX_SERVO_S3_SetAngle(-750);  // 小臂调整
    AX_SERVO_S4_SetAngle(200);   // 腕部保持水平
    vTaskDelay(800);
}

// 下降到抓取位置
static void DescendToTrash(void) {
    AX_SERVO_S2_SetAngle(-650);  // 大臂下降
    AX_SERVO_S3_SetAngle(-750);  // 小臂伸展
    vTaskDelay(700);
}

// 抬起物体
static void LiftObject(void) {
    AX_SERVO_S2_SetAngle(-500);   // 大臂抬起
    AX_SERVO_S3_SetAngle(-700);  // 小臂弯曲回收
    vTaskDelay(800);
}

// 移动到垃圾桶上方
static void MoveAboveBin(void) {
    //AX_SERVO_S5_SetAngle(400);   // 末端旋转器调整方向
    vTaskDelay(1000);
    
		AX_SERVO_S2_SetAngle(-300); // 大臂角度
    AX_SERVO_S3_SetAngle(-800); // 小臂角度
	
		AX_SERVO_S2_SetAngle(-400); // 大臂角度
    AX_SERVO_S3_SetAngle(-700); // 小臂角度
	
		AX_SERVO_S2_SetAngle(-500); // 大臂角度
    AX_SERVO_S3_SetAngle(-600); // 小臂角度
	
	
    AX_SERVO_S2_SetAngle(-580);    // 大臂定位
    AX_SERVO_S3_SetAngle(-500);  // 小臂定位
    vTaskDelay(800);
}

// 下降到释放位置
static void DescendToBin(void) {
    AX_SERVO_S2_SetAngle(-650);   // 大臂缓慢下降
    AX_SERVO_S3_SetAngle(-650);  // 小臂伸展
    vTaskDelay(700);
}

// 机械臂抓取垃圾
void AX_SERVO_Grab(void) {
    // 阶段1: 移动到垃圾上方
    MoveAboveTrash();
    
    // 阶段2: 下降并准备抓取
    DescendToTrash();
    
    // 阶段3: 抓取垃圾
		AX_SERVO_S6_SetAngle(-900);  // 夹爪全开
    vTaskDelay(1500);
    AX_SERVO_S6_SetAngle(-300);  // 夹爪闭合抓取
    vTaskDelay(1000);               // 确保抓稳
    
    // 阶段4: 抬起物体
    LiftObject();
    
    // 阶段5: 回到安全高度
	AX_SERVO_S2_SetAngle(-500); // 大臂角度
    AX_SERVO_S3_SetAngle(-600); // 小臂角度
	
	AX_SERVO_S2_SetAngle(-400); // 大臂角度
    AX_SERVO_S3_SetAngle(-700); // 小臂角度
	
	AX_SERVO_S2_SetAngle(-300); // 大臂角度
    AX_SERVO_S3_SetAngle(-800); // 小臂角度
	
    SetReadyPose();
}

// 机械臂放下垃圾
void AX_SERVO_Release(void) {
    // 阶段1: 移动到垃圾桶上方
    MoveAboveBin();
    
    // 阶段2: 下降到释放位置
    DescendToBin();
    
    // 阶段3: 释放物体
    AX_SERVO_S6_SetAngle(-900);  // 夹爪全开放下
    vTaskDelay(700);               // 确保完全释放
    
    // 阶段4: 抬起机械臂
    LiftObject();
    
    // 阶段5: 回到初始位置
	AX_SERVO_S2_SetAngle(-500); // 大臂角度
    AX_SERVO_S3_SetAngle(-600); // 小臂角度
	
	AX_SERVO_S2_SetAngle(-400); // 大臂角度
    AX_SERVO_S3_SetAngle(-700); // 小臂角度
	
	AX_SERVO_S2_SetAngle(-300); // 大臂角度
    AX_SERVO_S3_SetAngle(-800); // 小臂角度
	
    SetReadyPose();
    //AX_SERVO_S1_SetAngle(550);   // 底座复位
   // AX_SERVO_S5_SetAngle(650);   // 末端旋转器复位
    vTaskDelay(1000);
}
/******************* (C) 版权 2023 XTARK **************************************/
