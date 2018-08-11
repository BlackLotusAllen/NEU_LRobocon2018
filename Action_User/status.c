#include "status.h"
#include "elmo.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"
#include "timer.h"
#include "misc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
/**
* @brief  �ٶ�ת��Ӧ����
* @param  v���趨�ٶ�
* @author ACTION
* @note ��λ m/s
*/
int exchange(float v)
{
	int pulse=0;
	pulse=(int)(v*4096/0.335352);
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


struct position
{
	float x;
	float y;
	float angle;
}pos_t;
void SetAngle(float val)
{
	pos_t.angle=val;
}

void SetXpos(float val)
{
	pos_t.x=val;
}

void SetYpos(float val)
{
	pos_t.y=val;
}

float GetXpos(void)
{
	return pos_t.x;
}

float GetYpos(void)
{
	return pos_t.y;
}

float GetAngle(void)
{
	return pos_t.angle;
}

float LastAngleErr=0;
float AnglePID(float Angle,float SetAngle)
{
	struct PID Ang;
	Ang.p=10;
	Ang.i=0;
	Ang.d=1;
	float err=0,u=0,i=0;
	err=SetAngle-Angle;
	i+=err;
	u=Ang.p*err+Ang.i*i+Ang.d*(err-LastAngleErr);
	LastAngleErr=err;
	return u;
}

//���³�1ר��
int isOKFlag=0;
int isSendOK(void)
{
	return isOKFlag;
}

void SetOKFlagZero(void)
{
	isOKFlag=0;
}

void driveGyro(void)
{
	while(!isOKFlag)
	{
		TIM_Delayms(TIM4,5);
		USART_SendData(USART3,'A');
		USART_SendData(USART3,'T');
		USART_SendData(USART3,'\r');
		USART_SendData(USART3,'\n');
	}
	SetOKFlagZero();
}
