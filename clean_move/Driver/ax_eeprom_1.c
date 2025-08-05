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
  * @��  ��  EEPROM�������� 
  *
  ******************************************************************************
  * @˵  ��
  *
  * 
  ******************************************************************************
  */

#include "ax_eeprom.h"
#include "ax_sys.h"
#include "ax_delay.h"

//IO��������	 
#define  IIC_SCL    PEout(2) //SCL
#define  IIC_SDA    PEout(3) //SDA	 
#define  READ_SDA   PEin(3)  //����SDA 

//IIC������������
static void IIC_Start(void);
static void IIC_Stop(void);
static uint8_t IIC_Wait_Ack(void);
static void IIC_Ack(void);
static void IIC_NAck(void);
static void IIC_Send_Byte(uint8_t data);
static uint8_t IIC_Read_Byte(uint8_t ack);

/**
  * @��  ��  EEPROM��ʼ��
  * @��  ��  ��	  
  * @����ֵ  ��
  */
void AX_EEPROM_Init(void)
{
	//IIC��ʼ��
	GPIO_InitTypeDef  GPIO_InitStructure;
	
    //ʹ��GPIOBʱ��
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOE, ENABLE);

	//GPIO��ʼ������ SCL
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��
	
	//GPIO��ʼ������ SDA
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;  //��©���
	GPIO_Init(GPIOE, &GPIO_InitStructure);//��ʼ��

	//STOP�ź�
	IIC_SCL=1; 
	IIC_SDA=1;   
}

/**
  * @��  ��  ָ����ַ����һ������
  * @��  ��  addr :��ʼ�ĵ�ַ, ��24c02Ϊ0~255
  * @����ֵ  ����������
  */
uint8_t AX_EEPROM_ReadOneByte(uint16_t addr)
{				  
	uint8_t temp=0;	
	
	//������ʼ�ź�
    IIC_Start();  
	
	//��ַΪ1�����ֽڵ�ַ + ռ��������ַ��bit1~bit3λ ���ڱ�ʾ��λ��ַ, ���11λ��ַ
	//����24C02, ��������ַ��ʽ(8bit)Ϊ: 1  0  1  0  A2  A1  A0  R/W
	//R/W      : ��/д����λ 0,��ʾд; 1,��ʾ��;
	//A0/A1/A2 : ��Ӧ������1,2,3����(ֻ��24C01/02/04/8����Щ��)
	//a8/a9/a10: ��Ӧ�洢���еĸ�λ��ַ, 11bit��ַ�����Ա�ʾ2048��λ��, ����Ѱַ24C16�����ڵ��ͺ�
	
	//�������� 0xA0 + ��λa8/a9/a10��ַ,д����
	IIC_Send_Byte(0XA0 + ((addr >> 8) << 1) );   	
	IIC_Wait_Ack();
	
	//���͵͵�ַ
    IIC_Send_Byte(addr%256);   
	IIC_Wait_Ack();	    
	
	//���·�����ʼ�ź�
    IIC_Start();  	
	
	//�������ģʽ, IIC�涨���λ��1, ��ʾ��ȡ	 	   
	IIC_Send_Byte(0XA1);       	   
	IIC_Wait_Ack();	
	
	//����һ���ֽ�����
    temp=IIC_Read_Byte(0);	
	
	//����һ��ֹͣ����	
    IIC_Stop();                  
	
	return temp;
}


/**
  * @��  ��  ָ����ַд��һ������
  * @��  ��  addr:��ʼ�ĵ�ַ ��24c02Ϊ0~255
             data:Ҫд�������
  * @����ֵ  ����������
  */
void AX_EEPROM_WriteOneByte(uint16_t addr,uint8_t data)
{	
	//������ʼ�ź�	
    IIC_Start();  
	
	//�������� 0xA0 + ��λa8/a9/a10��ַ,д����
	IIC_Send_Byte(0XA0 + ((addr >> 8) << 1) );   	
	IIC_Wait_Ack();	   
	
	//���͵͵�ַ
    IIC_Send_Byte(addr%256);   
	IIC_Wait_Ack(); 

	//�����ֽ�		
	IIC_Send_Byte(data);   
	IIC_Wait_Ack();  

	//����һ��ֹͣ����
    IIC_Stop(); 
	
	//ע��EEPROM д��Ƚ���,����ȵ�10ms����д��һ���ֽ�
	AX_Delayus(10000);	 
}


/**
  * @��  ��  ����ָ����ַ��ʼ��ָ������������
  * @��  ��  addr:��ʼ�ĵ�ַ ��24c02Ϊ0~255
  *          buf:���������׵�ַ
  *          num:Ҫ�������ݵĸ���
  * @����ֵ  ��
  */
void AX_EEPROM_Read(uint16_t addr,uint8_t *buf, uint16_t num)
{
	while(num)
	{
		*buf ++= AX_EEPROM_ReadOneByte(addr++);	
		
		num--;
	}
}  


/**
  * @��  ��  д��ָ����ַ��ʼ��ָ������������
  * @��  ��  addr:��ʼ�ĵ�ַ ��24c02Ϊ0~255
  *          buf:���������׵�ַ
  *          num:Ҫ�������ݵĸ���
  * @����ֵ  ��
  */
void AX_EEPROM_Write(uint16_t addr, uint8_t *buf, uint16_t num)
{
	while(num--)
	{
		AX_EEPROM_WriteOneByte(addr,*buf);
		
		addr++;
		buf++;
	}
} 



//--------------------------I2C��������-----------------------------------------

/**
  * @��  ��  ����IIC��ʼ�ź�
  * @��  ��  ��
  * @����ֵ  ��
  */
static void IIC_Start(void)
{ 
	
    //START�ź�: ��SCLΪ��ʱ, SDA�Ӹ߱�ɵ�, ��ʾ��ʼ�ź�
	IIC_SDA=1;	  	  
	IIC_SCL=1;
	AX_Delayus(2);

 	IIC_SDA=0;     
	AX_Delayus(2);
	
	//ǯסI2C���ߣ�׼�����ͻ��������
	IIC_SCL=0;      
	AX_Delayus(2);
}


/**
  * @��  ��  ����IICֹͣ�ź�
  * @��  ��  ��
  * @����ֵ  ��
  */
static void IIC_Stop(void)
{
	//STOP�ź�: ��SCLΪ��ʱ, SDA�ӵͱ�ɸ�, ��ʾֹͣ�ź�
	IIC_SDA=0;      
 	AX_Delayus(2);
	
	IIC_SCL=1; 
	AX_Delayus(2);
	
	//����I2C���߽����ź�
	IIC_SDA=1;      
	AX_Delayus(2);							   	
}


/**
  * @��  ��  �ȴ�Ӧ���źŵ���
  * @��  ��  ��
  * @����ֵ  0������Ӧ��ɹ�  1������Ӧ��ʧ��
  */
static uint8_t IIC_Wait_Ack(void)
{
	u8 ucErrTime=0;
	
	//�����ͷ�SDA��(��ʱ�ⲿ������������SDA��)
	IIC_SDA=1;      
	AX_Delayus(2);	 
	
	//SCL=1, ��ʱ�ӻ����Է���ACK
	IIC_SCL=1;      
	AX_Delayus(2);	
	
	//�ȴ�Ӧ��
	while(READ_SDA)
	{
		ucErrTime++;
		
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 1;
		}
	}
	
	//SCL=0, ����ACK���	
	IIC_SCL=0;   
	AX_Delayus(2);
	
	return 0;  
} 

/**
  * @��  ��  ����ACKӦ�����IIC��ʼ�ź�
  * @��  ��  ��
  * @����ֵ  ��
  */
static void IIC_Ack(void)
{

	//SCL 0 -> 1  ʱ SDA = 0,��ʾӦ��
	IIC_SDA=0;
	AX_Delayus(2);	
	
	//����һ��ʱ��
	IIC_SCL=1;
	AX_Delayus(2);
	IIC_SCL=0;
	AX_Delayus(2);
	
	//�����ͷ�SDA��
	IIC_SDA=1;
	AX_Delayus(2);

}
	
/**
  * @��  ��  ����NACKӦ��
  * @��  ��  ��
  * @����ֵ  ��
  */
static void IIC_NAck(void)
{
	//SCL 0 -> 1  ʱ SDA = 1,��ʾ��Ӧ��
	IIC_SDA=1;
	AX_Delayus(2);
	
	//����һ��ʱ��
	IIC_SCL=1;
	AX_Delayus(2);
	IIC_SCL=0;
	AX_Delayus(2);
}	


/**
  * @��  ��  IIC����һ���ֽ�,���شӻ�����Ӧ��
  * @��  ��  data: Ҫ���͵�����
  * @����ֵ  ��
  */
static void IIC_Send_Byte(uint8_t data)
{                        
    uint8_t t;   
	   
    for(t=0; t<8; t++)
    {   
		//��λ�ȷ���
        IIC_SDA=(data&0x80)>>7;
		AX_Delayus(2);  

		//����һ��ʱ��
		IIC_SCL=1;
		AX_Delayus(2); 
		IIC_SCL=0;	
		
		//����1λ,������һ�η���		
		data<<=1; 
    }	
	
	//�������, �����ͷ�SDA��
	IIC_SDA=1;
} 	 


/**
  * @��  ��  IIC��ȡһ���ֽ�
  * @��  ��  ack:  ack=1ʱ������ack; ack=0ʱ������nack
  * @����ֵ  ���յ�������
  */
static uint8_t IIC_Read_Byte(uint8_t ack)
{                        
    uint8_t i, receive = 0;

	//����1���ֽ�����
    for (i = 0; i < 8; i++ )    /*  */
    {
		//��λ�����,�������յ�������λҪ����
        receive <<= 1; 
		
        IIC_SCL = 1;
        AX_Delayus(2);

        if (READ_SDA)
        {
            receive++;
        }
		
        IIC_SCL = 0;
        AX_Delayus(2);
    }

    if (!ack)
    {
        IIC_NAck();     /* ����nACK */
    }
    else
    {
        IIC_Ack();      /* ����ACK */
    }

    return receive;
} 	

/******************* (C) ��Ȩ 2023 XTARK **************************************/
