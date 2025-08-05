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
  * @��  ��  MPU6050 ����
  *
  ******************************************************************************
  * @˵  ��
  *
  * 1.MPU6050������ٶȴ�����������������
  * 2.IICͨ�Ų���IO��ģ�ⷽʽ
  *
  ******************************************************************************
  */

#include "ax_mpu6050.h" 
#include "ax_sys.h"
#include "ax_delay.h"

//IO��������
#define SDA_IN()  {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=0<<7*2;}	//����ģʽ
#define SDA_OUT() {GPIOB->MODER&=~(3<<(7*2));GPIOB->MODER|=1<<7*2;} //���ģʽ

//IO��������	 
#define IIC_SCL    PBout(6) //SCL
#define IIC_SDA    PBout(7) //SDA	 
#define READ_SDA   PBin(7)  //����SDA 

//��������
static void MPU6050_WriteRegister(uint8_t reg_address, uint8_t data);
static void MPU6050_ReadRegister(uint8_t reg_address, uint8_t *pdata, uint16_t len);

/**
  * @��  ��  MPU6050��������ʼ��
  * @��  ��  ��	  
  * @����ֵ  ��
  */
void AX_MPU6050_Init(void)
{	
	//��ʼ��I2C2
	GPIO_InitTypeDef  GPIO_InitStructure;

	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);//ʹ��GPIOBʱ��

	//SCL����Ϊ�������
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;//��ͨ���ģʽ
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;//100MHz
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;//����
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
	//SDA����Ϊ��©���
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;//�������
    GPIO_Init(GPIOB, &GPIO_InitStructure);//��ʼ��
	
    IIC_SCL=1;
    IIC_SDA=1;
	
	//��λ����MPU6050
    MPU6050_WriteRegister(MPU6050_PWR_MGMT1_REG,0x80);      //��λ
	AX_Delayus(100000);  	//�ȴ��������ȶ�
	MPU6050_WriteRegister(MPU6050_PWR_MGMT1_REG,0x00);      //�������״̬
	
	//����MPU6050�Ĵ���  
	MPU6050_WriteRegister(MPU6050_GYRO_CFG_REG,0x18);      //���������� Ĭ��2000deg/s
    MPU6050_WriteRegister(MPU6050_ACCEL_CFG_REG,0x00);     //���ټ����� Ĭ��2g	
	MPU6050_WriteRegister(MPU6050_INT_EN_REG,0x00);      //�ر��ж�
	MPU6050_WriteRegister(MPU6050_FIFO_EN_REG,0x00);       //�ر�FIFO
	
	MPU6050_WriteRegister(MPU6050_PWR_MGMT1_REG,0x01);      //����CLKSEL,PLL X��Ϊ�ο�
	MPU6050_WriteRegister(MPU6050_PWR_MGMT2_REG,0x00); 	    //���ٶ��������Ƕ�����	

}


/**
  * @��  ��  MPU6050���ü��ٶ����� 
  * @��  ��  range�� ���ٶ�����2g��4g��8g��16g
  *          �����õļ��ٶ�����ACC_RANGE_2G��ACC_RANGE_4G��ACC_RANGE_8G��ACC_RANGE_16G
  * @����ֵ  ��
  */
void AX_MPU6050_SetAccRange(uint8_t range)
{
	MPU6050_WriteRegister(MPU6050_ACCEL_CFG_REG,range<<3);
}


/**
  * @��  ��  MPU6050��������������
  * @��  ��  range ����������250��/S��500��/S��1000��/S��2000��/S
  *          �����õ�����������GYRO_RANGE_250��GYRO_RANGE_500��GYRO_RANGE_1000��GYRO_RANGE_2000	
  * @����ֵ  ��
  */
void AX_MPU6050_SetGyroRange(uint8_t range)
{
	MPU6050_WriteRegister(MPU6050_GYRO_CFG_REG,range<<3);
}

/**
  * @��  ��  MPU6050���������ǲ�����
  * @��  ��  smplrate �����ǲ����ʣ���Χ10~1000Hz	  
  * @����ֵ  ��
  */
void AX_MPU6050_SetGyroSmplRate(uint16_t smplrate)
{	
	if(smplrate>1000)
		smplrate = 1000;
	if(smplrate<10)
		smplrate = 10;
	
	MPU6050_WriteRegister(MPU6050_SAMPLE_RATE_REG,(uint8_t)(1000/smplrate -1));	
}

/**
  * @��  ��  MPU6050���õ�ͨ�˲�������
  * @��  ��  bandwidth ��ͨ�˲�������
  *          �����õĴ��� DLPF_ACC184_GYRO188��DLPF_ACC94_GYRO98��DLPF_ACC44_GYRO42��
  *                        DLPF_ACC21_GYRO20��DLPF_ACC10_GYRO10��DLPF_ACC5_GYRO5
  * @����ֵ  ��
  */
void AX_MPU6050_SetDLPF(uint8_t bandwidth)
{
	MPU6050_WriteRegister(MPU6050_CFG_REG,bandwidth);
}

/**
  * @��  ��  MPU6050��ȡ�������¶�ֵ
  * @��  ��  ��	  
  * @����ֵ  �������¶�ֵ��
  */
float AX_MPU6050_GetTempValue(void)
{	
	uint8_t buf[2];
	int16_t tmp;

	MPU6050_ReadRegister(MPU6050_TEMP_OUTH_REG,buf,2);

	tmp = (buf[0]<<8)| buf[1];
	
	return ( 36.53f + ((double)tmp/340.0f) );	
}


/**
  * @��  ��  MPU6050��ȡ������ٶȼĴ������ֵ
  * @��  ��  pbuf����ȡ�����ݻ�����ָ�� 
  * @����ֵ  ��
  */
void AX_MPU6050_GetAccData(int16_t *pbuf)
{	
	uint8_t buf[6];
	
	//��ȡ���ٶ�����
	MPU6050_ReadRegister(MPU6050_ACCEL_XOUTH_REG,buf,6);
	
    pbuf[0] = (buf[0] << 8) | buf[1];
    pbuf[1] = (buf[2] << 8) | buf[3];
    pbuf[2] = (buf[4] << 8) | buf[5];	
}

/**
  * @��  ��  MPU6050��ȡ���������ǼĴ������ֵ
  * @��  ��  pbuf����ȡ�����ݻ�����ָ�� 
  * @����ֵ  ��
  */
void AX_MPU6050_GetGyroData(int16_t *pbuf)
{	
	uint8_t buf[6];
	
	//��ȡ����������
	MPU6050_ReadRegister(MPU6050_GYRO_XOUTH_REG,buf,6);
    pbuf[0] = (buf[0] << 8) | buf[1];
    pbuf[1] = (buf[2] << 8) | buf[3];
    pbuf[2] = (buf[4] << 8) | buf[5];	
}


//--------------------------I2C ��ʼ����������-----------------------------------------

/**
  * @��  ��  ����IIC��ʼ�ź�
  * @��  ��  ��
  * @����ֵ  ����״̬
  */
static uint8_t IIC_Start(void)
{
	//SDA�����
	SDA_OUT();     
	
	//START�ź�: ��SCLΪ��ʱ, SDA�Ӹ߱�ɵ�, ��ʾ��ʼ�ź�
	IIC_SDA=1;
	
	if(!READ_SDA)
	{
		return 0;	
	}
		
	IIC_SCL=1;
	AX_Delayus(1);
	
 	IIC_SDA=0;
	if(READ_SDA)
		return 0;
	
	AX_Delayus(1);
	
	//ǯסI2C���ߣ�׼�����ͻ�������� 
	IIC_SCL=0;
	
	return 1;
}

/**
  * @��  ��  ����IICֹͣ�ź�
  * @��  ��  ��
  * @����ֵ  ��
  */	  
static void IIC_Stop(void)
{
	//SDA�����
	SDA_OUT(); 
	
	//STOP�ź�: ��SCLΪ��ʱ, SDA�ӵͱ�ɸ�, ��ʾֹͣ�ź�
	IIC_SCL=0;
	IIC_SDA=0;
 	AX_Delayus(1);
	
	//����I2C���߽����ź�
	IIC_SCL=1; 
	IIC_SDA=1;
	AX_Delayus(1);							   	
}

/**
  * @��  ��  �ȴ�Ӧ���źŵ���
  * @��  ��  ��
  * @����ֵ  0������Ӧ��ɹ�  1������Ӧ��ʧ��
  */
static uint8_t  IIC_Wait_Ack(void)
{
	uint8_t ucErrTime=0;
	
	//SDA����Ϊ����  
	SDA_IN();     
	
	//�����ͷ�SDA��(��ʱ�ⲿ������������SDA��)
	IIC_SDA=1;
	AX_Delayus(1);	
	
	//SCL=1, ��ʱ�ӻ����Է���ACK
	IIC_SCL=1;
	AX_Delayus(1);	
	
	//�ȴ�Ӧ��
	while(READ_SDA)
	{
		ucErrTime++;
		if(ucErrTime>250)
		{
			IIC_Stop();
			return 0;
		}
		
	    //AX_Delayus(1);
	}
	
	//SCL=0, ����ACK���	
	IIC_SCL=0;
	AX_Delayus(1);
	
	return 1;  
} 

/**
  * @��  ��  ����ACKӦ�����IIC��ʼ�ź�
  * @��  ��  ��
  * @����ֵ  ��
  */
static void IIC_Ack(void)
{
	IIC_SCL=0;
	
	//SDA�����
	SDA_OUT();
	
	IIC_SDA=0;
	AX_Delayus(1);
	
	IIC_SCL=1;
	AX_Delayus(1);
	IIC_SCL=0;
}

/**
  * @��  ��  ����NACKӦ��
  * @��  ��  ��
  * @����ֵ  ��
  */	    
static void IIC_NAck(void)
{
	IIC_SCL=0;
	
	//SDA�����
	SDA_OUT();
	
	IIC_SDA=1;
	AX_Delayus(1);
	
	IIC_SCL=1;
	AX_Delayus(1);
	IIC_SCL=0;
}

/**
  * @��  ��  IIC����һ���ֽ�,���شӻ�����Ӧ��
  * @��  ��  data: Ҫ���͵�����
  * @����ֵ  ��
  */		  
static void IIC_Send_Byte(uint8_t data)
{                        
    uint8_t t;   
	
	//SDA�����
	SDA_OUT();

    //����ʱ�ӿ�ʼ���ݴ���
    IIC_SCL=0;
	
    for(t=0; t<8; t++)
    {   
		//��λ�ȷ���		
        IIC_SDA=(data&0x80)>>7;
  
		AX_Delayus(1);   
		IIC_SCL=1;
		AX_Delayus(1); 
		IIC_SCL=0;	
		AX_Delayus(1);
		
	    //����1λ,������һ�η���		
		data<<=1; 
    }	 
} 	 
  
/**
  * @��  ��  IIC��ȡһ���ֽ�
  * @��  ��  ack:  ack=1ʱ������ack; ack=0ʱ������nack
  * @����ֵ  ���յ�������
  */
static uint8_t IIC_Read_Byte(uint8_t ack)
{
	unsigned char i,receive=0;
	
	//SDA����Ϊ����
	SDA_IN();
	
	//����1���ֽ�����
    for(i=0;i<8;i++ )
	{
		//��λ�����,�������յ�������λҪ����
		receive<<=1;
		
        IIC_SCL=0; 
        AX_Delayus(2);
		IIC_SCL=1;
        
        if(READ_SDA)
		{
			receive++; 
		}
		
		AX_Delayus(2); 
    }	
	
    if (ack)
    {
        IIC_Ack();  //����ACK
    }
    else
    {
        IIC_NAck();   //����NACK
    }

    return receive;
}


/**
  * @��  ��  д��ָ����ַָ����������
  * @��  ��  dev_addr���豸��ַ
  *          reg_addr���Ĵ�����ַ
  *          len�����ݳ���
  *          data����������
  * @����ֵ  ���ؽ��
  */
static uint8_t IIC_Write(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, const uint8_t *data)
{
	int i;
	
    if (!IIC_Start())
        return 1;
	
    IIC_Send_Byte(dev_addr << 1 );
	
    if (!IIC_Wait_Ack()) 
	{
        IIC_Stop();
        return 1;
    }
	
    IIC_Send_Byte(reg_addr);
    IIC_Wait_Ack();
	
	for (i = 0; i < len; i++) 
	{
        IIC_Send_Byte(data[i]);
        if (!IIC_Wait_Ack()) 
		{
            IIC_Stop();
            return 0;
        }
    }
	
    IIC_Stop();
    return 0;
}

/**
  * @��  ��  ��ȡָ����ַָ����������
  * @��  ��  dev_addr���豸��ַ
  *          reg_addr���Ĵ�����ַ
  *          len�����ݳ���
  *          data����������
  * @����ֵ  ���ؽ��
  */
static uint8_t IIC_Read(uint8_t dev_addr, uint8_t reg_addr, uint8_t len, uint8_t *data)
{
    if (!IIC_Start())
        return 1;
	
    IIC_Send_Byte(dev_addr << 1);
    if (!IIC_Wait_Ack()) 
	{
        IIC_Stop();
        return 1;
    }
	
    IIC_Send_Byte(reg_addr);
    IIC_Wait_Ack();
	
    IIC_Start();
    IIC_Send_Byte((dev_addr << 1)+1);
    IIC_Wait_Ack();
    while (len) 
	{
        if (len == 1)
            *data = IIC_Read_Byte(0);
        else
            *data = IIC_Read_Byte(1);
		
        data++;
        len--;
    }
	
    IIC_Stop();
    return 0;
}

/**
  * @��  ��  MPU6050д�Ĵ�����
  * @��  ��  reg_addr���Ĵ�����ַ
  *          pdata����������
  *          len�����ݳ���
  * @����ֵ  ��
  */
static void MPU6050_WriteRegister(uint8_t reg_address, uint8_t data)
{
	IIC_Write(MPU6050_ADDR,reg_address,1,&data);
}

/**
  * @��  ��  MPU6050���Ĵ�����
  * @��  ��  ��	  
  * @����ֵ  ��
  */
static void MPU6050_ReadRegister(uint8_t reg_address, uint8_t *pdata, uint16_t len)
{
	IIC_Read(MPU6050_ADDR,reg_address,len,pdata);
}


/******************* (C) ��Ȩ 2023 XTARK **************************************/
