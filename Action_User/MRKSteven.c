#include "MRKSteven.h"
#include "pps.h"
#include "elmo.h"
#include "math.h"
#include "usart.h"
void Move(int V1,int V2)//运动函数
{
VelCrl(CAN2,1,V1);//右轮
VelCrl(CAN2,2,V2);//左轮
}
extern Place positionf;
       Place positioni;
float v1,v2;               //两轮速度（2左 || 1右）
float v1_record=0,v2_record=0;
//====================================================================================
//                             （坐标翻转）
//====================================================================================
void Coordinate_Reverse(void)          //坐标反转函数
{
positioni.X=-(int)positionf.X;
positioni.Y=-(int)positionf.Y;//新定位系统要再加280
positioni.Angle=(int)positionf.Angle;
	
positionf.Y=-positionf.Y;
positionf.X=-positionf.X;
}
//====================================================================================
//                                 位置记录/发送实时位置
//====================================================================================
	float x_now;
    float x_p10=0;
	float x_p20=0;
    float y_now;
	float y_p10=0;
	float y_p20=0;
    float Angle_now;
	float Angle_p10=0;  //10ms前角度值
	float Angle_p20=0;  //20ms前角度值
void Position_Record(void)
	//记录10ms前、20ms前角度值、坐标值
	{
	Angle_p20=Angle_p10;
	Angle_p10=Angle_now;
	Angle_now=positionf.Angle;
		
	x_p20=x_p10;
	x_p10=x_now;
	x_now=positionf.X;
		
	y_p20=y_p10;
	y_p10=y_now;
	y_now=positionf.Y;
	
	USART_OUT(UART4,(uint8_t*)"%s%s","X",":");
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)positionf.X);
	USART_OUT(UART4,(uint8_t*)"%s%s","Y",":");
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)positionf.Y);
	USART_OUT(UART4,(uint8_t*)"%s%s","A",":");
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)positionf.Angle);
	}
//====================================================================================
//                                  基础运动
//====================================================================================
void Move_Basic(void)
{
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%d\r\n","M","o","d","e",1);//mode1
	if(r!=0)                            //圆周运动
	{
		v1=(int)((10865*v)+(217*v*10.865/r));
		v2=(int)((10865*v)-(217*v*10.865/r));
		if(direction==0)                      //圆周运动（前进）
		{Move(v1,-v2);}
		else if(direction==1)                 //圆周运动（后退）
		{Move(-v1,v2);}
	}
	else                                //直行
	{
		v1=(int)10865*v;
		if(direction==0)                      //直行（前进）
		{Move(v1,-v1);}
		if(direction==1)                      //直行（后退）
		{Move(-v1,v1);}
	}
}
//====================================================================================
//                                角度闭环（方案）
//====================================================================================
CAngle CangleLock={angle};
void Angle_Lock4(float angle_target)//锁定角度方案4【成功】——————最终采纳方案4
{
	v1=(int)10865*v;
	v2=(int)10865*v;
	//作差
	if(angle_target<-180){angle_target=360+angle_target;}
	if(angle_target>180){angle_target=angle_target-360;}
	CangleLock.m_angle_Dvalue=positioni.Angle-angle_target;
	//差值讨论
	if(CangleLock.m_angle_Dvalue>180){CangleLock.m_angle_Dvalue=CangleLock.m_angle_Dvalue-360;}
	if(CangleLock.m_angle_Dvalue<-180){CangleLock.m_angle_Dvalue=CangleLock.m_angle_Dvalue+360;}
	//运行
	Move(v1-Kp_A*CangleLock.m_angle_Dvalue,-v2-Kp_A*CangleLock.m_angle_Dvalue);

	//记录v1、v2
	v1_record=v1-Kp_A*CangleLock.m_angle_Dvalue;
	v2_record=-v1-Kp_A*CangleLock.m_angle_Dvalue;
}
//====================================================================================
//                                正方形运动（方案）
//====================================================================================

int   x1=0,y1=0;    //mode2正方形边初始坐标记录
int   mo=1;         //mode2正方形边初始坐标记录
void Square_Movement (void)
{
	if(positioni.Y<=(y1+750*r)&&mo==1){Angle_Lock4(0);}
	if(positioni.Y>(y1+750*r)&&mo==1){x1=positioni.X;y1=positioni.Y;Angle_Lock4(-90);mo=2;}
	
	if(positioni.X<=(x1+750*r)&&mo==2){Angle_Lock4(-90);}
	if(positioni.X>(x1+750*r)&&mo==2){x1=positioni.X;y1=positioni.Y;Angle_Lock4(179);mo=3;}
	
	if(positioni.Y>(y1-750*r)&&mo==3){Angle_Lock4(179);}
	if(positioni.Y<=(y1-750*r)&&mo==3){x1=positioni.X;y1=positioni.Y;Angle_Lock4(90);mo=4;}
	
	if(positioni.X>(x1-750*r)&&mo==4){Angle_Lock4(90);}
	if(positioni.X<(x1-750*r)&&mo==4){x1=positioni.X;y1=positioni.Y;Angle_Lock4(0);mo=1;}
}
//====================================================================================
//                                直线闭环（方案）
//====================================================================================
/////////////////////////// 
//          |-y/         //
//         0|0/          //
//          |/           //
// 90       /       -90  //
// ————————/|—————————— >//
//        / |          -x//
//       /  |            //
//      /180|-180        //
//     /    |            //
///////////////////////////
CLine ClineLock={line_angle,line_yintercept,line_xintercept};

void Line_Lock4(float lineangle, float yintercept, float xintercept)
{
if(lineangle==0||lineangle==-180)                    {ClineLock.m_line_distance=positioni.X-xintercept;}
if(lineangle==180) {ClineLock.m_line_distance=xintercept-positioni.X;}
if(lineangle!=0&&lineangle!=180&&lineangle!=-180)  
{
	if(lineangle>=-180&&lineangle<=0){ClineLock.m_line_slope=tan(pi*(lineangle+90)/180);}
	if(lineangle>0&&lineangle<=180){ClineLock.m_line_slope=tan(pi*(lineangle-90)/180);}
//	if(ang_l<-90){k=tan(pi*(ang_l+270)/180);}
	ClineLock.m_line_distance=(positioni.Y-ClineLock.m_line_slope*positioni.X-yintercept)/(sqrt(1+ClineLock.m_line_slope*ClineLock.m_line_slope));
}
if(lineangle>=0)
{
	if(ClineLock.m_line_distance>switch_distance){Angle_Lock4(lineangle+90);}
	if(ClineLock.m_line_distance<-switch_distance){Angle_Lock4(lineangle-90);}
	if(ClineLock.m_line_distance<=switch_distance&&ClineLock.m_line_distance>=-switch_distance){Angle_Lock4(lineangle+Kp_l*ClineLock.m_line_distance);}
}
if(lineangle<0)
{
	if(ClineLock.m_line_distance>switch_distance){Angle_Lock4(lineangle-90);}
	if(ClineLock.m_line_distance<-switch_distance){Angle_Lock4(lineangle+90);}
	if(ClineLock.m_line_distance<=switch_distance&&ClineLock.m_line_distance>=-switch_distance){Angle_Lock4(lineangle-Kp_l*ClineLock.m_line_distance);}
}

USART_OUT(UART4,(uint8_t*)"%d  ",(int)(90*ClineLock.m_line_distance/switch_distance));
USART_OUT(UART4,(uint8_t*)"%d  ",(int)ClineLock.m_line_distance);	
USART_OUT(UART4,(uint8_t*)"%d  ",(int)lineangle);		
USART_OUT(UART4,(uint8_t*)"%d  ",(int)(pi*(lineangle+90)/180));		
USART_OUT(UART4,(uint8_t*)"%d  ",(int)ClineLock.m_line_slope);		
}

//====================================================================================
//                                正方形闭环（方案）
//====================================================================================
//r为正方形半边长
//d_M为正方形半边长
//四条直线（顺时针）：【1】y=d_M-r（+） , 【2】x=r（+） , 【3】y=d_M+r（+） , 【4】x=-r（+）
//四条直线（逆时针）：【1】y=d_M-r（-） , 【4】x=-r（-） , 【3】y=d_M+r（-） , 【2】x=r（-）
//direction顺/逆时针（0为逆时针，1为顺时针）
/////////////////////////// 
//         0|0           //
//     【3】|-y          //
//     |<———|————|【2】  //
//     |    |    |       //
//     |    +d_M |       //
//     |   r|    |       //
//【4】|————|———>|       //
// 90       |【1】  -90  //
// —————————|—————————— >//
//          |         -x //
//          |            //
//       180|-180        //
//          |            //
///////////////////////////
CSquare CsquareLock={0,square_middle,square_halfedges,switch_distance};

void Square_Lock1(int squaremiddle , int squareHalfedges)//回到原定路线
{
if(direction==0)//逆时针
{
if(CsquareLock.m_square_side==0){Line_Lock4(-90,squaremiddle-squareHalfedges,0);}
if(CsquareLock.m_square_side==0&&positioni.X>=squareHalfedges-CsquareLock.m_switch_distance){CsquareLock.m_square_side=1;Line_Lock4(0,0,squareHalfedges);}
if(CsquareLock.m_square_side==1){Line_Lock4(0,0,squareHalfedges);}
if(CsquareLock.m_square_side==1&&positioni.Y>=(squaremiddle+squareHalfedges-CsquareLock.m_switch_distance)){CsquareLock.m_square_side=2;Line_Lock4(90,squaremiddle+squareHalfedges,0);}
if(CsquareLock.m_square_side==2){Line_Lock4(90,squaremiddle+squareHalfedges,0);}
if(CsquareLock.m_square_side==2&&positioni.X<-squareHalfedges+CsquareLock.m_switch_distance){CsquareLock.m_square_side=3;Line_Lock4(180,0,-squareHalfedges);}
if(CsquareLock.m_square_side==3){Line_Lock4(180,0,-squareHalfedges);}
if(CsquareLock.m_square_side==3&&positioni.Y<=squaremiddle-squareHalfedges+CsquareLock.m_switch_distance){CsquareLock.m_square_side=0;Line_Lock4(-90,squaremiddle-squareHalfedges,0);}

USART_OUT(UART4,(uint8_t*)"%d  ",Kp_A);
USART_OUT(UART4,(uint8_t*)"%d  ",CsquareLock.m_switch_distance);
USART_OUT(UART4,(uint8_t*)"%d  ",Kp_l*100);
}
}

//====================================================================================
//                                正圆闭环（方案）
//====================================================================================

CCircle CcircleLock={radius,center_x,center_y};

void Circle_Lock1(void)//【角度闭环】沿切线方向走(过点做切线)
{
CcircleLock.m_distance_tpcenter=sqrt((CcircleLock.m_center_x-positioni.X)*(CcircleLock.m_center_x-positioni.X)+(CcircleLock.m_center_y-positioni.Y)*(CcircleLock.m_center_y-positioni.Y));

	if(CcircleLock.m_center_x!=positioni.X)
	{CcircleLock.m_angle_tpcenter=180*(atan((CcircleLock.m_center_y-positionf.Y)/(CcircleLock.m_center_x-positionf.X)))/pi;}
	if(CcircleLock.m_center_x==positioni.X)
	{
	if(CcircleLock.m_center_y>positioni.Y){CcircleLock.m_angle_tpcenter=90;}
	if(CcircleLock.m_center_y<positioni.Y){CcircleLock.m_angle_tpcenter=-90;}
	}
	if(CcircleLock.m_center_y-positioni.Y>0&&CcircleLock.m_center_x-positioni.X<0){CcircleLock.m_angle_tpcenter=CcircleLock.m_angle_tpcenter+180;}
	if(CcircleLock.m_center_y-positioni.Y<0&&CcircleLock.m_center_x-positioni.X<0){CcircleLock.m_angle_tpcenter=CcircleLock.m_angle_tpcenter-180;}

if(direction==0)//逆时针
{
	if(CcircleLock.m_distance_tpcenter>1000*radius)
	{
		CcircleLock.m_angle_tptangent=180*(asin(1000*radius/CcircleLock.m_distance_tpcenter))/pi;
		if(CcircleLock.m_angle_tptangent<0){CcircleLock.m_angle_tptangent=-CcircleLock.m_angle_tptangent;}
		
		if(CcircleLock.m_distance_tpcenter-1000*radius<=switch_distance){CcircleLock.m_angle_target=(CcircleLock.m_distance_tpcenter-1000*radius)*CcircleLock.m_angle_tptangent/switch_distance;}
		if(CcircleLock.m_distance_tpcenter-1000*radius>switch_distance){CcircleLock.m_angle_target=CcircleLock.m_angle_tptangent;}
		
		CcircleLock.m_angle_target=CcircleLock.m_angle_tpcenter+CcircleLock.m_d_angle-CcircleLock.m_angle_tptangent-90;
		
		Angle_Lock4(CcircleLock.m_angle_target);
	}
	if(CcircleLock.m_distance_tpcenter==1000*radius)
	{Angle_Lock4(CcircleLock.m_angle_tpcenter-180);}
	if(CcircleLock.m_distance_tpcenter<1000*radius)
	{
	CcircleLock.m_d_angle=90*(1000*radius-CcircleLock.m_distance_tpcenter)/1000*radius;	
	CcircleLock.m_angle_target=CcircleLock.m_angle_tpcenter-CcircleLock.m_d_angle-180;
	Angle_Lock4(CcircleLock.m_angle_target);
	}
}
if(direction==1)//顺时针
{}
	
USART_OUT(UART4,(uint8_t*)"%d  ",CcircleLock.m_distance_tpcenter);
USART_OUT(UART4,(uint8_t*)"%d  ",(int)CcircleLock.m_angle_tpcenter);
USART_OUT(UART4,(uint8_t*)"%d\r\n",(int)CcircleLock.m_angle_target);	
}


void Circle_Lock3(void)//【角度闭环】（过圆心交圆一点做切线）
{
CcircleLock.m_distance_tpcenter=sqrt((CcircleLock.m_center_x-positioni.X)*(CcircleLock.m_center_x-positioni.X)+(CcircleLock.m_center_y-positioni.Y)*(CcircleLock.m_center_y-positioni.Y));
CcircleLock.m_angle_tpcenter=180*(atan((CcircleLock.m_center_y-positionf.Y)/(CcircleLock.m_center_x-positionf.X)))/pi;
if(direction==0)
{
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)(positionf.X));

	USART_OUT(UART4,(uint8_t*)"%d  ",(int)(CcircleLock.m_center_y-positionf.Y));
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)(CcircleLock.m_center_x-positionf.X));
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)(CcircleLock.m_center_x-positionf.X));

	USART_OUT(UART4,(uint8_t*)"%d  ",(int)CcircleLock.m_angle_tpcenter);
		
	if(CcircleLock.m_center_x-positioni.X<=0){CcircleLock.m_angle_tpcenter=CcircleLock.m_angle_tpcenter+90;}
	if(CcircleLock.m_center_x>positioni.X) {CcircleLock.m_angle_tpcenter=CcircleLock.m_angle_tpcenter-90;}

	if(CcircleLock.m_angle_tpcenter>180){CcircleLock.m_angle_tpcenter-=360;}
	if(CcircleLock.m_angle_tpcenter<-180){CcircleLock.m_angle_tpcenter+=360;}

	CcircleLock.m_distance_tpcenter=CcircleLock.m_distance_tpcenter-radius*1000;
	if(CcircleLock.m_distance_tpcenter>0)
	{CcircleLock.m_angle_target=CcircleLock.m_angle_tpcenter-90/(1+CcircleLock.m_distance_tpcenter/400);}
	if(CcircleLock.m_distance_tpcenter<0)
	{CcircleLock.m_angle_target=CcircleLock.m_angle_tpcenter-180+90/(1-CcircleLock.m_distance_tpcenter/400);}

	if(CcircleLock.m_angle_target>180){CcircleLock.m_angle_target-=360;}
	if(CcircleLock.m_angle_target<-180){CcircleLock.m_angle_target+=360;}
	Angle_Lock4(CcircleLock.m_angle_target);
}
if(direction==1)//（未尝试/理论可行）
{
	if(CcircleLock.m_center_x-positioni.X<=0){CcircleLock.m_angle_tpcenter=CcircleLock.m_angle_tpcenter-90;}
	if(CcircleLock.m_center_x>positioni.X) {CcircleLock.m_angle_tpcenter=CcircleLock.m_angle_tpcenter+90;}

	if(CcircleLock.m_angle_tpcenter>180){CcircleLock.m_angle_tpcenter-=360;}
	if(CcircleLock.m_angle_tpcenter<-180){CcircleLock.m_angle_tpcenter+=360;}

	CcircleLock.m_distance_tpcenter=CcircleLock.m_distance_tpcenter-radius*1000;
	if(CcircleLock.m_distance_tpcenter>0)
	{CcircleLock.m_d_angle=CcircleLock.m_angle_tpcenter+90/(1+CcircleLock.m_distance_tpcenter/400)-180;}
	if(CcircleLock.m_distance_tpcenter<0)
	{CcircleLock.m_d_angle=CcircleLock.m_angle_tpcenter-90/(1-CcircleLock.m_distance_tpcenter/400);}
		
	if(CcircleLock.m_d_angle>180){CcircleLock.m_d_angle-=360;}
	if(CcircleLock.m_d_angle<-180){CcircleLock.m_d_angle+=360;}
	Angle_Lock4(CcircleLock.m_d_angle);
}
USART_OUT(UART4,(uint8_t*)"%d  ",CcircleLock.m_distance_tpcenter);
USART_OUT(UART4,(uint8_t*)"%d  ",(int)CcircleLock.m_angle_tpcenter);			
USART_OUT(UART4,(uint8_t*)"%d\r\n",(int)CcircleLock.m_d_angle);
}
//====================================================================================
//                                正方形扫荡（方案）
//====================================================================================
int Cho=0;
int square_edg=1900;
int sweep_mode=0;
void Square_Sweep_Right1(int square_m , int square_e)//回到原定路线
{
	if(square_edg>=1900) {sweep_mode=0;}
	if(square_edg<=500)  {sweep_mode=1;}

	if(Cho==0){Line_Lock4(-90,square_m-square_e,0);}
	if(Cho==0&&positioni.X>=square_e-switch_distance){Cho=1;Line_Lock4(0,0,square_e);}
	if(Cho==1){Line_Lock4(0,0,square_e);}
	if(Cho==1&&positioni.Y>=(square_m +square_e-switch_distance)){Cho=2;Line_Lock4(90,square_m +square_e,0);}
	if(Cho==2){Line_Lock4(90,square_m +square_e,0);}
	if(Cho==2&&positioni.X<-square_e+switch_distance){Cho=3;Line_Lock4(180,0,-square_e);}
	if(Cho==3){Line_Lock4(180,0,-square_e);}
	if(Cho==3&&positioni.Y<=square_m -square_e+switch_distance)
	{
	if(sweep_mode==0){Cho=0;Line_Lock4(-90,square_m -square_e,0);square_edg=square_edg-280;}
	if(sweep_mode==1){Cho=0;Line_Lock4(-90,square_m -square_e,0);square_edg=square_edg+280;}
	}
}

void Square_Sweep_Left1(int square_m , int square_e)
{
	if(square_edg>=2000) {sweep_mode=0;}
	if(square_edg<=500)  {sweep_mode=1;}
		
	if(Cho==0){Line_Lock4(90,square_m-square_e,0);}
	if(Cho==0&&positioni.X<=-square_e+switch_distance){Cho=1;Line_Lock4(0,0,-square_e);}
	if(Cho==1){Line_Lock4(0,0,-square_e);}
	if(Cho==1&&positioni.Y>=(square_m +square_e-switch_distance)){Cho=2;Line_Lock4(-90,square_m +square_e,0);}
	if(Cho==2){Line_Lock4(-90,square_m +square_e,0);}
	if(Cho==2&&positioni.X>square_e-switch_distance){Cho=3;Line_Lock4(180,0,square_e);}
	if(Cho==3){Line_Lock4(180,0,square_e);}
	if(Cho==3&&positioni.Y<=square_m -square_e+switch_distance)
	{
	if(sweep_mode==0)
	{Cho=0;Line_Lock4(90,square_m -square_e,0);square_edg=square_edg-300;}
	if(sweep_mode==1)
	{Cho=0;Line_Lock4(90,square_m -square_e,0);square_edg=square_edg+300;}	
	}
}
//====================================================================================
//                                 ADC激光
//====================================================================================
	float G_Adc_A4;
	float G_Adc_A5;

	float V4;
	float V4_f;
	int V4_i;

	float V5;
	float V5_f;
	int V5_i;

	int t_adc=0;
	int left=0,right=0;

void Adc(void)
{
	V4=G_Adc_A4*10/4096;
	V4_f=V4;
	while(V4_f-1>=0)
	{V4_f--;}
	V4_i=V4-V4_f;
	V4_f*=1000;

	V5=G_Adc_A5*10/4096;
	V5_f=V5;
	while(V5_f-1>=0)
	{V5_f--;}
	V5_i=V5-V5_f;
	V5_f*=1000;
	
	USART_OUT(UART4,(uint8_t*)"%d",V5_i);
	// 发字符
	USART_OUT(UART4,(uint8_t*)"%s",".");
	USART_OUT(UART4,(uint8_t*)"%d  ",(int) V5_f);
	
	USART_OUT(UART4,(uint8_t*)"%d",V4_i);
	// 发字符
	USART_OUT(UART4,(uint8_t*)"%s",".");
	USART_OUT(UART4,(uint8_t*)"%d  ",(int) V4_f);
}

void Adc_Check(void)
{
	if(V5<1){t_adc--;}
	if(V4<1){t_adc++;}
	if(t_adc>=100){right=1;}               //判断左、右
	if(t_adc<=-100){left=1;}               //判断左、右
    //反馈数据
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)left);
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)right);		
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)t_adc);
}
//====================================================================================
//                                 死亡重启
//====================================================================================
int t_col=0;
int square_break=0;
float v_calculate;
float v_real;
	
int t_back=0;       //倒退计时
int t_times=0;      //碰撞计次
int t_times_return=0;//计次倒计
	
void Check_Error(void)
{

}
	
void square_edg_jump(void)
{
	if(t_back<=100)
	{
	if(v1_record>=0){Move(-10865,10865);}
	//if(v1_record<=0){Move(10865*2,-10865*2);}
	}
	if(t_back>100)
	{
		if(t_times<3)
		{
			square_break=0;
			t_times++;
			if(sweep_mode==0&&square_edg>=780){square_edg-=280;}
			if(sweep_mode==1&&square_edg<=1620){square_edg+=280;}
			if(square_edg<=500){sweep_mode=1;square_edg+=280;}
			if(square_edg>=1900){sweep_mode=0;square_edg-=280;}
			t_back=0;
			t_col=0;
		}
		if(t_times>=3)
		{
			square_break=0;
			t_times=0;
			t_back=0;
			if(right==1)
			{
				left=1;
				right=0;
				if(Cho==1){Cho=3;}
				if(Cho==3){Cho=1;}
			}
			if(left==1) 
			{
				left=0;
				right=1;
				if(Cho==1){Cho=3;}
				if(Cho==3){Cho=1;}
			}
			t_col=0;
		}
	}
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s  ","E","R","R","O","R");//发送数据
}
	
void Collision_Processing(void)
{
	USART_OUT(UART4,(uint8_t*)"%d  ",t_times);//发送数据
	USART_OUT(UART4,(uint8_t*)"%d  ",x_p10);//发送数据
	USART_OUT(UART4,(uint8_t*)"%d  ",y_p10);//发送数据
	
//	v_calculate=(v1_record-v2_record)/(2*10865);
	v_real=0.1*sqrt((positionf.X-x_now)*(positionf.X-x_now)+(positionf.Y-y_now)*(positionf.Y-y_now));//(m/s)
	
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)(v_real*1000));//发送数据
	
    if(v_real>0.1){t_col=0;t_back=0;}
	if(v_real<0.1){t_col++;}
	if(t_col<200){t_times_return++;}                             //判断可能不充分
	if(t_times_return>=5000&&t_times>0){t_times_return=0;t_times--;}
	if(t_col>=200){square_break=1;}
	if(square_break==1){t_times_return=0;t_back++;square_edg_jump();}
}

