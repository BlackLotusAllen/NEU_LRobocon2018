#include "status.h"
#include "elmo.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"

/**
* @brief  �ٶ�ת��Ӧ����
* @param  v���趨�ٶ�
* @author ACTION
* @note ��λ m/s
*/
int exchange(float v)
{
	int pulse=0;
	pulse=v*4096/0.335352;
	return pulse;
}

/**
* @brief  ֱ��
* @param  v���趨�ٶ�
* @author ACTION
* @note ��λ m/s
*/
void straight(float v)
{
	int pulse=0;
	pulse=exchange(v);
	VelCrl(CAN2,0x01,pulse);
	VelCrl(CAN2,0x02,-pulse);
}

/**
* @brief  תȦ
* @param  v���趨�ٶ�
* @param  r���趨�뾶
* @param  direction������ת��Left��ת��Right��ת��
* @author ACTION
* @note ��λ v:m/s��r��m �뾶Ϊ�������ĵ�Բ�ľ��룬r>=l/2,l=0.434m
*/
void circular(float v,float r,char direction)
{
	float v_big=0,v_small=0,pulse_big=0,pulse_small=0;
	v_big=v+0.434*v/(2*r);
	v_small=v-0.434*v/(2*r);
	pulse_big=exchange(v_big);
	pulse_small=exchange(v_small);
	switch(direction)
	{
		case Left:	VelCrl(CAN2,0x01,pulse_big);
								VelCrl(CAN2,0x02,-pulse_small);
								break;
		case Right: VelCrl(CAN2,0x02,pulse_big);
								VelCrl(CAN2,0x01,-pulse_small);
								break;
	}
}