#include "other.h"
extern GunneryData Gundata;
//====================================================================================
//                                    ���ݴ���
//====================================================================================
//��0Ϊ������׺�ر������¶Գ��������ڳ�����ϵ��0~360���¼���
//��1Ϊ������׺�ı������ǶԵر������ڳ�����ϵ��-180~+180���¼���
/*************************************************************************************
                                      ��������
*************************************************************************************/
float ReternMax(float Num1,float Num2)//�������ֵ
{
	return ((Num1)>=(Num2)? (Num1):(Num2));
}

float ReturnMin(float Num3,float Num4)//������Сֵ
{
	return ((Num3)<=(Num4)? (Num3):(Num4));
}

float ReturnAbsolute(float Num5)//���ؾ���ֵ
{
	return ((Num5)>=0? (Num5):(-Num5));
}

float ReturnLimitValue(float Num6, float NumMax, float NumMin)//�����޷�ֵ
{
    return ((Num6)<(NumMin)?(NumMin):((Num6)>(NumMax)?(NumMax):(Num6)));
}

int ReturnJudgeResult(float Num7, float MaxNum, float MinNum)//�����ж�ֵ
{
	return ((Num7)<(MinNum)?(0):((Num7)>(MaxNum)?(0):(1)));
}

float Return_SymmetricRangeValue(float Num8,float Range1)//���ضԳƻ��趨ֵ
{
	do
	{
	if((Num8)>(Range1))  {Num8-=(2*Range1);}
	if((Num8)<(-Range1)) {Num8+=(2*Range1);}
	}
	while( (Num8)<(-Range1) || (Num8)>(Range1) );
	return Num8;
}

float Return_NonSymmetricRangeValue(float Num9,float upper_bound,float lower_bound)//���طǶԳƻ��趨ֵ
{
	do
	{
		if((Num9)>(upper_bound)){Num9-=(upper_bound-lower_bound);}
		if((Num9)<(lower_bound)){Num9+=(upper_bound-lower_bound);}
	}
	while( (Num9)>(upper_bound) || (Num9)<(lower_bound) );
	return Num9;
}

float SetToFort_AngleProcessing(float r_veh1,float r_fort0/*ʵ����̨�Ƕ�*/,float R_fort0/*��ȡ��̨�Ƕ�*/,float setfortangle1)//����̨�Ƕȴ�������Ŀ��Ƕ�(������ϵ��)���趨��̨�Ƕȣ�
{
	return(Return_SymmetricRangeValue((Return_NonSymmetricRangeValue((r_veh1-setfortangle1),360,0)-Return_NonSymmetricRangeValue(r_fort0,360,0)),180)+R_fort0);
}

float FortToGround_AngleProcessing(float r_veh1,float r_fort0)//����̨�Ƕȴ�������Ŀ��Ƕ�(������ϵ��)���趨��̨�Ƕȣ�
{
	return(Return_SymmetricRangeValue((r_veh1+Return_SymmetricRangeValue(-Return_NonSymmetricRangeValue(r_fort0,360,0),180)),180));
}

void GetFloat (float Num10, int places)//�������͸�����
{
	if(Num10<0){USART_OUT(UART4,(uint8_t*)"%s","-");}
	if(Num10==0)  USART_OUT(UART4,(uint8_t*)"%d  ",0);
	else
	{
		Num10=ReturnAbsolute(Num10);
		USART_OUT(UART4,(uint8_t*)"%d%s",(int)Num10,".");
		while(Num10-1>=0){Num10--;}
		Num10*=(pow(10,places));
		USART_OUT(UART4,(uint8_t*)"%d  ",(int)Num10);
	}
}

float ReturnRadian(float ANG)
{
	return (Pi*ANG/180);
}

float ReturnAngle(float ANG)
{
	return (ANG*180/Pi);
}
/*************************************************************************************
                                      ʱ�䴦��*
*************************************************************************************/
int t_Flag=0;
int t_cnt=0;
void Delay(int countTime)//�ӳٺ���
{
	t_cnt=0;
	while(!t_Flag)//�ӳٱ�־λ
	{
		if(t_cnt<countTime){t_cnt+=10;}//�ӳټ�����countTime ms��
		if(t_cnt>=countTime) {t_cnt=0;t_Flag=1;}
	}
	t_cnt=0;
	t_Flag=0;
}

int Timer(void)//��ʱ����
{
	if(t_Flag==0){t_cnt++;return 0;}
	if(t_Flag==1){t_cnt=0;return t_cnt;}
}
/*************************************************************************************
                                      �ع鷽��
*************************************************************************************/
float ReturnVectorAngle1(float x1,float y1,float x2,float y2)//���س�����ϵ�������Ƕ�
{
	if((y2-y1)>0&&(x2-x1)<0) return Return_SymmetricRangeValue(((180*(atan((y2-y1)/(x2-x1)))/Pi)+180-90),180);
	else if((y2-y1)<0&&(x2-x1)<0) return Return_SymmetricRangeValue(((180*(atan((y2-y1)/(x2-x1)))/Pi)-180-90),180);
	else return Return_SymmetricRangeValue(((180*(atan((y2-y1)/(x2-x1)))/Pi)-90),180);
}

float ReturnSigma (float array[2][51],int choose_xy, int num)//������num����֮��
{
    float Sigma=0;
	for(int i=1 ; i<=num ; i++)
	{
		Sigma+=array[choose_xy][i];
	}
	return (Sigma);
}

float ReturnProductSum(float array[2][51] , int num)//������num����x*y��֮��
{
    float Product=0;
	for(int i=1 ; i<=num ; i++)
	{
		Product+=((array[0][i])*(array[1][i]));
	}
	return (Product);
}

float ReturnQuadraticSum(float array[2][51],int choose_xy, int num)//������num������ƽ��֮��
{
    float QuadraticSum=0;
	for(int i=1 ; i<=num ; i++)
	{
		QuadraticSum+=((array[choose_xy][i])*(array[choose_xy][i]));
	}
	return (QuadraticSum);
}

float ReturnAverage (float Array[2][51] , int Choose_xy , int Num)//������num����ƽ��ֵ
{
    float Average=0;
	Average = (ReturnSigma(Array , Choose_xy , Num)/(Num));
	return (Average);
}

float Return_LinearRegressionEquation_k (float ARRAY[2][51], int NUM)//��ع鷽��б��
{
	return (((ReturnProductSum(ARRAY , NUM))-(NUM*(ReturnAverage (ARRAY , 0 , NUM))*(ReturnAverage (ARRAY , 1 , NUM))))/((ReturnQuadraticSum(ARRAY , 0 , NUM))-(NUM*(ReturnAverage (ARRAY , 0 , NUM))*(ReturnAverage (ARRAY , 0 , NUM)))));
}

float Return_LinearRegressionEquation_b (float array[2][51], int num)//��ع鷽�̽ؾ�
{
	return ((ReturnAverage (array , 1 , num))-((Return_LinearRegressionEquation_k (array, num))*(ReturnAverage (array , 0 , num))));
}

float Return_LinearRegressionEquation_angle (float array[2][51], int num)//��ع鷽�̷���
{
	
}

void LinearRegressionEquation_3Coordinates(float x1,float y1,float x2,float y2,float x3,float y3)//����ع鷽��
{
}
//====================================================================================
//                                     ���ⴥ��
//====================================================================================
float Avalue;
float Bvalue;
void Laser_data(void)
{
	Avalue=2.4479*fort.laserAValueReceive+71.215;	
	Bvalue=2.44885*fort.laserBValueReceive+57.925;
}

void Laser_Trigger(void)
{
	
	//���㼤�����(mm)
}
//====================================================================================
//                                   ��Ԫ����
//====================================================================================
void Sweep (void)//����++(���뷽�������α�־λ&������Բ�α�־λ&����)
{}
int error_flag=0;  //�����־λ
int purpose_mode=0;//0Ϊ���� 1ΪͶ�� 2Ϊ���ϣ����ñ��ϻ���Ͷ��
int Sweep_mode=1;  //·����־λ��1����   0��С
int Shape_mode=1;  //��״��־λ1����   0��Բ
void Check_Path (void)//����·���ж�++
{
	if(error_flag==0)
	{
		if(purpose_mode==0)//������λ(ǰ10-15s)
		{

		}
		if(purpose_mode==1)//Ͷ����λ
		{

		}
	}
	if(error_flag==1)//���ϴ���
	{
		if(purpose_mode==2)
		{

		}
	}
}
void Check_Error(void)//error�жϼ��++
{}
void Collision_Process(void)//��ײ����++
{}
//====================================================================================
//                          ����ɨ�裨Ŀ���⣩��ȫ��λ��
//====================================================================================
//	int t_PSBreakdown=0;          //��λϵͳ�쳣����
//	int ifPSBreakdownFlag=0;      //��λϵͳ�쳣Flag

//	float FortAngle0;           //�����̨��ǰ�Ƕȣ�δ������//��
//	float FortAngle1;           //�����̨��ǰ�Ƕȣ�δ������//�Ե�
//	float Set_FortInitialAngle1;//�趨��ʼ�Ƕȣ�δ������//�Ե�
//	float d_angle=0;            //�Ƕ�����
//	float Set_FortAngle1;       //�趨����ɨ�裩�Ƕȣ�δ������//�Ե�

//	int t_laserlock=0;          //������������
//	int laserLock=0;            //Ŀ�������ж�

//	float laserValue;           //���ⷵ��ֵ������Ŀ�����꣩
//	float target_X;             //����Ŀ��X����
//	float target_Y;             //����Ŀ��Y����

//	int TellFortToShoot=0;      //�����־λ���ж�shoot()��

//void Target_Lock(GunneryData *Gun,PID_Value *pos)
//{
//	//���㼤�����(mm)
//	Laser_data();
//	/*����ɨ��Ǽ���*/
///*
//	//��λϵͳ��ը�ж�
//	if(ReturnAbsolute(GetWZ())>=500) {t_PSBreakdown++;}
//	if(ReturnAbsolute(GetWZ())<500)  {t_PSBreakdown=0;}
//	if(t_PSBreakdown>=5){ifPSBreakdownFlag=1;}У����λϵͳ��Ҫ��0
//*/
//	//δ����Ŀ�꣨ɨ���߼���
//	if(!laserLock)
//	{
///*
//		//��λϵͳ����
//		if(!ifPSBreakdownFlag)
//		{
//			//����ǶȲ��䣬����ǰ������ɨ��
//			Set_FortAngle1=0;
//		}
//		//��λϵͳ�쳣
//		if(ifPSBreakdownFlag)
//		{
//*/
//				d_angle++;
//				switch(Gun->BucketNum)
//				{
//					case 0:
//						if(pos->direction == ACW)Set_FortInitialAngle1=-180;
//						if(pos->direction == CW)Set_FortInitialAngle1=-90;
//						break;
//					
//					case 1:
//						if(pos->direction == ACW)Set_FortInitialAngle1=-90;
//						if(pos->direction == CW)Set_FortInitialAngle1=0;
//						break;
//					
//					case 2:
//						if(pos->direction == ACW)Set_FortInitialAngle1=0;
//						if(pos->direction == CW)Set_FortInitialAngle1=90;
//						break;
//					
//					case 3:
//						if(pos->direction == ACW)Set_FortInitialAngle1=90;
//						if(pos->direction == CW)Set_FortInitialAngle1=180;
//						break;
//				}
//				if(GetWZ()<100)//���ٶ�С��100
//				{
//					if(pos->direction == ACW)
//					{
//						if(d_angle<90)   {d_angle++;}
//						if(d_angle==90)  {d_angle=0;}
//					}
//					if(pos->direction == CW)
//					{
//						if(d_angle>-90)  {d_angle--;}
//						if(d_angle==-90) {d_angle=0;}
//					}
//				}
//				if(GetWZ()>100)//���ٶȴ���100
//				{
//					//�ǶȲ���
//					d_angle=0;
//				}
//				Set_FortAngle1=Set_FortInitialAngle1+d_angle;
//				Set_FortAngle1=Return_SymmetricRangeValue(Set_FortAngle1,180.f);
//					
//					//���㺽�����Գ��Ƕ�
//					/*****************/
//					//�趨�������Ƕ�
//					/*****************\
//					 *
//					 *
//					 *
//					 *
//					\*****************/
///*
//		}
//*/
//		//���Ŀ�꣨�ж��߼���
//		{
//			if(ReturnJudgeResult(Avalue,4800,1000)&&ReturnJudgeResult(Bvalue,4800,1000))
//			{
//				if(ReturnAbsolute(Avalue-Bvalue)<75){t_laserlock++;}
//				else t_laserlock=0;
//				if(t_laserlock==3)//��¼��ǰ��̨�Ƕȡ����Ƕȡ����������
//				{
//					FortAngle1=Return_SymmetricRangeValue(((-Return_SymmetricRangeValue(fort.yawPosReceive,180.f))+(pos->Angle)),180);//��̨�ԵؽǶ�
//					laserValue=(Avalue+Bvalue)/2;
//				}
//				if(t_laserlock>=6){t_laserlock=0;laserLock=1;}
//			}
//		}
//	}
//	//������Ŀ�꣨��ȡ��Ϣ��
//	if(laserLock)
//	{
//		//����Ŀ������
//		target_X=Gun->Fort_X;
//		target_Y=Gun->Fort_Y;
//		
//		//Ͷ���laserlock��0
//	}
//}
//====================================================================================
//                                     ������
//====================================================================================
extern PID_Value PID_A;
int ifPushFlag;
Record shooterVel;

void ShooterVel_Record(void)//��¼����
{
    shooterVel.p100=shooterVel.p90;
	shooterVel.p90=shooterVel.p80;
	shooterVel.p80=shooterVel.p70;
	shooterVel.p70=shooterVel.p60;
	shooterVel.p60=shooterVel.p50;
	shooterVel.p50=shooterVel.p40;
	shooterVel.p40=shooterVel.p30;
	shooterVel.p30=shooterVel.p20;
	shooterVel.p20=shooterVel.p10;
	shooterVel.p10=shooterVel.now;
	shooterVel.now=Gundata.ShooterVelRec;
}

int ifShootFlag=0;
int ifPushFlag_remain=0;
int t_ifpushflag=0;
void Remain_ifPushFlag(void)//ifPushFlag��־����250ms
{
	ifPushFlag=PID_A.fire_command;
	
	if(ifPushFlag==1){ifPushFlag_remain=1;}
	if(ifPushFlag_remain==1)
	{
		t_ifpushflag++;
		if(t_ifpushflag<=35){ifPushFlag=1;}
		if(t_ifpushflag>35) {ifPushFlag_remain=0;t_ifpushflag=0;}
	}
}

//void Filter(void)//�˲�����
//{}
void Data_Processing(void)//���ݴ���
{
//�˲�
//����
//΢��
}

//�������
int Bucket_0_ShootNum=0;
int Bucket_1_ShootNum=0;
int Bucket_2_ShootNum=0;
int Bucket_3_ShootNum=0;
//���д���
int Bucket_0_HitNum=0;
int Bucket_1_HitNum=0;
int Bucket_2_HitNum=0;
int Bucket_3_HitNum=0;

void Shoot_Judge(void)//������
{
	if(ifPushFlag==1)
	{
		if(shooterVel.p100-shooterVel.p90>=6||shooterVel.p100-shooterVel.p80>=6)
		{
			if(shooterVel.p100-shooterVel.now<=6){ifShootFlag=1;}
			else{ifShootFlag=0;}
		}
		else{ifShootFlag=0;}
	}
	else{ifShootFlag=0;}
	
	if(ifShootFlag==1)
	{
		ifPushFlag_remain=0;
		t_ifpushflag=0;
		if(Gundata.BucketNum==0){Bucket_0_ShootNum++;}
		if(Gundata.BucketNum==1){Bucket_1_ShootNum++;}	
		if(Gundata.BucketNum==2){Bucket_2_ShootNum++;}
		if(Gundata.BucketNum==3){Bucket_3_ShootNum++;}	
		ifShootFlag=0;
	}
}

//====================================================================================
//                                  Ͷ���߼�/adc��ȫ����
//====================================================================================
void Bucket_Choose (void)//����
{

}
//====================================================================================
//                                 �Ӿ�ϵͳ+�������
//====================================================================================
int ballCalour;// 0Ϊ����;1Ϊ����
void JudgeBallColour()
{

}
//====================================================================================
//                                   �³��������
//====================================================================================
float laserAvaluereceive;
float laserBvaluereceive; 
float Xvaluereceive;
float Yvaluereceive; 
void GetLaserData (void)
{
	laserAvaluereceive=fort.laserAValueReceive;
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","A","a","d","c",":");	
	GetFloat (laserAvaluereceive, 3);
	
	laserBvaluereceive=fort.laserBValueReceive;
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","B","a","d","c",":");
	GetFloat (laserBvaluereceive, 3);
}

void GetPositionValue(PID_Value *pid_out)//�����������
{
	Xvaluereceive=pid_out->X;
	Yvaluereceive=pid_out->Y-2400; 
	
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","p","o","s","X",":");
	GetFloat (Xvaluereceive, 3);
	
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","p","o","s","Y",":");
	GetFloat (Yvaluereceive, 3);
}

//====================================================================================
//                                  �Ƕ�������ok��
//====================================================================================
/*
void SetFortAngle(PID_Value *pos,float set_angle)//
{
	Set_FortAngle1=SetToFort_AngleProcessing(pos->Angle,fort.yawPosReceive,set_angle);
	
	r_fortAngle1=Return_SymmetricRangeValue((pos->Angle+Return_SymmetricRangeValue(-Return_NonSymmetricRangeValue(fort.yawPosReceive,360,0),180)),180);
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","F","A","n","g",":");
	GetFloat (r_fortAngle1, 3);
}
*/
//====================================================================================
//                                  �״�ʵ��
//====================================================================================
float setangle=0;           //

float r_fortAngle1;         //
float r_fortAngle0;         //
float r_standardFortAngle;  //
float Set_FortAngle1;       //�趨����ɨ�裩�Ƕȡ��Եء���-180��180��

float A_targetx_now=0;      //
float A_targety_now=0;      //
float B_targetx_now=0;      //
float B_targety_now=0;      //
float A_targetx_past=0;     //
float A_targety_past=0;     //
float B_targetx_past=0;     //
float B_targety_past=0;     //
int   ifkeepFlag;           //

float A_targetx;            //
float A_targety;            //
float B_targetx;            //
float B_targety;            //

float post_fortAngle=-60;   //
float FortPost_Flag=1;      //
float t_post=0;             //

float CorrectiveAngle=0;    //

int   abandonFlag=0;        //

float APositionRecord[2][4]={{0},{0}};
float BPositionRecord[2][4]={{0},{0}};
float ATargetPositionRecord[2][51]={{0},{0}};
float BTargetPositionRecord[2][51]={{0},{0}};
//int ai,aj,bi,bj,ap,aq,bp,bq;

float Fort_X;
float Fort_Y;
float A_Laser_X;
float A_Laser_Y;
float B_Laser_X;
float B_Laser_Y;
void DataProcessing (PID_Value *p)
{
		//������̨�Ƕ�
		r_fortAngle0=fort.yawPosReceive+Compensation_Angle;

		//������̨����
		Fort_X = p->X - (10.5f) * sin(p->Angle * Pi / 180.0);
		Fort_Y = p->Y + (10.5f) * cos(p->Angle * Pi / 180.0);	
		//���㼤������
		r_fortAngle1=FortToGround_AngleProcessing(p->Angle,r_fortAngle0);
		A_Laser_X=Fort_X-40.f*sin(Return_SymmetricRangeValue((r_fortAngle1+90.f),180)*Pi/180)-23.32*sin(Return_SymmetricRangeValue(r_fortAngle1,180)*Pi/180.f);
		A_Laser_Y=Fort_Y+40.f*cos(Return_SymmetricRangeValue((r_fortAngle1+90.f),180)*Pi/180)+23.32*cos(Return_SymmetricRangeValue(r_fortAngle1,180)*Pi/180.f);
		B_Laser_X=Fort_X-40.f*sin(Return_SymmetricRangeValue((r_fortAngle1-90.f),180)*Pi/180)-23.32*sin(Return_SymmetricRangeValue(r_fortAngle1,180)*Pi/180.f);
		B_Laser_Y=Fort_Y+40.f*cos(Return_SymmetricRangeValue((r_fortAngle1-90.f),180)*Pi/180)+23.32*cos(Return_SymmetricRangeValue(r_fortAngle1,180)*Pi/180.f);
}

void SetFortAngle(PID_Value *pos,float set_angle)//
{
		DataProcessing (pos);
		Set_FortAngle1=SetToFort_AngleProcessing(pos->Angle,r_fortAngle0,fort.yawPosReceive,set_angle);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","P","A","n","g",":");
			GetFloat (pos->Angle, 3);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","F","A","n","g",":");
			GetFloat (fort.yawPosReceive, 3);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","R","A","n","g",":");
			GetFloat (r_fortAngle0, 3);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","s","A","n","g",":");
			GetFloat (set_angle, 3);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","S","A","n","g",":");
			GetFloat (Set_FortAngle1, 3);

		r_fortAngle1=FortToGround_AngleProcessing(pos->Angle,r_fortAngle0);
//		r_fortAngle1=Return_SymmetricRangeValue((pos->Angle+Return_SymmetricRangeValue(-Return_NonSymmetricRangeValue(fort.yawPosReceive,360,0),180)),180);
		USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","F","A","n","g",":");
		GetFloat (r_fortAngle1, 3);

}
int ifAFirstOFOK=0;
int ifBFirstOFOK=0;
int cntNumA=0;
int cntNumB=0;
void FirstOrder_Filter(PID_Value *p)//�˲�����
{
	//���˳�����Χ���
	if(sqrt((A_targetx-(A_Laser_X))*(A_targetx-(A_Laser_X))+(A_targety-(A_Laser_Y)+2400)*(A_targety-(A_Laser_Y)+2400))>=400)
	{
		//��ȥ����Χ��
		if(sqrt(A_targetx*A_targetx+A_targety*A_targety)>2000&&sqrt(A_targetx*A_targetx+A_targety*A_targety)<4500)
		{
			A_targetx_past=A_targetx_now;
			A_targety_past=A_targety_now;
			A_targetx_now=A_targetx;
			A_targety_now=A_targety;
			//��ȥ�¶���
			if(sqrt((A_targetx_past-A_targetx_now)*(A_targetx_past-A_targetx_now)+(A_targety_past-A_targety_now)*(A_targety_past-A_targety_now))<=150||\
			   sqrt((A_targetx_past-APositionRecord[0][2])*(A_targetx_past-APositionRecord[0][2])+(A_targety_past-APositionRecord[1][2])*(A_targety_past-APositionRecord[1][2]))<=150)
			{
				APositionRecord[0][0]=A_targetx_past;
				APositionRecord[1][0]=A_targety_past;
				for(int ap=0 ; ap<=1 ; ap++)
				{
					for(int aq=3 ; aq>=1 ; aq--)
					{
						APositionRecord[ap][aq]=APositionRecord[ap][aq-1];
					}
				}
				ifAFirstOFOK=1;
				/*����*/
				{
					/*
						USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","A","T","L","X",":");
						GetFloat (A_targetx_past, 3);
						USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","A","T","L","Y",":");
						GetFloat (A_targety_past, 3);
					*/
				}
			}
		}
	}
	
	//���˳�����Χ���
	if(sqrt((B_targetx-(B_Laser_X))*(B_targetx-(B_Laser_X))+(B_targety-(B_Laser_Y)+2400)*(B_targety-(B_Laser_Y)+2400))>=400)
	{
		//��ȥ����Χ��
		if(sqrt(B_targetx*B_targetx+B_targety*B_targety)>2000&&sqrt(B_targetx*B_targetx+B_targety*B_targety)<4500)
		{
			B_targetx_past=B_targetx_now;
			B_targety_past=B_targety_now;
			B_targetx_now=B_targetx;
			B_targety_now=B_targety;
			//��ȥ�¶���
			if(sqrt((B_targetx_past-B_targetx_now)*(B_targetx_past-B_targetx_now)+(B_targety_past-B_targety_now)*(B_targety_past-B_targety_now))<=150||\
			   sqrt((B_targetx_past-BPositionRecord[0][2])*(B_targetx_past-BPositionRecord[0][2])+(B_targety_past-BPositionRecord[1][2])*(B_targety_past-BPositionRecord[1][2]))<=150)
			{
				BPositionRecord[0][0]=B_targetx_past;
				BPositionRecord[1][0]=B_targety_past;
				for(int bp=0 ; bp<=1 ; bp++)
				{
					for(int bq=3 ; bq>=1 ; bq--)
					{
						BPositionRecord[bp][bq]=BPositionRecord[bp][bq-1];
					}
				}
				ifBFirstOFOK=1;
				/*����*/
				{
					/*
						USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","B","T","L","X",":");
						GetFloat (B_targetx_past, 3);
						USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","B","T","L","Y",":");
						GetFloat (B_targety_past, 3);
					*/
				}
			}
		}
	}

}

void CheckLine(void)
{
	
}

void SecondOrder_Filter(void)//�˲�����
{
	if(ifAFirstOFOK==1)
	{
			/*�����˲�*/
			if(sqrt((APositionRecord[0][2]-APositionRecord[0][1])*(APositionRecord[0][2]-APositionRecord[0][1])+(APositionRecord[1][2]-APositionRecord[1][1])*(APositionRecord[1][2]-APositionRecord[1][1]))<=150||\
			   sqrt((APositionRecord[0][3]-APositionRecord[0][2])*(APositionRecord[0][3]-APositionRecord[0][2])+(APositionRecord[1][3]-APositionRecord[1][2])*(APositionRecord[1][3]-APositionRecord[1][2]))<=150)
			{
					cntNumA++;
					ATargetPositionRecord[0][0]=APositionRecord[0][2];
					ATargetPositionRecord[1][0]=APositionRecord[1][2];
					for(int ai=0 ; ai<=1 ; ai++)
					{
						for(int aj=50; aj>=1 ; aj--)
						{
							ATargetPositionRecord[ai][aj]=ATargetPositionRecord[ai][aj-1];
						}
					}
					if(cntNumA>=2&&sqrt((ATargetPositionRecord[0][2]-ATargetPositionRecord[0][1])*(ATargetPositionRecord[0][2]-ATargetPositionRecord[0][1])+\
					  (ATargetPositionRecord[1][2]-ATargetPositionRecord[1][1])*(ATargetPositionRecord[1][2]-ATargetPositionRecord[1][1]))>200)
					{
						if(cntNumA>=6)
						/*__*\
					   |*|__|*|���鳤�ȴ���50�ᷢ0���������
						\*__*/
						{
							/*��������*/
							{
								
							}
							/*����*/
							{
									for(int i=cntNumA;i>=1;i--)
									{
									USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s","A","X",":");
									GetFloat (ATargetPositionRecord[0][i], 3);
									USART_OUT(UART4,(uint8_t*)"%s%s%s","A","Y",":");
									GetFloat (ATargetPositionRecord[1][i], 3);
									}
							}
						}
						cntNumA=0;
						//ATargetPositionRecord��������
						memset(ATargetPositionRecord,0,sizeof(ATargetPositionRecord));
					}
			}
		ifAFirstOFOK=0;
	}
	if(ifBFirstOFOK==1)
	{
			/*�����˲�*/
			if(sqrt((BPositionRecord[0][2]-BPositionRecord[0][1])*(BPositionRecord[0][2]-BPositionRecord[0][1])+(BPositionRecord[1][2]-BPositionRecord[1][1])*(BPositionRecord[1][2]-BPositionRecord[1][1]))<=150||\
			   sqrt((BPositionRecord[0][3]-BPositionRecord[0][2])*(BPositionRecord[0][3]-BPositionRecord[0][2])+(BPositionRecord[1][3]-BPositionRecord[1][2])*(BPositionRecord[1][3]-BPositionRecord[1][2]))<=150)
			{
					cntNumB++;
					BTargetPositionRecord[0][0]=BPositionRecord[0][2];
					BTargetPositionRecord[1][0]=BPositionRecord[1][2];
					for(int bi=0 ; bi<=1 ; bi++)
					{
						for(int bj=50; bj>=1 ; bj--)
						{
							BTargetPositionRecord[bi][bj]=BTargetPositionRecord[bi][bj-1];
						}
					}
					if(cntNumB>=2&&sqrt((BTargetPositionRecord[0][2]-BTargetPositionRecord[0][1])*(BTargetPositionRecord[0][2]-BTargetPositionRecord[0][1])+\
					  (BTargetPositionRecord[1][2]-BTargetPositionRecord[1][1])*(BTargetPositionRecord[1][2]-BTargetPositionRecord[1][1]))>200)
					{
						if(cntNumB>=6)
						/*__*\
					   |*|__|*|���鳤�ȴ���50�ᷢ0���������
						\*__*/
						{
							/*��������*/
							{
								
							}
							/*����*/
							{
									for(int i=cntNumB;i>=1;i--)
									{
									USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s","B","X",":");
									GetFloat (BTargetPositionRecord[0][i], 3);
									USART_OUT(UART4,(uint8_t*)"%s%s%s","B","Y",":");
									GetFloat (BTargetPositionRecord[1][i], 3);
									}
							}
						}
						cntNumB=0;
						//ATargetPositionRecord��������
						memset(BTargetPositionRecord,0,sizeof(BTargetPositionRecord));
					}
			}
		ifBFirstOFOK=0;
	}
}

void Power_On_Self_Test(PID_Value *pos)//���ӵ��Լ졿
{
	DataProcessing (pos);
	t_post++;
	Laser_data();
	/*��̨���������������ӵ��Լ�*/
		if(t_post<=240){post_fortAngle+=0.5;}
		if(t_post>240) {post_fortAngle=0;}
		Set_FortAngle1=SetToFort_AngleProcessing(pos->Angle,r_fortAngle0,fort.yawPosReceive,post_fortAngle);//
					USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","S","A","n","g",":");
					GetFloat (Set_FortAngle1, 3);
		
		{
			/*����Ŀ������*/
//			r_fortAngle1=Return_SymmetricRangeValue((pos->Angle+Return_SymmetricRangeValue(-Return_NonSymmetricRangeValue(fort.yawPosReceive,360,0),180)),180);
			r_fortAngle1=FortToGround_AngleProcessing(pos->Angle,r_fortAngle0);/**///��̨�ԵؽǶȣ�������ϵ��
			r_standardFortAngle=Return_SymmetricRangeValue((r_fortAngle1+90.f),180.f);   //��̨�ԵؽǶȣ���׼����ϵ��
			A_targetx=Avalue*(cos(Pi*r_standardFortAngle/180.f))+A_Laser_X;
			A_targety=Avalue*(sin(Pi*r_standardFortAngle/180.f))+A_Laser_Y-2400.f;
			B_targetx=Bvalue*(cos(Pi*r_standardFortAngle/180.f))+B_Laser_X;
			B_targety=Bvalue*(sin(Pi*r_standardFortAngle/180.f))+B_Laser_Y-2400.f;
					USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","A","T","a","r","X",":");
					GetFloat (A_targetx, 3);
					USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","A","T","a","r","Y",":");
					GetFloat (A_targety, 3);
					USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","B","T","a","r","X",":");
					GetFloat (B_targetx, 3);
					USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","B","T","a","r","Y",":");
					GetFloat (B_targety, 3);
		}
		FirstOrder_Filter(pos);
	/*��̨���������������Ƕȳ�ʼ����0��У����*/
		
	/*��̨���������������ӵ��Լ�*/
		
	/*���֡��������������ӵ��Լ�*/
		
	/*���ӡ������ҵ�������ӵ��Լ�*/
		
	/*POST�ӵ��Լ�Finish*/
}

void RadarCorrection(PID_Value *pos)//�״�У��ϵͳ
{
	DataProcessing (pos);
	//������������ɨ��ָ��
	Laser_data();
	setangle+=0.5;
	if(setangle>360){setangle-=360;}
	Set_FortAngle1=SetToFort_AngleProcessing(pos->Angle,r_fortAngle0,fort.yawPosReceive,setangle);//
//			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","S","A","n","g",":");
//			GetFloat (Set_FortAngle1, 3);
	//����Ƕ�
//	r_fortAngle1=Return_SymmetricRangeValue((pos->Angle+Return_SymmetricRangeValue(-Return_NonSymmetricRangeValue(fort.yawPosReceive,360,0),180)),180);
	r_fortAngle1=FortToGround_AngleProcessing(pos->Angle,r_fortAngle0);/**///��̨�ԵؽǶȣ�������ϵ��
			//���ؽǶ�
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","P","A","n","g",":");
			GetFloat (pos->Angle, 3);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","F","A","n","g",":");
			GetFloat (r_fortAngle1, 3);
	//��������
	r_standardFortAngle=Return_SymmetricRangeValue((r_fortAngle1+90.f),180.f);//��̨�ԵؽǶȣ���׼����ϵ��
	A_targetx=Avalue*(cos(Pi*r_standardFortAngle/180.f))+A_Laser_X;/////////|����������Ϊ����������
	A_targety=Avalue*(sin(Pi*r_standardFortAngle/180.f))+A_Laser_Y-2400.f;////|����������Ϊ����������
	B_targetx=Bvalue*(cos(Pi*r_standardFortAngle/180.f))+B_Laser_X;/////////|����������Ϊ����������
	B_targety=Bvalue*(sin(Pi*r_standardFortAngle/180.f))+B_Laser_Y-2400.f;////|����������Ϊ����������
			//��������
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","A","T","a","r","X",":");
			GetFloat (A_targetx, 3);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","A","T","a","r","Y",":");
			GetFloat (A_targety, 3);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","B","T","a","r","X",":");
			GetFloat (B_targetx, 3);
			USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","B","T","a","r","Y",":");
			GetFloat (B_targety, 3);
	//���ݷ���
	FirstOrder_Filter(pos);
	SecondOrder_Filter();
}

int possibility=0;
void Check_Target(void)
{
	Laser_data();	
}














void Entertainment(void)
{
/*
USART_OUT(UART4,(uint8_t*)"%s\r\n","		  ______           ___________   _______________   _____________     ___________      ____     ___");
USART_OUT(UART4,(uint8_t*)"%s\r\n","		 /      \         /           \ /               \ /             \   /           \    /    \   /   \");
USART_OUT(UART4,(uint8_t*)"%s\r\n","		/   /\   \       /    ________/ \_____     _____/ \____     ____/  /    _____    \  |      \  |    |");
USART_OUT(UART4,(uint8_t*)"%s\r\n","	   /   /  \   \     |    /                |   |            |   |      |    /     \    | |       \ |    |");
USART_OUT(UART4,(uint8_t*)"%s\r\n","	  /   /____\   \    |   |                 |   |            |   |      |   |       |   | |    \   \|    |");
USART_OUT(UART4,(uint8_t*)"%s\r\n","	 /   ________   \   |   |                 |   |            |   |      |   |       |   | |    |\   \    |");
USART_OUT(UART4,(uint8_t*)"%s\r\n","	/   /        \   \  |    \________        |   |        ____|   |____  |    \_____/    | |    | \       |");
USART_OUT(UART4,(uint8_t*)"%s\r\n","   /   /          \   \  \            \       |   |       /             \  \             /  |    |  \      |");
USART_OUT(UART4,(uint8_t*)"%s\r\n","  |___/            \___|  \___________/        \_/        \_____________/   \___________/    \___/   \____/");
*/
}

/*
		  ______          ___________   _______________   _____________     ___________      ____     ___
		 /      \        /           \ /               \ /             \   /           \    /    \   /   \  
		/   /\   \      /    ________/ \_____     _____/ \____     ____/  /    _____    \  |      \  |    |
	   /   /  \   \    |    /                |   |            |   |      |    /     \    | |       \ |    |  
	  /   /____\   \   |   |                 |   |            |   |      |   |       |   | |    \   \|    |
	 /   ________   \  |   |                 |   |            |   |      |   |       |   | |    |\   \    |
	|   /        \   \ |    \________        |   |        ____|   |____  |    \_____/    | |    | \       | 
    |   |        |   |  \            \       |   |       /             \  \             /  |    |  \      |
    |___|        |___|   \___________/        \_/        \_____________/   \___________/    \___/   \____/  
*/
/*
		  ______           ___________   _______________   _____________     ___________      ____     ___
		 /      \         /           \ /               \ /             \   /           \    /    \   /   \  
		/   /\   \       /    ________/ \_____     _____/ \____     ____/  /    _____    \  |      \  |    |
	   /   /  \   \     |    /                |   |            |   |      |    /     \    | |       \ |    |  
	  /   /____\   \    |   |                 |   |            |   |      |   |       |   | |    \   \|    |
	 /   ________   \   |   |                 |   |            |   |      |   |       |   | |    |\   \    |
	/   /        \   \  |    \________        |   |        ____|   |____  |    \_____/    | |    | \       | 
   /   /          \   \  \            \       |   |       /             \  \             /  |    |  \      |
  |___/            \___|  \___________/        \_/        \_____________/   \___________/    \___/   \____/  
*/
