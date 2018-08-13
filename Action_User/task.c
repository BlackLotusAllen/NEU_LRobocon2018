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

/*
===============================================================
						信号量定义
===============================================================
*/
OS_EXT INT8U OSCPUUsage;
OS_EVENT *PeriodSem;

pos_t xya;
static OS_STK App_ConfigStk[Config_TASK_START_STK_SIZE];
static OS_STK WalkTaskStk[Walk_TASK_STK_SIZE];
static float set_x=0;
static float set_y=0;
static float set_angle=0;
int iSOKFlag=0;
static int car=1;
static int n=0;
static float setangle;
static float Kp=300;
static float Kd=10;
static float Aout=0;
static float nowerror=0;
static float lasterror=0;
static float adderror=0;
void driveGyro(void)
{
	while(!iSOKFlag)
	{
		delay_ms(5);
		USART_SendData(USART3,'A');
		USART_SendData(USART3,'T');
		USART_SendData(USART3,'\r');
		USART_SendData(USART3,'\n');
	
	}
	iSOKFlag=0;
}
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
}

/*
   ===============================================================
   初始化任务
   ===============================================================
   */
extern uint8_t opsFlag;
void ConfigTask(void)
{
	CPU_INT08U os_err;
	os_err = os_err;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	TIM_Init(TIM2,1000-1,84-1,0x00,0x00);
	USART3_Init(115200);
	UART4_Init(921600);
	CAN_Config(CAN2,500,GPIOB,GPIO_Pin_5,GPIO_Pin_6);
	CAN_Config(CAN1,500,GPIOB,GPIO_Pin_8,GPIO_Pin_9);
	ElmoInit(CAN2);
	VelLoopCfg(CAN2,1,10000,10000);
	VelLoopCfg(CAN2,2,10000,10000);
	MotorOn(CAN2,1);
	MotorOn(CAN2,2);
	delay_s(2);
	
	if(car==1)
	{	
     driveGyro();
     USART_OUT(UART4,(uint8_t*)"OKOPSOPS");
	 while(!opsFlag);
	}else if (car==4)
     delay_s(10);	
	
	
	OSTaskSuspend(OS_PRIO_SELF);
}
static int Right_cr1;
static int Left_cr2;
void WalkTask(void)
{   void Anglepid();
    void go(float);
	CPU_INT08U os_err;
	os_err = os_err;
    int x;
	int y;
	int angle;
	OSSemSet(PeriodSem, 0, &os_err);
	while (1)
	{		
		OSSemPend(PeriodSem,  0, &os_err);
		
		x=(int)xya.x;
		y=(int)xya.y;
		angle=(int)xya.angle;
		USART_OUT(UART4,(uint8_t*)"%d\t%d\r\n",x,y);
//		setangle=0;
//		Anglepid();
//		Right_cr1=2048-Aout;		
//	    Left_cr2=-2048-Aout;
//		VelCrl(CAN2,1,Right_cr1);
//		VelCrl(CAN2,2,Left_cr2);
		
		go(0.754);
//		Right_cr1=4096;		
//	    Left_cr2=-4096;
//		VelCrl(CAN2,1,Right_cr1);
//		VelCrl(CAN2,2,Left_cr2);
		
	}
}
void turn(float);	
void go(float v)
{ 
	Kp=200;
    void Anglepid();
	float V=v*1000;
	if(n==0)
	{   
     	setangle=0;
	}
	else if(n==1)
	{
		setangle=-90;
	}
	else if(n==2)
	{	if(xya.angle>=0)	
		setangle=180;
		else if(xya.angle<=0)
		setangle=-180;
	}
	else if(n==3)
	{
		setangle=90;
    }
	
	if(fabs(xya.x-set_x)<1750&&fabs(xya.y-set_y)<1750)
	{	
		Anglepid();
		Right_cr1=4096*V/377-Aout;		
	    Left_cr2=-4096*V/377-Aout;
		VelCrl(CAN2,1,Right_cr1);
		VelCrl(CAN2,2,Left_cr2);
		
		
	}
	else if(fabs(xya.x-set_x)>=1750||fabs(xya.y-set_y)>=1750)
	{   
		
		turn(v);
	}
		
}
void turn(float v)
{ 
    void Anglepid();
	float V=v*1000;
	int h=0;
	if(n==0)
	{   set_angle=0;
     	setangle=-90;
	}
	else if(n==1)
	{
		set_angle=-90;
		setangle=-180;
	}
	else if(n==2)
	{
		set_angle=180;
		setangle=90;
	}
	else if(n==3)
	{
		n=-1;
		set_angle=90;
		setangle=0;
    }
	while(1)
	{
		
		 if(n==2)
		 {
			 if(xya.angle<0)
			 {
				 xya.angle+=360;
				  h=1;
			 }else h=0;
		 }
     if(fabs(xya.angle-set_angle)<90||h)		
	 { 
		 Kp=100;
		 int x;
	     int y;
	     int angle;
		 int Set_angle;
			
		 Anglepid();
		 Right_cr1=1024-Aout;
	     Left_cr2=1024-Aout;
		 VelCrl(CAN2,1,Right_cr1);
		 VelCrl(CAN2,2,Left_cr2);
		 x=(int)xya.x;
		 y=(int)xya.y;
		 angle=(int)xya.angle;
		 Set_angle=(int)set_angle;
		 USART_OUT(UART4,(uint8_t*)"%d\t%d\r\n",x,y);
		 h=0;
		 
	 }else if(fabs(xya.angle-set_angle)>=90) 
	 { 
		 VelCrl(CAN2,1,0);
		 VelCrl(CAN2,2,0);
		 delay_ms(20);
		 n++;
		 set_x=xya.x;
		 set_y=xya.y;
		 break;
		 
	 }
    }
}
	


void Anglepid(void)
{   
	lasterror=nowerror;
	nowerror=setangle-xya.angle;
	adderror+=nowerror;
	Aout=Kp*nowerror;	
}
