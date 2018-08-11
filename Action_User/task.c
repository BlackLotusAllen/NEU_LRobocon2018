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
#include "status.h"

/*
===============================================================
						信号量定义
===============================================================
*/
OS_EXT INT8U OSCPUUsage;
OS_EVENT *PeriodSem;

static OS_STK App_ConfigStk[Config_TASK_START_STK_SIZE];
static OS_STK WalkTaskStk[Walk_TASK_STK_SIZE];

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
	OSTaskSuspend(OS_PRIO_SELF);
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
//	CAN_Config(CAN1,500,GPIOB,GPIO_Pin_8,GPIO_Pin_9);//can1初始化
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	CAN_Config(CAN2,500,GPIOB,GPIO_Pin_5,GPIO_Pin_6);//can2初始化
	USART3_Init(115200);
	UART4_Init(921600);
	TIM_Init(TIM2,10*1000-1,83,0x01,0x03);
	MotorOff(CAN2,0x001);//电机失能
	MotorOff(CAN2,0x002);
	ElmoInit(CAN2);//驱动初始化
	VelLoopCfg(CAN2,1,2000,2000);//速度环初始化
	VelLoopCfg(CAN2,2,2000,2000);
//	SetVelLimit(CAN2,0x001,80000,-80000);//配置驱动速度限制
//	SetVelLimit(CAN2,0x002,80000,-80000);
//	PosLoopCfg(CAN2,0x001,2000,2000,40000);//位置环初始化
//	PosLoopCfg(CAN2,0x002,2000,2000,40000);
//	SetPosLimit(CAN2,0x001,80000,-80000);//配置驱动位置限制
//	SetPosLimit(CAN2,0x002,80000,-80000);
//	SetPosCountingRange(CAN2,0 ,200000,-200000);//配置位置记录范围
//	SetUnitMode(CAN2,0x001, SPEED_CONTROL_MODE);//配置驱动器为速度控制模式
//	SetUnitMode(CAN2,0x002, SPEED_CONTROL_MODE);
//	SetSmoothFactor(CAN2,0x001,100);//设置驱动器平滑输出
	MotorOn(CAN2,1);//电机初始化
	MotorOn(CAN2,2);
	driveGyro();
	OSTaskSuspend(OS_PRIO_SELF);
}
extern struct position pos_t;
int T=0;
float setangle=90;
void WalkTask(void)
{
	
	float SpeedDiff=0,angle=0;
	CPU_INT08U os_err;
	os_err = os_err;

	OSSemSet(PeriodSem, 0, &os_err);
	while (1)
	{
		OSSemPend(PeriodSem, 0, &os_err);
		straight(0.5);
		T++;
		if(T>=400)
		{
			T=0;
			setangle+=90;
			if(setangle>=360)
				setangle-=360;
		}
		angle=GetAngle();
		SpeedDiff=AnglePID(angle,setangle);
		VelCrl(CAN2,0x01,exchange(0.5)-SpeedDiff);
		VelCrl(CAN2,0x02,-exchange(0.5)-SpeedDiff);
		if(T%50==0)
		{
			USART_OUT(UART4, "angle:%d	set:%d	diff:%d\n",(int)(angle),(int)(setangle),(int)(SpeedDiff));
		}
	}
}
