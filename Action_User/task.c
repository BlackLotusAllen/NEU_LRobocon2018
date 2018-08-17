#include "includes.h"
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
#include "moveBase.h"
#include "math.h"
#define Pi 3.1415926
int DPID=0;
int DDD=0;

extern char pposokflag;

/*
===============================================================
						信号量定义
===============================================================
*/
OS_EXT INT8U OSCPUUsage;
OS_EVENT *PeriodSem;

static OS_STK App_ConfigStk[Config_TASK_START_STK_SIZE];
static OS_STK WalkTaskStk[Walk_TASK_STK_SIZE];
//static OS_STK Walk_VirerStk[Walk_Virer_TASK_STK_SIZE];
//static OS_STK Walk_CircleStk[Walk_Circle_TASK_STK_SIZE];
//static OS_STK Walk_StraightStk[Walk_Straight_TASK_STK_SIZE];

void App_Task()
{
	CPU_INT08U os_err;
	os_err = os_err; /*防止警告...*/

	/*创建信号量*/
	PeriodSem = OSSemCreate(0);

	/*创建任务*/
	os_err = OSTaskCreate((void (*)(void *))ConfigTask, /*初始化任务*/
						  (void *)0,
						  (OS_STK *)&App_ConfigStk[Config_TASK_START_STK_SIZE - 1],
						  (INT8U)Config_TASK_START_PRIO);

	os_err = OSTaskCreate((void (*)(void *))WalkTask,
						  (void *)0,
						  (OS_STK *)&WalkTaskStk[Walk_TASK_STK_SIZE - 1],
						  (INT8U)Walk_TASK_PRIO);

//						  (void *)0,
//						  (OS_STK *)&Walk_CircleStk[Walk_Circle_TASK_STK_SIZE - 1],
//						  (INT8U)Walk_Circle_TASK_PRIO);
//	os_err = OSTaskCreate((void (*)(void *))Walk_Straight,
//						  (void *)0,
//						  (OS_STK *)&Walk_StraightStk[Walk_Straight_TASK_STK_SIZE - 1],
//						  (INT8U)Walk_Straight_TASK_PRIO);
}

/*
   ===============================================================
   初始化任务
   ===============================================================
   */
void ConfigTask(void)
{
	CPU_INT08U os_err;
	os_err = os_err;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	USART3_Init(115200);
	UART4_Init(921600);
	TIM_Init(TIM2,999,83,1,0);
	CAN_Config(CAN1,500,GPIOB,GPIO_Pin_8,GPIO_Pin_9);
	CAN_Config(CAN2,500,GPIOB,GPIO_Pin_5,GPIO_Pin_6);
	ElmoInit(CAN2);
	VelLoopCfg(CAN2,1,0,0);
	VelLoopCfg(CAN2,2,0,0);
	MotorOn(CAN2,1);
	MotorOn(CAN2,2);
    delay_s(2);
	#if CARNUM == 1
	driveGyro();
	while(!pposokflag);
	#elif CARNUM == 4
	delay_s(10);
	#endif
	
	
	OSTaskSuspend(OS_PRIO_SELF);
}


void  Walk_Straight(float speed1,float speed2)
{
	VelCrl(CAN2,1,speed1);
	VelCrl(CAN2,2,-speed2);
}

 
float Angle_Pid(float err)
{
	static float Iterm=0;
	float Dterm=0,lasterr=0,Uk=0;
	if(err>=180)
		err-=360;
	else if(err<=-180)
		err+=360;
	Iterm +=Ki*err;
	Dterm=Kd*(err-lasterr);
	lasterr=err;
	Uk=Kp*err+Iterm+Dterm;
	return Uk;
}
float Distance_PID(float err)
{
	static float Iterm=0;
	float Dterm=0,lasterr=0,Uk=0;
	Iterm+=KI*err;
	Dterm=KD*(err-lasterr);
	lasterr=err;
	Uk=KP*err+Iterm+Dterm;
	return Uk;
}
int PID_OUT1(int a,int b,int c,int dir,float x,float y)
{	static float d=0;
	d=fabs(a*x+b*y+c)/sqrtf(a*a+b*b);
	static float OUT=0;
	DPID=Distance_PID(0-d);
	if(DPID>=90)
		DPID=90;
	else if(DPID<=-90)
		DPID=-90;
	DDD=d;
if(dir==1)
{
	if(y>(-a*x-c)/b)
	{
		OUT=Angle_Pid(Distance_PID(0-d)+((atan(-a/b)*180/Pi-90)-GetAngle()));
	}
	if(y<(-a*x-c)/b)
	{
	
		OUT=Angle_Pid(-Distance_PID(0-d)+((atan(-a/b)*180/Pi-90)-GetAngle()));
	
	}

}
else if(dir==2)
{
	if(y>(-a*x-c)/b)
	{
	OUT=Angle_Pid(Distance_PID(0-d)+((atan(-a/b)*180/Pi+90)-GetAngle()));
	}
	if(y<(-a*x-c)/b)
	{
	OUT=Angle_Pid(-Distance_PID(0-d)+((atan(-a/b)*180/Pi+90)-GetAngle()));
	}
}
else if(dir==3)
{
	if(y>(-a*x-c)/b)
	{
	OUT=Angle_Pid(Distance_PID(0-d)+((atan(-a/b)*180/Pi+90)-GetAngle()));
	}
	if(y<(-a*x-c)/b)
	{
	OUT=Angle_Pid(-Distance_PID(0-d)+((atan(-a/b)*180/Pi+90)-GetAngle()));
	}
}
else if(dir==4)
{
	if(y>(-a*x-c)/b)
	{
	OUT=Angle_Pid(Distance_PID(0-d)+((atan(-a/b)*180/Pi-90)-GetAngle()));
	}
	if(y<(-a*x-c)/b)
	{
	OUT=Angle_Pid(-Distance_PID(0-d)+((atan(-a/b)*180/Pi-90)-GetAngle()));
	}
}
else if (dir==5)
{
	if(y>(-a*x-c)/b)
	{
	OUT=Angle_Pid(Distance_PID(0-GetYpos())+(-90-GetAngle()));
	}
	if(y<(-a*x-c)/b)
	{
	OUT=Angle_Pid(-Distance_PID(0-GetYpos())+(-90-GetAngle()));
	}
}
else if (dir==6)
{
	if(y>(-a*x-c)/b)
	{
	OUT=Angle_Pid(Distance_PID(0-GetYpos())+(90-GetAngle()));
	}
	if(y<(-a*x-c)/b)
	{
	OUT=Angle_Pid(-Distance_PID(0-GetYpos())+(90-GetAngle()));
	}
}
else if(dir==7)
{
	if(y>(-a*x-c)/b)
	{
	OUT=Angle_Pid(Distance_PID(0-GetXpos())+(0-GetAngle()));
	}
	if(y<(-a*x-c)/b)
	{
	OUT=Angle_Pid(-Distance_PID(0-GetXpos())+(0-GetAngle()));
	}
}
else if (dir==8)
{
	if(x<-c)
	{
	OUT=Angle_Pid(-Distance_PID(0-d)+(180-GetAngle()));
	}
	if(x>-c)
	{
	OUT=Angle_Pid(Distance_PID(0-d)+(180-GetAngle()));
	}
}

//	if(GetAngle()>0)
//	OUT=Angle_Pid(Distance_PID(-c-GetXpos())+(180-GetAngle()));
//	if(GetAngle()<0)
//	OUT=Angle_Pid(Distance_PID(-c-GetXpos())+(-180-GetAngle()));
	
		

return OUT;
}



void WalkTask(void)
{
//	 static int cnt=0;
	CPU_INT08U os_err;
	os_err = os_err;
	OSSemSet(PeriodSem, 0, &os_err);
	while (1)
	{	

		OSSemPend(PeriodSem, 0, &os_err);
//	static float PID_OUT=0;
		USART_OUT(UART4,(uint8_t*) "%d\t",(int)GetAngle());
		USART_OUT(UART4,(uint8_t*) "%d\t",(int)GetXpos());
		USART_OUT(UART4,(uint8_t*) "%d\t",(int)GetYpos());
		USART_OUT(UART4,(uint8_t*) "%d\t",(int)DDD);
		USART_OUT(UART4,(uint8_t*) "%d\r\n",(int)DPID);
		//USART_OUT(UART4,(uint8_t*) "%d\r\n",(int)PID_OUT1(1,-1,0,1,GetXpos(),GetYpos()));
		//PID_OUT=Angle_Pid(Distance_PID(0-Distance(GetXpos(),GetYpos(),1,-1,0))+((atan(1/1)*180/Pi-90)-GetAngle()));
//		Walk_Straight(3000+PID_OUT,3000-PID_OUT);
	Walk_Straight(8000+PID_OUT1(1,0,-1000,8,GetXpos(),GetYpos()),8000-PID_OUT1(1,0,-1000,8,GetXpos(),GetYpos()));
		
	//	Walk_Straight(3000+PID_OUT2(1,GetYpos()),3000-PID_OUT2(1,GetYpos()));
	//	Walk_Straight(3000+PID_OUT3(2,GetXpos()),3000-PID_OUT3(2,GetXpos()));
		
//		Walk_Straight(3000-Angle_Pid(GetAngle()),3000+Angle_Pid(GetAngle()));
//		switch(cnt)
//		{
//			case 0:
//				Walk_Straight(6000-Angle_Pid(GetAngle()),6000+Angle_Pid(GetAngle()));
//				if(GetYpos()>=2000)
//					cnt++;
//				
//			break;
//			case 1:
//				Walk_Straight(6000+(Angle_Pid(-90-GetAngle())),6000-Angle_Pid((-90-GetAngle())));
//				if(GetXpos()>=2000)
//					cnt++;
//			break;
//			case 2:
//				if(GetAngle()>=0)
//				{
//					Walk_Straight(6000+(Angle_Pid(180-GetAngle())),6000-Angle_Pid((180-GetAngle())));
//					if(GetYpos()<=0)
//						cnt++;
//				}
//				if(GetAngle()<0)
//				{
//				Walk_Straight(6000+(Angle_Pid(-180-GetAngle())),6000-Angle_Pid((-180-GetAngle())));
//					if(GetYpos()<=0)
//					cnt++;
//				}
//						
//			break;
//			case 3:
//				if(GetAngle()>=0)
//				{
//					Walk_Straight(3000+(Angle_Pid(90-GetAngle())),3000-Angle_Pid(90-GetAngle()));
//				}
//				if(GetAngle()<=0)
//				{
//					Walk_Straight(3000+(Angle_Pid(-270-GetAngle())),3000-Angle_Pid(-270-GetAngle()));
//				}
//				if(GetXpos()<=0)
//					cnt=0;
//				break;
//		}				
		
}
}
