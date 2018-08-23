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
#include "PID.H"
#include "Pos.h"
#include "pps.h"
#include "adc.h"
#include "othermoto.h"

#define X1 100
#define X2 200

#define THRESHOLD4ADC 150
/*
===============================================================
						信号量定义
===============================================================
*/
OS_EXT INT8U OSCPUUsage;
OS_EVENT *PeriodSem;
OS_EVENT *PeriodSem2;
OS_EVENT *ErrSem;

PIDCtrlStructure PidA;
PIDCtrlStructure PidB;

point nowPoint;

static struct
{
    float left;
    float right;
}wheelspeed={0, 0};

static enum {clockwise, anticlockwise} Dir2TurnAround = clockwise; 

static linewithdir line1 = {0, -1, 0, forward};
static linewithdir line2 = {-1, 0, 0, forward};
static linewithdir line3 = {0, -1, 0, backward};
static linewithdir line4 = {-1, 0, 0, backward};

static const linewithdir corss1 = {1, 1, -2400, forward};
static const linewithdir corss2 = {-1, 1, -2400, forward};

#define CCX 0
#define CCY 2400
static point circlecentre;//{2400, 0}
static reldir circledir = right;
static float circleradius = 700;
static float length = 0;
static enum {circle, fang} roadsignal = circle;

static linewithdir Target =
{
    .a = 0,
    .b = 1,
    .c = 0,
    .linedir = forward,
};

static _Bool errCheckOn_Flag;

void App_Task()
{
	CPU_INT08U os_err;
	os_err = os_err; /*防止警告...*/

	/*创建信号量*/
	PeriodSem = OSSemCreate(0);
	PeriodSem2 = OSSemCreate(0);
	ErrSem = OSSemCreate(0);
	/*创建任务*/
	os_err = OSTaskCreate((void (*)(void *))ConfigTask, /*初始化任务*/
						  (void *)0,
						  (OS_STK *)&App_ConfigStk[Config_TASK_START_STK_SIZE - 1],
						  (INT8U)Config_TASK_START_PRIO);

	os_err = OSTaskCreate((void (*)(void *))WalkTask,
						  (void *)0,
						  (OS_STK *)&WalkTaskStk[Walk_TASK_STK_SIZE - 1],
						  (INT8U)Walk_TASK_PRIO);
                          
	os_err = OSTaskCreate((void (*)(void *))ErrCheck,
						  (void *)0,
						  (OS_STK *)&ErrCheckStk[ERR_CHECK_STK_SIZE - 1],
						  (INT8U)ERR_CHECK_PRIO);
                          
	os_err = OSTaskCreate((void (*)(void *))ErrSolve,
						  (void *)0,
						  (OS_STK *)&ErrSolveStk[ERR_SOLVE_STK_SIZE - 1],
						  (INT8U)ERR_SOLVE_PRIO);
                          
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
    TIM_Init(TIM2, 999, 83, 0x00, 0x00);
    Adc_Init();
    UART4_Init(921600);
    MoveBaseInit();
    OtherMotoInit();
    ppsInit();
    OSTaskSuspend(ERR_CHECK_PRIO);
    errCheckOn_Flag = 0;
	OSTaskSuspend(OS_PRIO_SELF);
}

float LengthProcessing(float x)
{
    if(x <= X1)
    {
        return 0;
    }
    else if(x >= X2)
    {
        return 1;
    }
    else
    {
        return (atan((x - X1) / (X2 - x)) / 1.57);
    }
}

float LengthProcessing2(float x)
{
    return (x/X1)/((x/X1)+1);
}

float LengthProcessing3(float x)
{
    return (1 - pow(100, (-1 * x / X1)));
}

float GetP4circle(void)
{
    float tempNum = 0, tempAns = 0;
    tempNum = Point2Point(circlecentre, nowPoint) - circleradius;
    tempAns = LengthProcessing(__fabs(tempNum));
    if(circledir == left)
    {
        tempAns = -1 * tempAns;
    }
    if(tempNum > 0)
    {
        return tempAns;
    }
    else if(tempNum < 0)
    {
        return -1 * tempAns;
    }
    else
    {
        return 0;
    }
}

float GetP4fang(void)
{
    float tempNum = 0;
    tempNum =Point2Line(Target, nowPoint);
    if(RelDir2Line(Target, nowPoint) == right)
    {
        return LengthProcessing(__fabs(tempNum));
    }
    else if(RelDir2Line(Target, nowPoint) == left)
    {
        return -1 * LengthProcessing(__fabs(tempNum));
    }
    else
    {
        return 0;
    }
}

void SetDir2TurnAround(void)
{
    if(Dir2TurnAround == clockwise)
    {
        circledir = right;
        line1.linedir = forward;
        line2.linedir = forward;
        line3.linedir = backward;
        line4.linedir = backward;
    }
    else if(Dir2TurnAround == anticlockwise)
    {
        circledir = left;
        line1.linedir = backward;
        line2.linedir = backward;
        line3.linedir = forward;
        line4.linedir = forward;
    }
}

void setFangArea(float length)
{
    line1.c = 2400 - length;
    line2.c = -length;
    line3.c = 2400 + length;
    line4.c = length;
}

void ChangeRoad(void)
{
    static _Bool yChangeFlag = 0;
    static float lasta = 0;
    point relPoint;
    relPoint = RelPos(circlecentre, nowPoint);
    if(relPoint.y > 0)
    {
        yChangeFlag = 1;
    }
    if(relPoint.y < 0 && yChangeFlag)
    {
        if(lasta * relPoint.a < 0)
        {
            yChangeFlag = 0;
            if(roadsignal == circle)
            {
                circleradius += 500;
            }
            if(roadsignal == fang)
            {
                length -= 500;
                setFangArea(length);
            }
        }
    }
    lasta = relPoint.a;
    if(circleradius >= 2000 && roadsignal == circle)
    {
        roadsignal = fang;
        length = 2000;
        setFangArea(length);
    }
    if(length <= 700 && roadsignal == fang)
    {
        roadsignal = circle;
        circleradius = 700;
    }
    if(roadsignal == fang)
    {
        reldir dir2corss1,dir2corss2;
        dir2corss1 = RelDir2Line(corss1, nowPoint);
        dir2corss2 = RelDir2Line(corss2, nowPoint);
        if(dir2corss1 == left)
        {
            if(dir2corss2 == right)
            {
                Target = line1;
            }
            else if(dir2corss2 == left)
            {
                Target = line2;
            }
            else
            {
                Target = Target;
            }
        }
        else if(dir2corss1 == right)
        {
            if(dir2corss2 == left)
            {
                Target = line3;
            }
            else if(dir2corss2 == right)
            {
                Target = line4;
            }
            else
            {
                Target = Target;
            }
        }
        else
        {
            Target = Target;
        }
    }
}

void WalkTask(void)
{
	CPU_INT08U os_err;
	os_err = os_err;
    float uout1 = 0, uout2 = 0;
    linewithdir line2centre;
    PidA.KP = 20; //20
    PidA.KI = 0.01; //0.01
    PidA.KD = 20; //20
    PidA.GetVar = GetA;
    PidA.ExOut = 0;
    PidB.KP = 10; //10
    PidB.KI = 0.01; //0.0001
    PidB.KD = 20; //20
    PidB.GetVar = GetA;
    PidB.ExOut = 0;
    circlecentre = setPointXY(CCX, CCY);
    MotorOn(CAN2, 1);
    MotorOn(CAN2, 2);
    PIDCtrlInit1();
    PIDCtrlInit2();
//    USART_OUT(UART4, (uint8_t *)"x   y   a   cr   fl   ta   tb   tc   tdir\r\n");
    USART_OUT(UART4, (uint8_t *)"laserA laserB\r\n");
//    _Bool ADCFlag = 1;
//    while(ADCFlag)
//    {
//        static _Bool leftFlag = 0, rightFlag = 0;
//        if(GETADCLEFT < THRESHOLD4ADC)
//        {
//            leftFlag = 1;
//        }
//        if(GETADCRIGHT < THRESHOLD4ADC)
//        {
//            rightFlag = 1;
//        }
//        if(GETADCLEFT > THRESHOLD4ADC && leftFlag)
//        {
//            Dir2TurnAround = clockwise;
//            ADCFlag = 0;
//        }
//        if(GETADCRIGHT >THRESHOLD4ADC && rightFlag)
//        {
//            Dir2TurnAround = anticlockwise;
//            ADCFlag = 0;
//        }
//    }
	OSSemSet(PeriodSem, 0, &os_err);
	while (1)
	{
		OSSemPend(PeriodSem, 0, &os_err);
        SendBall2Launcher();
        LauncherYAWCtrl(90);
        LauncherWheelSpeedCtrl(50);
//        CollecterWheelSpeedCtrl(40);
        nowPoint = GetNowPoint();
        SetDir2TurnAround();
        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) GetLaserA());
        USART_OUT(UART4, (uint8_t *)"%d   \r\n", (int32_t) GetLaserB());
        ChangeRoad();
        if(roadsignal == circle)
        {
            line2centre = DirlinePoint2Point(circlecentre, nowPoint);
            PidA.ExOut = VDirForLine(line2centre, circledir);
            PidB.ExOut = PidA.ExOut - 90 * GetP4circle();
        }
        else if(roadsignal == fang)
        {
            PidA.ExOut = LineDir(Target);
            PidB.ExOut = PidA.ExOut + 90 * GetP4fang();
        }
        if(PidB.ExOut >= 180)
        {
            PidB.ExOut -= 360;
        }
        if(PidB.ExOut < -180)
        {
            PidB.ExOut += 360;
        }
        uout1 = PIDCtrl1(PidA);
        uout2 = PIDCtrl2(PidB);
        wheelspeed.left = 1000 - uout1 - uout2;
        wheelspeed.right = 1000 + uout1 + uout2;
//        if(__fabs(500 + uout2 + uout1) > 1500 || __fabs(500 - uout2 - uout1) > 1500)
//        {
//            uout1 = 0;
//            uout2 = 0;
//            PIDCtrlInit1();
//            PIDCtrlInit2();
//            while(1)
//            {
//                WheelSpeed(0, 1);//right
//                WheelSpeed(0, 2);//left
//                USART_OUT(UART4, (uint8_t *)"overspeed!   \r\n");
//            }
//        }
        WheelSpeed(wheelspeed.right, 1);
        WheelSpeed(wheelspeed.left, 2);
        if(!errCheckOn_Flag)
        {
            errCheckOn_Flag = 1;
            //OSTaskResume(ERR_CHECK_PRIO);
        }
//        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) nowPoint.x);
//        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) nowPoint.y);
//        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) GetA());
//        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) circleradius);
//        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) length);
//        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) Target.a);
//        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) Target.b);
//        USART_OUT(UART4, (uint8_t *)"%d   ", (int32_t) Target.c);
//        USART_OUT(UART4, (uint8_t *)"%d   \r\n", (int32_t) Target.linedir);
	}
}

void ErrCheck(void)
{
    CPU_INT08U os_err;
    os_err = os_err;
    point lastpoint = {0, 0, 0, 0};
    uint8_t counter = 0;
    OSSemSet(PeriodSem2, 0, &os_err);
    while (1)
    {
        OSSemPend(PeriodSem2, 0, &os_err);
        if(Point2Point(lastpoint, nowPoint) >= 10)
        {
            counter = 0;
        }
        if(Point2Point(lastpoint, nowPoint) < 10)
        {
            counter++;
        }
        if(counter > 10)
        {
            counter = 0;
            OSSemPost(ErrSem);
        }
        lastpoint = nowPoint;
    }
}

void ChangeDirSignal(void)
{
    if(Dir2TurnAround == clockwise)
    {
        Dir2TurnAround = anticlockwise;
    }
    if(Dir2TurnAround == anticlockwise)
    {
        Dir2TurnAround = clockwise;
    }
    SetDir2TurnAround();
}

void ErrSolve(void)
{
    CPU_INT08U os_err;
    os_err = os_err;
    OSSemSet(ErrSem, 0, &os_err);
    while(1)
    {
        OSSemPend(ErrSem, 0, &os_err);
        OSTaskSuspend(Walk_TASK_PRIO);
        OSTaskSuspend(ERR_CHECK_PRIO);
        PIDCtrlInit1();
        PIDCtrlInit2();
        uint8_t counter = 100;
        while(counter--)
        {
            OSSemPend(PeriodSem, 0, &os_err);
            WheelSpeed(-1 * wheelspeed.right, 1);
            WheelSpeed(-1 * wheelspeed.left, 2);
        }
//        ChangeDirSignal();
        OSSemSet(PeriodSem, 0, &os_err);
        OSSemSet(PeriodSem2, 0, &os_err);
        OSTaskResume(Walk_TASK_PRIO);
        OSTaskResume(ERR_CHECK_PRIO);
    }
}
