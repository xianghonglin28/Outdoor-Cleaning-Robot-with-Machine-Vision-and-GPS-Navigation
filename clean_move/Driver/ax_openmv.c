#include "ax_openmv.h"
#include <stdio.h>

uint8_t  Distance=0,X_Coordinate=0,Y_Coordinate=0;
int distance = 0,x_coordinate = 0,y_coordinate = 0;
int Find_Trash = 0;


void OpenMV_Receive_Data(int16_t com_data)
{
		uint8_t i;
		static uint8_t RxCounter1=0;//����
		static uint16_t RxBuffer1[10]={0};
		static uint8_t RxState = 0;	
		
		if(RxState==0&&com_data==0x2C)  //0x2c֡ͷ
		{
			RxState=1;
			RxBuffer1[RxCounter1++]=com_data;
		}
		
		else if(RxState==1&&com_data==0x12)  //0x12֡ͷ 
		{
			RxState=2;
			RxBuffer1[RxCounter1++]=com_data;
		}
		
		else if(RxState==2)
		{
			RxBuffer1[RxCounter1++]=com_data;
			if(RxCounter1>=10||com_data == 0x5B)       //RxBuffer1��������,�������ݽ���
			{
				RxState=3;
				
				Distance = RxBuffer1[RxCounter1-4];
				X_Coordinate = RxBuffer1[RxCounter1-3];
				Y_Coordinate = RxBuffer1[RxCounter1-2];
				distance = Distance;
				x_coordinate = X_Coordinate;
				y_coordinate = Y_Coordinate;
			
				if(x_coordinate > 80)
				{
					x_coordinate = x_coordinate - 256;
				}
				if(y_coordinate > 40)
				{
					y_coordinate = y_coordinate - 256;
				}

			}
	}
		
		else if(RxState==3)		//����Ƿ���ܵ�������־
		{
				if(RxBuffer1[RxCounter1-1] == 0x5B)
				{
							
							RxCounter1 = 0;
							RxState = 0;
				}
				else   //���մ���
				{
							RxState = 0;
							RxCounter1=0;
							for(i=0;i<10;i++)
							{
									RxBuffer1[i]=0x00;      //�����������������
							}
				}
		} 

		else   //�����쳣
		{
				RxState = 0;
				RxCounter1=0;
				for(i=0;i<10;i++)
				{
						RxBuffer1[i]=0x00;      //�����������������
				}
		}
}
