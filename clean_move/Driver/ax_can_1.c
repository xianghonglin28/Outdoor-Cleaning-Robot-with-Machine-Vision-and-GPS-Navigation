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
  * @��  ��  CANͨ������
  *
  ******************************************************************************
  * @˵  ��
  *
  * CAN���Ͷ�����˵��:
  * �����ʣ�1000K 
  * ֡ID�� 0x0181
  * ֡��ʽ������֡
  * DLC��8�ֽ�
  * ����֡Э�飺X�� | X�� | Y�� | Y�� | Z�� | Z�� | Ԥ�� | Ԥ��  
  * 
  ******************************************************************************
  */

#include "ax_can.h"

#include "ax_robot.h"

static CanTxMsg TxMessage;  //����֡
static CanRxMsg RxMessage; //����֡

/**
  * @��  ��  CAN��ʼ��
  * @˵  ��  ������Ϊ1M����Ƶ����6��TSJW=1��TBS1=4��TBS2=2��
  * @����ֵ  ��
  */
void AX_CAN_Init(void)
{

  	GPIO_InitTypeDef GPIO_InitStructure; 
	NVIC_InitTypeDef  NVIC_InitStructure;
	CAN_InitTypeDef        CAN_InitStructure;
  	CAN_FilterInitTypeDef  CAN_FilterInitStructure;

    //ʹ�����ʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);//ʹ��PORTʱ��	                   											 
  	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);//ʹ��CAN1ʱ��	
	
    //��ʼ��GPIO
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0| GPIO_Pin_1;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;//���ù���
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOD, &GPIO_InitStructure);
	
	//���Ÿ���ӳ������
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource0,GPIO_AF_CAN1); 
	GPIO_PinAFConfig(GPIOD,GPIO_PinSource1,GPIO_AF_CAN1); 
	  
  	//CAN��Ԫ����
   	CAN_InitStructure.CAN_TTCM=DISABLE;	   //��ʱ�䴥��ͨ��ģʽ   
  	CAN_InitStructure.CAN_ABOM=DISABLE;	   //����Զ����߹���	  
  	CAN_InitStructure.CAN_AWUM=DISABLE;    //˯��ģʽͨ���������(���CAN->MCR��SLEEPλ)
  	CAN_InitStructure.CAN_NART=ENABLE;	   //��ֹ�����Զ����� 
  	CAN_InitStructure.CAN_RFLM=DISABLE;	   //���Ĳ�����,�µĸ��Ǿɵ�  
  	CAN_InitStructure.CAN_TXFP=DISABLE;	   //���ȼ��ɱ��ı�ʶ������ 
	
  	CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;    //CAN����ģʽ����Ϊ��ͨģʽ
  	CAN_InitStructure.CAN_SJW = CAN_SJW_1tq;	    //����ͬ����Ծ���1��ʱ�䵥λ����Χ:1~3;
  	CAN_InitStructure.CAN_BS1 = CAN_BS1_4tq;        //ʱ���1Ϊ3��ʱ�䵥λ����Χ:1~8;
  	CAN_InitStructure.CAN_BS2 = CAN_BS2_2tq;        //ʱ���2Ϊ2��ʱ�䵥λ����Χ:1~16;
  	CAN_InitStructure.CAN_Prescaler=6;              //�����ʷ�Ƶ�����趨ʱ�䵥λ���ȣ�������=Fpclk1/((tbs1+tbs2+1)*brp),��42M/(6*(1+4+2))=1000K
  	CAN_Init(CAN1, &CAN_InitStructure);             //��ʼ��CAN1 
    
    //���ù�����
 	CAN_FilterInitStructure.CAN_FilterNumber=0;	  //������0
	CAN_FilterInitStructure.CAN_FilterIdHigh=0x0000;  //��������ʶ���ĸ�16λֵ
	CAN_FilterInitStructure.CAN_FilterIdLow=0x0000;  //��������ʶ���ĵ�16λֵ
	CAN_FilterInitStructure.CAN_FilterMaskIdHigh=0x0000;  //���������α�ʶ���ĸ�16λֵ
	CAN_FilterInitStructure.CAN_FilterMaskIdLow=0x0000;  //���������α�ʶ���ĵ�16λֵ		
  	CAN_FilterInitStructure.CAN_FilterMode=CAN_FilterMode_IdMask;  //ָ��������Ϊ��ʶ������λģʽ
  	CAN_FilterInitStructure.CAN_FilterScale=CAN_FilterScale_32bit;   //������λ��Ϊ32
   	CAN_FilterInitStructure.CAN_FilterFIFOAssignment=CAN_Filter_FIFO0;  //�趨��ָ���������FIFOΪ0
  	CAN_FilterInitStructure.CAN_FilterActivation=ENABLE;   //������ʹ��
  	CAN_FilterInit(&CAN_FilterInitStructure);  //�˲�����ʼ��	
	
	//ʹ��FIFO0��Ϣ�Һ��ж�
    CAN_ITConfig(CAN1,CAN_IT_FMP0, ENABLE); 
	
	//NVIC ����
    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;//����1�ж�ͨ��
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=1;//��ռ���ȼ�
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =1;		//�����ȼ�
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ���	
	
} 

/**
  * @��  ��  CAN�ж�
  * @��  ��  ��
  * @����ֵ	 ��
  */
void CAN1_RX0_IRQHandler(void)
{

	//����FIFO0�е�����
    CAN_Receive(CAN1,CAN_FIFO0, &RxMessage); 
	
	//����ΪROS����ģʽ
	ax_control_mode = CTL_ROS;
	
	//��Ϣ����
	if(RxMessage.StdId == ID_CANRX_VEL)  //�ٶ�����
	{
		R_Vel.TG_IX = (int16_t)((RxMessage.Data[0]<<8) | RxMessage.Data[1]);
		R_Vel.TG_IY = (int16_t)((RxMessage.Data[2]<<8) | RxMessage.Data[3]);
		R_Vel.TG_IW = (int16_t)((RxMessage.Data[4]<<8) | RxMessage.Data[5]);
		
		//����ǰ����������ˣ�����ʸ���ٶȼ���ǰ��ת���
		#if (ROBOT_TYPE == ROBOT_AKM)
			ax_akm_angle = AX_AKM_WToAngle(R_Vel.TG_IX, R_Vel.TG_IW);
		#endif
	}
	
//	//�鿴���յ�����
//	printf("ID%02X L%02X DATA%02X %02X %02X %02X %02X %02X %02X %02X\r\n",RxMessage.StdId,RxMessage.DLC,
//	RxMessage.Data[0],RxMessage.Data[1],RxMessage.Data[2],RxMessage.Data[3],
//	RxMessage.Data[4],RxMessage.Data[5],RxMessage.Data[6],RxMessage.Data[7]);		
	
} 


/**
  * @��  ��  CAN ���ͱ�׼֡����
  * @��  ��  *pbuf����������ָ��
  *          len���������ݳ��ȸ�������8
  *          id��֡�ţ�֡���뷶Χ0-0xFF
  * @����ֵ	 �Ƿ������
  */
uint8_t AX_CAN_SendPacket(uint32_t id, uint32_t len, uint8_t *pbuf )
{
	uint8_t i;  

	//��װ����
	TxMessage.StdId=id;	  //��׼��ʶ��
	TxMessage.IDE=CAN_ID_STD;  //ʹ�ñ�׼��ʶ��
	TxMessage.RTR=CAN_RTR_DATA;  //Ϊ����֡
	TxMessage.DLC=len;	 //���ݳ���,��λΪ�ֽ�

	for(i=0; i<len; i++)
	{
		TxMessage.Data[i] = *(pbuf+i);
	}

	//��������	
	return (CAN_Transmit(CAN1,&TxMessage)); 
}


/******************* (C) ��Ȩ 2023 XTARK **************************************/
