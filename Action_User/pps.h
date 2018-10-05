#ifndef __PPS_H
#define __PPS_H
#include "stdint.h"
#include "PID.h"

/*接受几个来自定位系统的float数据*/
#define GET_PPS_VALUE_NUM      6
/*接受几个来自定位系统的uint8_t数据*/ /* 6 * 4byte = 24*/
#define GET_PPS_DATA_NUM       24

typedef union{
	uint8_t data[GET_PPS_DATA_NUM];
	float  value[GET_PPS_VALUE_NUM];
}PosSend_t;

/*定位系统返回的值*/
typedef struct{
		/*定位系统返回的角度*/
		float ppsAngle ;
		/*定位系统返回的X值*/
		float ppsX ;
		/*定位系统返回的Y值*/
		float ppsY ;
		/*定位系统返回的X轴速度*/
		float ppsSpeedX;
		/*定位系统返回的Y轴速度*/
		float ppsSpeedY;
		/*定位系统的z轴角速度*/
		float ppsWZ ;
}Pos_t;


typedef struct
{
	int MoveFlag_cmd;           //设置是否运动
	int FireFlag_cmd;           //设置是否射球
	int TarBucketNum_cmd;       //设置目标桶编号	
	
	float YawZeroOffset_cmd;    //设置航向电机归零角度
	float YawPosSet_cmd;        //设置航向电机角度
	float ShooterVelSet_cmd;    //设置射球电机转速
}Command;

void TalkOpsToGetReady(void);
/*初始化并且让程序等待定位系统发数*/
void WaitOpsPrepare(void);

void SetOpsReady(uint8_t flag);
void SetAngle(float setValue);
void SetX(float setValue);
void SetY(float setValue);
void SetSpeedX(float setValue);
void SetSpeedY(float setValue);
void SetWZ(float setValue);

void CorrectX(float value);
void CorrectY(float value);
void CorrectAngle(float value);

/*定位系统准备完毕*/
uint8_t GetOpsReady(void);
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
void UART4_OUT(PID_Value *pid_out , Err*error1);
void GetValueFromPC(u8 data);
float GetYawPosCommand(void);
float GetShooterVelCommand(void);
float GetTargetNumberCommand(void);


#endif 

