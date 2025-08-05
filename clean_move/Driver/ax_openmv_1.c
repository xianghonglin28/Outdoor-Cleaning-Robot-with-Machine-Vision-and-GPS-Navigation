#include "ax_openmv.h"
#include <stdio.h>



uint8_t  Distance=0,X_Coordinate=0,Y_Coordinate=0;
int distance = 0,x_coordinate = 0,y_coordinate = 0;
int Find_Trash = 0;


void OpenMV_Receive_Data(int16_t com_data)
{
		uint8_t i;
		static uint8_t RxCounter1=0;//计数
		static uint16_t RxBuffer1[6]={0};
		static uint8_t RxState = 0;	
		
		if(RxState==0&&com_data==0x2C)  //0x2c帧头
		{
			RxState=1;
			RxBuffer1[RxCounter1++]=com_data;
		}
		
		else if(RxState==1&&com_data==0x12)  //0x12帧头 
		{
			RxState=2;
			RxBuffer1[RxCounter1++]=com_data;
		}
		
		else if(RxState==2)
		{
			RxBuffer1[RxCounter1++]=com_data;
			if(RxCounter1>=6||com_data == 0x5B)       //RxBuffer1接受满了,接收数据结束
			{
				RxState=3;
				
				Distance = RxBuffer1[2];
				X_Coordinate = RxBuffer1[3];
				Y_Coordinate = RxBuffer1[4];
				distance = Distance;
				x_coordinate = X_Coordinate;
				y_coordinate = Y_Coordinate;
				
//						 printf("===> Parsed: D=%d, X=%d, Y=%d\r\n", distance, x_coordinate, y_coordinate);
				
				if(x_coordinate > 80)
				{
					x_coordinate = x_coordinate - 256;
				}
				if(y_coordinate > 40)
				{
					y_coordinate = y_coordinate - 256;
				}
				
//				if( distance > 40 && (x_coordinate < 50 || x_coordinate > -50))
//				{
//					Find_Trash = 1;
////					Game_start_flag = 2;
//				}
//				if(distance < 40 && (x_coordinate < 20 || x_coordinate > -20))
//				{
//					Find_Trash = 2;
////					Game_start_flag = 3;
//				}
			}
	}
		
		else if(RxState==3)		//检测是否接受到结束标志
		{
				if(RxBuffer1[RxCounter1-1] == 0x5B)
				{
							
							RxCounter1 = 0;
							RxState = 0;
				}
				else   //接收错误
				{
							RxState = 0;
							RxCounter1=0;
							for(i=0;i<10;i++)
							{
									RxBuffer1[i]=0x00;      //将存放数据数组清零
							}
				}
		} 

		else   //接收异常
		{
				RxState = 0;
				RxCounter1=0;
				for(i=0;i<10;i++)
				{
						RxBuffer1[i]=0x00;      //将存放数据数组清零
				}
		}
}
