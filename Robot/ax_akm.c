#include "ax_akm.h"
#include "ax_robot.h"

/**
  * @��  ��  ���ݰ�����ǰ����ת���ٶ�ת��Ϊǰ��ת��Ƕ�
  * @��  ��  vx  ǰ���ٶȣ���λm/s 
  *          vw  ת���ٶȣ���λrad/s
  * @����ֵ  ��ǰ��ת��Ƕȣ���λrad
  */
int16_t AX_AKM_WToAngle(int16_t vx, int16_t vw) 
{
    float akm_angle; //ǰ��ת��Ƕ�
	float radius;  //ת��뾶

	if(vw!=0 && vx!=0)
	{
		//����ת��뾶
		radius =  (float)vx/(float)vw;
		
		//ת��뾶С����Сת��
		if(radius>0 && radius<AKM_TURN_R_MINI)
		{
			radius = AKM_TURN_R_MINI; 
		}
			
		if(radius<0 && radius>(-AKM_TURN_R_MINI))
		{
			radius = -AKM_TURN_R_MINI;
		}		

		//���������ǰ��ת��Ƕ�,��λ����
		akm_angle = atan(AKM_ACLE_BASE/(radius));
	}
	else
	{
		akm_angle = 0;
	}
	
	return (akm_angle*1000);
}
