/**
  ******************************************************************************
  * @file	  moveBase.c
  * @author	  Action
  * @version   V1.0.0
  * @date	  2018/08/09
  * @brief	 2018省赛底盘运动控制部分
  ******************************************************************************
  * @attention
  *			None
  ******************************************************************************
  */
/* Includes -------------------------------------------------------------------------------------------*/

#include "moveBase.h"
#include <app_cfg.h>
#include "misc.h"
#include "stm32f4xx_gpio.h"
#include "stm32f4xx_rcc.h"
#include "timer.h"
#include "gpio.h"
#include "usart.h"
#include "can.h"
#include "elmo.h"
#include "stm32f4xx_it.h"
#include "stm32f4xx_usart.h"
#include "math.h"
#include "pps.h"
/* Private typedef ------------------------------------------------------------------------------------*/
/* Private define -------------------------------------------------------------------------------------*/
/* Private macro --------------------------------------------------------------------------------------*/
/* Private variables ----------------------------------------------------------------------------------*/
/* Private function prototypes ------------------------------------------------------------------------*/
/* Private functions ----------------------------------------------------------------------------------*/
/**
  * @brief  
  * @note
  * @param  
  * @retval None
  */
#define pi 3.141592f
float Kp=200,Ki=0.01,Kd=0,err=0,lastErr=0,Sumi=0,Output=0,Vk=0,errl,lastErr1,Vkl=0,Sumli=0;
void Straight(float v)
{
	VelCrl(CAN2,5,REDUCTION_RATIO*v*8192/(pi*WHEEL_DIAMETER));
	VelCrl(CAN2,6,-10*Vk*REDUCTION_RATIO);
}	
void Spin(float R,float v)
{
	VelCrl(CAN2,1,v*4096/(pi*WHEEL_DIAMETER)+Vk);
	VelCrl(CAN2,2,-v*4096*(R+(WHEEL_TREAD-WHEEL_WIDTH)/2)/((R-(WHEEL_TREAD-WHEEL_WIDTH)/2)*pi*WHEEL_DIAMETER)+Vk);
}
void TurnRight(float angle,float v)
{
	VelCrl(CAN2,1,0);
	VelCrl(CAN2,2,-v*4096*WHEEL_TREAD*angle*2/((WHEEL_DIAMETER)*360));
}	
void AnglePID(float setAngle,float feedbackAngle)
{
	err=setAngle-feedbackAngle;
	if(err>180)
		err=-(360-err);
	if(err<-180)
		err=360+err;
	Sumi+=Ki*err;
	Vk=Kp*err+Sumi+Kd*(err-lastErr);
	lastErr=err;
}	
float k,b,lAngle,setAngle,x,y,d;
static int flag;
extern float dLeft,dRight;
void GetFunction(float x1,float y1,float x2,float y2)
{
		if(x1-0.1<x2&&x2<x1+0.1)
		{
			if(y2-y1>0)
				lAngle=0;
			else
				lAngle=180;
			flag=0;
		}	
	  	else
		{
			k=(y2-y1)/(x2-x1);
			b=y1-k*x1;
			if(k>0)
			{	
				if(y2-y1>=0)
					lAngle=(atan(k)*180/pi)-90;
				else 
					lAngle=(atan(k)*180/pi)+90;
			}	
			else 
			{
				if(y2-y1>=0)
					lAngle=(atan(k)*180/pi)+90;
				else 
					lAngle=(atan(k)*180/pi)-90;
			}	
			if(y1-0.5<y2&&y2<y1+0.5)
			{	
				if(x2-x1>0)
					lAngle=-90;
				if(x2<x1)
					lAngle=90;
			}	
			flag=1;
		}
}	
void linePID(float x1,float y1,float x2,float y2,float v)
{
		
		GetFunction(x1,y1,x2,y2);
		if(flag)
		{
			if(k>0)
				if((y-k*x-b)*(y2-y1)>0)
					setAngle=lAngle-90*(1-500/(y-k*x-b+500));
				else
					setAngle=lAngle+90*(1-500/(-y+k*x+b+500));
			else
				if((y-k*x-b)*(y2-y1)>0)
					setAngle=lAngle+90*(1-500/(y-k*x-b+500));
				else
					setAngle=lAngle-90*(1-500/(-y+k*x+b+500));
			//斜率为0	
			if(y1-0.5<y2&&y2<y1+0.5)
			{	
				if(x2>x1)
					if(y>y1)
						setAngle=lAngle-90*(1-500/(y-b+500));
					else
						setAngle=lAngle+90*(1-500/(-y+b+500));
				else
					if(y>y1)
						setAngle=lAngle+90*(1-500/(y-b+500));
					else
						setAngle=lAngle-90*(1-500/(-y+b+500));	
			}	
		}	
		//斜率不存在
	 	else
		{	
			if((y2-y1)*(x-x1)<0)
					setAngle=lAngle-90*(1-500/(fabs(x-x1)+500));
			else
					setAngle=lAngle+90*(1-500/(fabs(x-x1)+500));	
		}
		AnglePID(setAngle,GetAngle());
		Straight(v);
		
}	
void CirclePID(float x0,float y0,float R,float v,int status)
{
	GetFunction(x,y,x0,y0);
	d=(x-x0)*(x-x0)+(y-y0)*(y-y0);
	//逆时针
	if(status==0)
	{	
		if(sqrt(d)>R)
			setAngle=lAngle-90*(1000/(fabs(sqrt(d)-R)+1000));	
		else
			setAngle=lAngle-180+90*(1000/(fabs(sqrt(d)-R)+1000));		
	}	
	//顺时针
	if(status==1)
	{	
		if(sqrt(d)>R)
			setAngle=lAngle+90*(1000/(fabs(sqrt(d)-R)+1000));
		else
			setAngle=lAngle+180-90*(1000/(fabs(sqrt(d)-R)+1000));
	}	
	AnglePID(setAngle,GetAngle());
	Straight(v);
}	
/********************* (C) COPYRIGHT NEU_ACTION_2018 ****************END OF FILE************************/
// 发射航向角转换函数 由度转换为脉冲
// yawAngle为角度，范围180到-180之间，初始位置为0度。
// 将角度转换为脉冲
float YawTransform(float yawAngle)
{
	return (yawAngle * YAW_REDUCTION_RATIO * COUNT_PER_DEGREE);
}

//发射航向角控制函数 单位：度（枪顺时针转为正，逆时针为负）
void YawAngleCtr(float yawAngle)
{
	PosCrl(CAN1, GUN_YAW_ID, ABSOLUTE_MODE, YawTransform(yawAngle));
}
// 同样要配置位置环