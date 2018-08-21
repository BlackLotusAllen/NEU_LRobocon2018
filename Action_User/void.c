#include "void.h"
#include "elmo.h"
#include "can.h"
#include "usart.h"
#include "gpio.h"
#include "timer.h"
#include "misc.h"
#include "stm32f4xx_rcc.h"
#include "stm32f4xx_gpio.h"
#include "math.h"
#include "movebase.h"
#include "pps.h"
#define Pulse2mm COUNTS_PER_ROUND/(WHEEL_DIAMETER*Pi)
struct position
{
	float x;
	float y;
	float angle;
}pos_t;
/////////////////////////脉冲转速度////////////////////////////////
int exchange(float v)
{
	int pulse=0;
	pulse=(int)(v*4096/0.335352);
	return pulse;
}
//////////////////AngPID//////////////////////
float AnglePID(float Angle,float SetAngle)
{
	struct PID Ang;
	Ang.p=300;
	Ang.i=0;
	Ang.d=0;
	float err=0,u=0,err1=0,err2=0;
	err1=SetAngle-Angle;
	//////////////////劣弧调节//////////////////
	if(err1>=0)
		err2=err1-360;
	else
		err2=360+err1;
	if(err1>180||err1<-180)
	{
		err=err2;
	}
	else
	{
		err=err1;
	}
	u=Ang.p*err;
	return u;
}

//////////////////////////方向PID///////////////////////////
float DirectionPID( float distance,float setdistance )
{
	struct PID Dir;
	float u=0,errd=0;
	errd=setdistance-distance;
	if(distance<300&&distance>-300)
	{
		Dir.p=0.06;
		u=Dir.p*errd;
	}
	else if(distance<500&&distance>-500)
	{
		Dir.p=0.1;
		u=Dir.p*errd-12;
	}
	else
	{
		Dir.p=0.15;
		u=Dir.p*errd-37;
	}
	return u;
}
//////////////绕圈///////////////
void Walkline( int setx,int sety, int r,int direction , float v )
{
	int vright=0,vleft=0;
	float AngPID=0,DirPID=0;
	float SetAngle=0,distance=0,k=0,Ang=0,diff=0,anglek=0;
	float x=0,y=0;
	Ang=GetAngle();
	x=GetX();
	y=GetY();
////////读取角度
	distance=sqrt(((x-setx)*(x-setx)+(y-sety)*(y-sety)));   ///////distance取值
	if((sety-y)!=0&&(setx-x)!=0)
	{
		k=(sety-y)/(setx-x);
		anglek=atan(k)*180/pi;
		if(direction==1)
		{
			if(setx>x&&sety>y)
			{
				SetAngle=anglek-180;
			}
			else if(setx>x&&sety<y)
			{
				SetAngle=180+anglek;
			}
			else if(setx<x&&sety<y)
			{
				SetAngle=anglek;
			}
			else if(setx<x&&sety>y)
			{
				SetAngle=anglek;
			}
		}
		if(direction==2)
		{
			if(setx>x&&sety>y)
			{
				SetAngle=anglek;
			}
			else if(setx>x&&sety<y)
			{
				SetAngle=anglek;
			}
			else if(setx<x&&sety<y)
			{
				SetAngle=anglek-180;
			}
			else if(setx<x&&sety>y)
			{
				SetAngle=anglek+180;
			}		
		}
	}
	else if(sety==y)
	{
		if(direction==1)
		{
			if(setx<x)
			{
				SetAngle=-180;
			}
			else if(setx>x)
			{
				SetAngle=0;
			}
		}
		if(direction==2)
		{
			if(setx<x)
			{
				SetAngle=0;
			}
			else if(setx>x)
			{
				SetAngle=-180;
			}
		}
	}
		else if(setx==x)
	{
		if(distance==1)
		{
			if(sety<y)
			{
				SetAngle=-90;
			}
			if(sety>y)
			{
				SetAngle=90;
			}
		}
		if(direction==2)
		{
			if(sety<y)
			{
				SetAngle=90;
			}
			if(sety>y)
			{
				SetAngle=-90;
			}			
		}
	}
	DirPID=DirectionPID(distance,r);
	if(DirPID>70)
		DirPID=70;
	else if(DirPID<-70)
		DirPID=-70;
///////////////////////角度限制///////////////////////
	if(direction==1)
	{
		SetAngle-=DirPID;
	}
	else if(direction==2)
	{
		SetAngle+=DirPID;
	}
	AngPID=AnglePID(Ang,SetAngle);
	diff=AngPID;
//	USART_OUT(UART4,(uint8_t*) "diff:%d	distance:%d	%d	%d\r\n",(int)(diff),(int)(distance),(int)(GetXpos()),(int)(GetYpos()));   ///////////////////////////////////test//////////////////////////
	vright=(int)(exchange(v)+diff);
	vleft=(int)(-exchange(v)+diff);
		VelCrl(CAN2,0x01,vright);
		VelCrl(CAN2,0x02,vleft);
}

void Walkback(float v)
{
	VelCrl(CAN2,0x01,-exchange(v)+2000);
	VelCrl(CAN2,0x02,exchange(v)+2000);
}

int AdcFlag(void)
{
		int adc_num1,adc_num2,AdcFLAG;
		adc_num1=Get_Adc_Average(15,30);	///左轮
		adc_num2=Get_Adc_Average(14,30);  ///右轮
		if(adc_num1>20&&adc_num1<600&&AdcFLAG==0)
		{
			AdcFLAG=2;  ////顺时针
		}
		else if(adc_num2>40&&adc_num2<600&&AdcFLAG==0)
		{
			AdcFLAG=1;  ////逆时针
		}
		USART_OUT(UART4,(uint8_t*)"%d	%d	%d\n",adc_num1,adc_num2,AdcFLAG);	
		return AdcFLAG;
}
extern int errFlag;
int Radius(void)
{
		float LastAngle;
		int r=2000;    ////初始半径
		if(errFlag)
		{
			if(r>700)
			{
				r-=200;
			}
			if(r<=700)
			{
				r+=200;
			}
		}
		if(GetAngle()>0&&LastAngle<0)
		{
			if(r>=700)
			{
				r-=100;
			}
			if(r<700)
			{
				r+=100;
			}
		}
		LastAngle=GetAngle();
		return(r);
}
int errFlag;
void errdeal(void)
{
		int Lastx,Lasty,errtime;
		errFlag=0;
		if((Lastx==(int)GetX())&&(Lasty==(int)GetY())&&AdcFlag()!=0)
		{
			errtime++;
		}
		else if(Lastx!=(int)GetX||Lasty!=(int)GetY)
		{
			errtime=0;
		}
			if(errtime>100)   
			{
				for(int i=0;i<2000;i++)
				{
					Walkback(0.7);
					i++;
				}
				errtime=0;
				errFlag=1;
			}			
		Lastx=(int)GetX();
		Lasty=(int)GetY();
		//	USART_OUT(UART4,(uint8_t*)"%d\n",errtime);
}