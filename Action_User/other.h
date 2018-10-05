#ifndef OTHER_H
#define OTHER_H
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
#include "PID.h"
#include "fort.h"
#include "pps.h"
#include "math.h"
#include "stm32f4xx_it.h"

//#include <cruntime.h>
#include <stdlib.h>
//#include <search.h>
//#include <internal.h>

#define Compensation_Angle (0.f)		//����

#define SET_ROLLER_SPEED (70)			//������ת��
#define LEFT_ROLLER_THRESHOLD (4) /*3*/	//������ת����ֵ
#define RIGHT_ROLLER_THRESHOLD (5) /*6*///������ת����ֵ
/*1*/
#define LEFT_FIRST_THRESHOLD ()			//>=4����Сֵ<=65(+)   <=8(+) ��Сֵ>54(-2)   ����[ , ]
#define RIGHT_FIRST_THRESHOLD ()		//>=3����Сֵ<=62(+)   <=10(+)��Сֵ>55(-2)   ����[45(-) , 97(+)]
/*2*/
#define LEFT_SECOND_THRESHOLD ()		//>=10(-)����Сֵ<=53(+)   <=12(+)��Сֵ>47(-)   ����[146(-) , 233(+)]
#define RIGHT_SECOND_THRESHOLD ()		//>=15(-)����Сֵ<=53(+)   <=18(+)��Сֵ>47(-)   ����[130(-) , 242(+)]
/*3*/
#define LEFT_THIRED_THRESHOLD ()		//>=14(-)����Сֵ<=41(+)   <=18(+)��Сֵ>36(-)   ����[230(-) , 457(+)]
#define RIGHT_THIRED_THRESHOLD ()		//>=18(-)����Сֵ<=52(+)   <=23(+)��Сֵ>43(-)   ����[223(-) , 359(+)]
/*4*/
#define LEFT_FOURTH_THRESHOLD ()		//��Чֵ����[ , ]   ��Сֵ��Χ[ , ]   ��������[ , ]
#define RIGHT_FOURTH_THRESHOLD ()		//��Чֵ����[ , ]   ��Сֵ��Χ[ , ]   ��������[ , ]
/*5*/
#define LEFT_FIFTH_THRESHOLD ()			//��Чֵ����[ , ]   ��Сֵ��Χ[ , ]   ��������[ , ]
#define RIGHT_FIFTH_THRESHOLD ()		//��Чֵ����[ , ]   ��Сֵ��Χ[ , ]   ��������[ , ]
/*More*/
#define LEFT_FIFTH_THRESHOLD ()			//��Чֵ����[ , ]   ��Сֵ��Χ[ , ]   ��������[ , ]
#define RIGHT_FIFTH_THRESHOLD ()		//��Чֵ����[ , ]   ��Сֵ��Χ[ , ]   ��������[ , ]

void GetFloat (float Num10, int places);
void GetLaserData(void);
void GetPositionValue(PID_Value *pid_out);
void SetFortAngle(PID_Value *pos,float set_angle);
void RadarCorrection(PID_Value *pos);
void Entertainment(void);
void Power_On_Self_Test(PID_Value *pos);
void CntGolf(void);
#endif
