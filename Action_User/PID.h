#ifndef _PID_H
#define _PID_H
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
#include "stm32f4xx.h"
#include <ucos_ii.h>
#include <math.h>
#include "adc.h"
#include "includes.h"
#include "usart.h"
#include "movebase.h"
// 宏定义推球电机ID
#define PUSH_BALL_ID (6)
// 宏定义送弹机构送弹时电机应该到达位置：单位位脉冲
#define PUSH_POSITION (4500)
// 宏定义送弹机构收回时电机位置
#define PUSH_RESET_POSITION (5)
//发射航向角控制函数 单位：度（枪顺时针转为正，逆时针为负）
// 宏定义发射机构航向电机ID
#define GUN_YAW_ID (7)
// 电机旋转一周的脉冲数
#define COUNT_PER_ROUND (4096.0f)
// 宏定义每度对应脉冲数
#define COUNT_PER_DEGREE  (COUNT_PER_ROUND/360.0f)
// 宏定义航向角减速比
#define YAW_REDUCTION_RATIO (4.0f)
#define BLACK_BALL (1)
#define WHITE_BALL (0)
// 发射航向角转换函数 由度转换为脉冲
// yawAngle为角度，范围180到-180之间，初始位置为0度
/*返回定位系统的角度*/
float GetAngle(void);
/*返回定位系统的X值*/
float GetX(void);
/*返回定位系统的Y值*/
float GetY(void);
/*返回定位系统的X轴的速度*/
float GetSpeedX(void);
/*返回定位系统的角度*/
float GetSpeedY(void);
/*返回定位系统的Z轴角速度值*/
float GetWZ(void);

extern void CorrectAngle(float value);

#define manual (1)
#define Arc (2)
#define Line (3)
#define Coordinate (4)
#define ACW (5)
#define CW (6)
#define Left (7)
#define Right (8)

typedef struct line
{
	float x1;
	float y1;
	float x2;
	float y2;
	float line_A;
	float line_B;
	float line_C;
	float line_kp;
	float line_Angle;
	float line_Error;
	float line_Priority;
}Line_Value;

typedef struct arc
{
	float x0;
	float y0;
	float r0;
	float arc_kp;
	float arc_Direction;
	float arc_Angle;
	float arc_Error;
}Arc_Value;

typedef struct coordinate
{
	float x3;
	float y3;
	float coordinate_Angle;
}Coordinate_Value;

typedef struct PID
{
	u8 corner;
	u8 direction;
	u8 fire_request;
	u8 fire_command;
	u8 fire_flag;
	u8 fire_cornor;
	u8 fire_turn;
	u8 dogHungry;
	u8 stop;
	Line_Value *l;
	Arc_Value *r;
	Coordinate_Value *c;
	int timeCnt;
	int food;
	int Mode;
	int Mode_Last;
	int Line_Num;
	int Line_Num_Next;
	int Line_Num_Last;
	int Arc_Num;
	int Coordinate_Num;
	int target_Num;
	int target_Next;
	int target_Last;
	int err_line_num;
	int push_pos_up;
	int push_pos_down;
	float Angle_Set;
	float kp;
	float ki;
	float kd;
	float Angle;
	float Angle_Last;
	float Error;
	float ITerm;
	float DTerm;
	float X;
	float Y;
	float X_Speed;
	float Y_Speed;
	float V;
	float vel;
	float V_Set;
}PID_Value;

typedef struct errorDisposal
{
	float Err_X;
	float Err_Y;
	float timeCnt;
	float distance;
	float err_distance;
	int errCnt;
	u8 stop;
	u8 flag;
}Err;

typedef struct targetrecord
{
	int n0;
	int n1;
	int n2;
	int n3;
}targetNum;

typedef struct priority1
{
	int line_num;
	int priority;
}priority;

void PID_Init(PID_Value *PID_a);
void PID_Line_Init(void);
void PID_Arc_Init(void);
void PID_Coordinate_Init(void);
void Init_PID(PID_Value *pid_init);
void PID_Control(PID_Value *p);
void GO(PID_Value *p_GO);
void PID_Pre(PID_Value *p);
float GetWZ(void);
float ABS(float thing);
float Compare(float a1,float b1);
float constrain(float amt, float high, float low);
float GetShooterVelCommand(void);
float GetYawPosCommand(void);
void PID_Competition_testVersion(PID_Value *pid, u8 dir);
void PID_Priority(PID_Value *pid, u8 dir, Err *error, int targetp[]);
void ErrorDisposal(PID_Value *pid,Err *error);
void GetData(PID_Value *p);
void shoot(PID_Value *p_gun, int target[], int Debug);
void PriorityControl(PID_Value *PID,Err *err,int targetn[]);
void WatchDog(PID_Value *Dog);
#endif
