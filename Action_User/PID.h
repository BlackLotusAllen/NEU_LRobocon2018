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
#include "fort.h"
// �궨��������ID
#define PUSH_BALL_ID (6)
// �궨���͵������͵�ʱ���Ӧ�õ���λ�ã���λλ����
#define PUSH_POSITION (4500)
// �궨���͵������ջ�ʱ���λ��
#define PUSH_RESET_POSITION (5)
//���亽��ǿ��ƺ��� ��λ���ȣ�ǹ˳ʱ��תΪ������ʱ��Ϊ����
// �궨�巢�����������ID
#define GUN_YAW_ID (7)
// �����תһ�ܵ�������
#define COUNT_PER_ROUND (4096.0f)
// �궨��ÿ�ȶ�Ӧ������
#define COUNT_PER_DEGREE  (COUNT_PER_ROUND/360.0f)
// �궨�庽��Ǽ��ٱ�
#define YAW_REDUCTION_RATIO (4.0f)
// ���亽���ת������ �ɶ�ת��Ϊ����
// yawAngleΪ�Ƕȣ���Χ180��-180֮�䣬��ʼλ��Ϊ0��
/*���ض�λϵͳ�ĽǶ�*/
float GetAngle(void);
/*���ض�λϵͳ��Xֵ*/
float GetX(void);
/*���ض�λϵͳ��Yֵ*/
float GetY(void);
/*���ض�λϵͳ��X����ٶ�*/
float GetSpeedX(void);
/*���ض�λϵͳ�ĽǶ�*/
float GetSpeedY(void);
/*���ض�λϵͳ��Z����ٶ�ֵ*/

#define manual 1
#define Arc 2
#define Line 3
#define Coordinate 4
#define ACW 5
#define CW 6
#define Left 7
#define Right 8

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
	u8 direction;
	int Mode;
	int Mode_Last;
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
	float V_Set;
	int Line_Num;
	int Arc_Num;
	int Coordinate_Num;
	int Line_Cnt;
	int Line_Order[32];
	Line_Value *l;
	Arc_Value *r;
	Coordinate_Value *c;
	float vel;
}PID_Value;

typedef struct gun
{
	float distance;
	float yawpos;
	float shootervel;
	float yawpos_set;
	float shootervel_set;
	float x_pre;
	float y_pre;
	float target_x;
	float target_y;
	u8 flag;
	int time_cnt;
}Gun_Value;

void PID_Init(PID_Value *PID_a);
void PID_Line_Init(void);
void PID_Arc_Init(void);
void PID_Coordinate_Init(void);
void Init_PID(PID_Value *pid_init);
void PID_Control(PID_Value *p);
void GO(PID_Value *p_GO);
void PID_Control_Competition(PID_Value *pid,Gun_Value *gun,u8 dir);
void PID_Pre(PID_Value *p);
void PID_Control_Competition_2(PID_Value *pid,u8 dir);
void shoot(Gun_Value *gun);
void Shoot_Control_Competition(PID_Value *pid,Gun_Value *gun,u8 dir);
float GetWZ(void);
float ABS(float thing);
float Compare(float a1,float b1);
float constrain(float amt, float high, float low);
float GetShooterVelCommand(void);
float GetYawPosCommand(void);
#endif
