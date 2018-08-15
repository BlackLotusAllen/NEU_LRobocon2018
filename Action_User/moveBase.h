/**
  ******************************************************************************
  * @file    .h
  * @author  ACTION_2017
  * @version V0.0.0._alpha
  * @date    2017//
  * @brief   This file contains all the functions prototypes for 
  *          
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MOVEBASE_H
#define __MOVEBASE_H


/* Includes ------------------------------------------------------------------*/



/* Exported types ------------------------------------------------------------*/

/** 
  * @brief  
  */


 
/* Exported constants --------------------------------------------------------*/



/** @defgroup 
  * @{
  */

//#define 

//电机旋转一周的脉冲数
#define COUNTS_PER_ROUND (4096)
//轮子直径（单位：mm）
#define WHEEL_DIAMETER (106.8f)
//调试小车车长（单位：mm）
#define MOVEBASE_LENGTH (500.0f)
//调试小车车宽(单位：mm)
#define MOVEBASE_WIDTH (403.0f)
//轮子宽度（单位：mm）
#define WHEEL_WIDTH (46.0f)
//两个轮子中心距离（单位：mm）
#define WHEEL_TREAD (355.4f)
//车轮周长(mm)
#define WHEEL_CIRCUMFERENCE (1116.3f)

#include "elmo.h"
/**
  * @}
  */
#define P_Angle 30
#define I_Angle 0
#define D_Angle 0

#define P_Location 1.5
#define I_Location 0
#define D_Location 0

/* Exported macro ------------------------------------------------------------*/
/* Exported functions --------------------------------------------------------*/
void CircleAround(float radius, float speed); //(半径(mm)), 速度(mm/s))
void RectangleAround(float length, float width, float BasicSpeed); //(长(mm), 宽(mm), 速度(mm/s))
void LockLineMove(double k, float b, float BasicSpeed, uint8_t direction); //(设定直线斜率, 设定直线截距, 基础速度, 方向(1为沿X轴正向, 0为负向))
float AnglePID(float Kp, float Ki, float Kd, float AngleSet, float AngleActual);
float LocationPID(float Kp, float Ki, float Kd, float PointSet, float PointActual);
void Move(float SpeedL, float  SpeedR);
void SetAngle(float val);
void SetXpos(float val);
void SetYpos(float val);
float GetAngle(void);
float GetXpos(void);
float GetYpos(void);
float AbsoluteValue(float value);





#endif /* ___H */



/************************ (C) COPYRIGHT NEU_ACTION_2017 *****END OF FILE****/

