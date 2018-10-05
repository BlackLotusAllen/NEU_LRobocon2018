#include "other.h"
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

extern GunneryData Gundata;
extern int radarDebug;
extern int rollerDebug;
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

float Return_Angle(float array1[2][101] , int a1 , float array2[2][101] , int a2) //�����Ƕȣ����飩��������
{
	float angle=0.f;
	angle=((180.f*(atan((array2[1][a2]-array1[1][a1])/(array2[0][a2]-array1[0][a1]))))/Pi);
	if     ((array2[1][a2]-array1[1][a1])>0&&(array2[0][a2]-array1[0][a1])<0)
	{return Return_SymmetricRangeValue((angle+180.f),180.f);}
	else if((array2[1][a2]-array1[1][a1])<0&&(array2[0][a2]-array1[0][a1])<0)
	{return Return_SymmetricRangeValue((angle-180.f),180.f);}
	else
	{return Return_SymmetricRangeValue((angle),180.f);}
}

float Return_Angle2(float A_X,float A_Y,float B_X,float B_Y) //�����Ƕȣ����꣩���� ����A->B
{
	float angle=0.f;
	angle=((180.f*(atan((B_Y-A_Y)/(B_X-A_X))))/Pi);
	if     ((B_Y-A_Y)>0&&(B_X-A_X)<0)
	{return Return_SymmetricRangeValue((angle+180.f),180.f);}
	else if((B_Y-A_Y)<0&&(B_X-A_X)<0)
	{return Return_SymmetricRangeValue((angle-180.f),180.f);}
	else
	{return Return_SymmetricRangeValue((angle),180.f);}
}

float Distance (float Array1[2][101] , int a1 , float Array2[2][101] , int a2)//��������+
{
	return (sqrt((Array1[0][a1]-Array2[0][a2])*(Array1[0][a1]-Array2[0][a2])+(Array1[1][a1]-Array2[1][a2])*(Array1[1][a1]-Array2[1][a2])));
}

float SetToFort_AngleProcessing(float r_veh1,float r_fort0/*ʵ����̨�Ƕ�*/,float R_fort0/*��ȡ��̨�Ƕ�*/,float setfortangle1)//����̨�Ƕȴ�������Ŀ��Ƕ�(������ϵ��)���趨��̨�Ƕȣ�
{
	return(Return_SymmetricRangeValue((Return_NonSymmetricRangeValue((r_veh1-setfortangle1),360,0)-Return_NonSymmetricRangeValue(r_fort0,360,0)),180)+R_fort0);
}

float FortToGround_AngleProcessing(float r_veh1,float r_fort0)//����̨�Ƕȴ�����ȡ��̨�Ƕ�r_fort0������ԵؽǶȣ�
{
	return(Return_SymmetricRangeValue((r_veh1+Return_SymmetricRangeValue(-Return_NonSymmetricRangeValue(r_fort0,360,0),180)),180));
}

float TargetToFort_AngleProcessing(float Teh_X,float Teh_Y,float Target_X,float Target_Y,float r_Veh1/*��ȡ���Ƕ�*/,float r_Fort0/*ʵ����̨�Ƕ�*/,float R_Fort0/*��ȡ��̨�Ƕ�*/)//����̨�Ƕȴ�������Ŀ�����꣬������̨�Ƕȣ�
{
	return (SetToFort_AngleProcessing(r_Veh1,r_Fort0,R_Fort0,(Return_SymmetricRangeValue((Return_Angle2(Teh_X,Teh_Y,Target_X,Target_Y)-90.f),180.f))));
}

/***************************BUG��ӡ*****************************/
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
/***************************BUG��ӡ*****************************/

float ReturnRadian(float ANG)
{
	return (Pi*ANG/180);
}

float ReturnAngle(float ANG)
{
	return (ANG*180/Pi);
}

float ReturnArrayMin (int Num , float array[])
{
	float min=11111111.f;
	for(int i=Num ; i>=0 ; i--)
	{
		if((min-array[i])>0){min=array[i];}
	}
	return min;
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
/*************************************************************************************
                                      �ع鷽��
*************************************************************************************/
float ReturnVectorAngle1(float x1,float y1,float x2,float y2)//���س�����ϵ�������Ƕ�
{
	if((y2-y1)>0&&(x2-x1)<0) return Return_SymmetricRangeValue(((180*(atan((y2-y1)/(x2-x1)))/Pi)+180-90),180);
	else if((y2-y1)<0&&(x2-x1)<0) return Return_SymmetricRangeValue(((180*(atan((y2-y1)/(x2-x1)))/Pi)-180-90),180);
	else return Return_SymmetricRangeValue(((180*(atan((y2-y1)/(x2-x1)))/Pi)-90),180);
}

float ReturnSigma (float array1[2][101],int choose_xy, int num)//������num����֮�͡���[1]��ʼ��
{
    float Sigma=0;
	for(int i=1 ; i<=num ; i++)
	{
		Sigma+=array1[choose_xy][i];
	}
	return (Sigma);
}

float ReturnProductSum(float array2[2][101] , int num)//������num����x*y��֮�͡���[1]��ʼ��
{
    float Product=0;
	for(int i=1 ; i<=num ; i++)
	{
		Product+=((array2[0][i])*(array2[1][i]));
	}
	return (Product);
}

float ReturnQuadraticSum(float array3[2][101],int choose_xy, int num)//������num������ƽ��֮�͡���[1]��ʼ��
{
    float QuadraticSum=0;
	for(int i=1 ; i<=num ; i++)
	{
		QuadraticSum+=((array3[choose_xy][i])*(array3[choose_xy][i]));
	}
	return (QuadraticSum);
}

float ReturnAverage (float array4[2][101] , int Choose_xy , int Num)//������num����ƽ��ֵ����[1]��ʼ��
{
    float Average=0;
	Average = (ReturnSigma(array4 , Choose_xy , Num)/(Num));
	return (Average);
}

float Return_LinearRegressionEquation_k (float array5[2][101], int NUM)//��ع鷽��б�ʡ���[1]��ʼ��
{
	return (((ReturnProductSum(array5 , NUM))-(NUM*(ReturnAverage (array5 , 0 , NUM))*(ReturnAverage (array5 , 1 , NUM))))/((ReturnQuadraticSum(array5 , 0 , NUM))-(NUM*(ReturnAverage (array5 , 0 , NUM))*(ReturnAverage (array5 , 0 , NUM)))));
}

float Return_LinearRegressionEquation_b (float array6[2][101], int num)//��ع鷽�̽ؾࡾ��[1]��ʼ��
{
	return ((ReturnAverage (array6 , 1 , num))-((Return_LinearRegressionEquation_k (array6, num))*(ReturnAverage (array6 , 0 , num))));
}

float Return_LinearRegressionEquation_r (float array7[2][101], int NUM)
{
	return (((ReturnProductSum(array7 , NUM))-(NUM*(ReturnAverage (array7 , 0 , NUM))*(ReturnAverage (array7 , 1 , NUM))))/(sqrt(ReturnAbsolute(((ReturnQuadraticSum(array7 , 0 , NUM))-(NUM*(ReturnAverage (array7 , 0 , NUM))*(ReturnAverage (array7 , 0 , NUM))))*((ReturnQuadraticSum(array7 , 1 , NUM))-(NUM*(ReturnAverage (array7 , 1 , NUM))*(ReturnAverage (array7 , 1 , NUM))))))));
}

float Return_LinearRegressionEquation_angle (float array7[2][101], int num)//��ع鷽�̷���
{
	return 0;
}//�����顿

float ReturnDistance(float array8[2][101] , int pointNum , float k , float b)
{
	return (ReturnAbsolute((k*array8[0][pointNum]-array8[1][pointNum]+b)/(sqrt(1.f+k*k))));
}

void Exchange(float *x, float *y)
{
	float  tmp =*x;
	*x =* y;
	*y = tmp;
}

void LinearRegressionEquation_3Coordinates(float x1,float y1,float x2,float y2,float x3,float y3)//����ع鷽��
{
}

/*************************************************************************************
                                      other
*************************************************************************************/
int cmp_p ( const void *m , const void *n )
{
	return *(int *)m - *(int *)n;  //��������++
}

int cmp_r ( const void *m , const void *n )
{
	return *(int *)n - *(int *)m;  //��������--
}
//====================================================================================
//                                     ���ⴥ��
//====================================================================================
float Avalue;
float Bvalue;
void Laser_data(void)
{
//	Avalue=2.4479*fort.laserAValueReceive+71.215;	
//	Bvalue=2.44885*fort.laserBValueReceive+57.925;
	Avalue=(float)(2.4621f*fort.laserAValueReceive+29.234f);	
	Bvalue=(float)(2.4706f*fort.laserBValueReceive+11.899f);
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
float setangle=0.f;           //

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
float ATargetPositionRecord[2][101]={{0},{0}};
float BTargetPositionRecord[2][101]={{0},{0}};
//int ai,aj,bi,bj,ap,aq,bp,bq;

float Fort_X;
float Fort_Y;
float A_Laser_X;
float A_Laser_Y;
float B_Laser_X;
float B_Laser_Y;

float Corner[2][101];      //����ֱ�ǵ㼯
int Corner_Num=0;          //Corner������Ч������1��
float CheckPoint[2][101];  //�������е㼯
int Check_Num=0;           //CheckPoint������Ч������1��

float RangeF[2][101];      //����4908+-����
int F_Num=0;
float RangeS[2][101];      //����6940+-����
int S_Num=0;

int CheckPermit=0;
float SquarePos[2][4];

void DataProcessing (PID_Value *p)
{
		//������̨�Ƕ�
		r_fortAngle0=fort.yawPosReceive+Compensation_Angle;

		//������̨����
		Fort_X = p->X - (10.5f) * sin(p->Angle * Pi / 180.0f);
		Fort_Y = p->Y + (10.5f) * cos(p->Angle * Pi / 180.0f);	
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

int SquareLock=0;

void  CheckCorner  (float array9 [2][101] , int num)//num��1��ʼ����,num>6
{
	float k;
	float b;
	float r;
	int   CntCorner=0;             		/*ʹ�����0����*/ //����ȷ���ǵ����
	int   CntCorner_FB=0;////////////////
	float SectionPosRecord[2][101];		/*ʹ�����0����*/ //���Ʒֶ������¼        ����[1]��ʼ��
	int   sectionPosLength=0;           /*ʹ�����0����*/ //���Ʒֶγ��ȼ�¼
	int   CornerPointNum   [11];   		/*ʹ�����0����*/ //���ƽǵ�����λ�ü�¼    ����[0]��ʼ��
	int   CornerPointNum_FB[11];/////////
	float distance=0;
	int   Number=0;
	int   number=0;
	float For[2][101]; 
	float Bac[2][101];
	int   ifJudgeAngle=0;
	int   ifCornerIncrease=0;
	
	//ֱ�߽ǵ�num�����ʼ��&&������λֵ
		for(int i=0;i<=10;i++)
		{
			CornerPointNum[i]=111;
		}
		CornerPointNum[0]=1;
		CornerPointNum[1]=num;
	do
	{
		ifCornerIncrease=0;
		//��cornerNum���ǵ�λ�ý�����������
		qsort(CornerPointNum,11,sizeof(CornerPointNum[0]),cmp_p);//��������
		for(int i=0 ; i<=10 ; i++)
        {
            CornerPointNum_FB[i]=CornerPointNum[i];
        }
		CntCorner_FB=CntCorner;
		/*�Ե��ƽ��зֶ�*/
		//�����Ʒֳ�cornerNum+1���ֶ�
		for(int j=0 ; j<=CntCorner ; j++)/*ÿȷ��һ���ǵ㣬flag=1��Cnt++*/
		{
			qsort(CornerPointNum_FB,11,sizeof(CornerPointNum_FB[0]),cmp_p);//��������
			//�ֶ�����
			/*���Ʒֶ������¼*/
			memset(SectionPosRecord,0,sizeof(SectionPosRecord));
				for(int p=CornerPointNum[j+1] ; p>=CornerPointNum[j] ; p--)
				{
					//SectionPosRecord�����¼��[1]��ʼ
					SectionPosRecord[0][p-CornerPointNum[j]+1]=array9 [0][p];
					SectionPosRecord[1][p-CornerPointNum[j]+1]=array9 [1][p];
				}
				sectionPosLength=CornerPointNum[j+1]-CornerPointNum[j]+1;
			/*SectionPosRecord�жϽ���������*/
			
			/*����ع鷽��k��b*/
			k=Return_LinearRegressionEquation_k (SectionPosRecord, sectionPosLength);
			b=Return_LinearRegressionEquation_b (SectionPosRecord, sectionPosLength);
			r=Return_LinearRegressionEquation_r (SectionPosRecord, sectionPosLength);
			
			//�Էֶν��нǵ���
			/*__*\
			*|__|*
			\*  */
			//����Լ��
			if(ReturnAbsolute(r)<0.93f)
			{
				distance=0;
				for (int i=sectionPosLength-1 ; i>=2 ; i--)
				{
					/**�ж���λ������루>150��**/
					if( i>=3 && i<=sectionPosLength-2 && (ReturnDistance(SectionPosRecord , i , k , b)-ReturnDistance(SectionPosRecord , i-1 , k , b))>=0 && (ReturnDistance(SectionPosRecord , i , k , b)-ReturnDistance(SectionPosRecord , i+1 , k , b))>=0)
					{						
						//��¼�÷ֶ���ȡ��һ�ǵ�λ�������λ�ã��ܣ�
						/*��distance��0��*/
						if(ReturnDistance(SectionPosRecord , i , k , b)>distance)
						{
							distance=ReturnDistance(SectionPosRecord , i , k , b);
							number=i;
							Number=CornerPointNum[j]+i-1;
							ifJudgeAngle=1;
						}
					}
				}
				if(ifJudgeAngle==1&&distance>=50)
				{
					ifJudgeAngle=0;
					for(int i=sectionPosLength ; i>=number ; i--)
					{
						For[0][i-number+1]=SectionPosRecord[0][i];
						For[1][i-number+1]=SectionPosRecord[1][i];
					}
					for(int i=number ; i>=1 ; i--)
					{
						Bac[0][i]=SectionPosRecord[0][i];
						Bac[1][i]=SectionPosRecord[1][i];
					}
					if (ReturnAbsolute(180*atan(Return_LinearRegressionEquation_k(For,sectionPosLength-number+1))/Pi-\
									   180*atan(Return_LinearRegressionEquation_k(Bac,number))/Pi)>=30) /******************************/
					{
						for(int i=0 ; i<=9 ; i++)
						{
							if(Number>=CornerPointNum[i]+2&&Number<=CornerPointNum[i+1]-2)
							{
								/*������ʼ��0*/
								
								if(CornerPointNum_FB[10]==111||CntCorner_FB<=9)
								{
									ifCornerIncrease=1;
									CornerPointNum_FB[10]=Number;
									CntCorner_FB++;
								}
							}
						}
					}
				}
			}
		}
		//������λ
        CntCorner=CntCorner_FB;
		for(int i=0 ; i<=10 ; i++)
        {
            CornerPointNum[i]=CornerPointNum_FB[i];
        }
	}
	/*�жϱ����Ƿ����½ǵ����*/
	while(ifCornerIncrease==1);
	
		/**********����**********/
		if(radarDebug==1)
		{
//				//USART_OUT(UART4,(uint8_t*)"%d",CntCorner);
//				//��������
//				for (int i=0 ; i<=CntCorner+1 ; i++)
//				{
//						USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s","C","X",":");
//						GetFloat (array9 [0][CornerPointNum [i]], 3);
//						USART_OUT(UART4,(uint8_t*)"    %s%s%s","C","Y",":");
//						GetFloat (array9 [1][CornerPointNum [i]], 3);
//				}
		}
		/**********����**********/

	float FOR [2][101];
	float BAC [2][101];
	int TrueCPNum [11];
	int TCP_number=0;
	
		for(int i=0;i<=10;i++)
		{
			TrueCPNum[i]=111;
		}
		//��¼��β����	
		TrueCPNum [0]=1;
		TrueCPNum [1]=num;
	    //�ж������ߡ�����߼��Ƿ����80-100�ȵĽǡ�ReturnAbsolute��
	if(CntCorner>=1)
    {
		for (int i=1 ; i<=CntCorner ;  i++)
		{
			for(int j=CornerPointNum [i-1] ; j<=CornerPointNum [i] ; j++)
			{
				FOR[0][j-CornerPointNum[i-1]+1]=array9[0][j];
				FOR[1][j-CornerPointNum[i-1]+1]=array9[1][j];
			}
			for(int j=CornerPointNum [i] ; j<=CornerPointNum [i+1] ; j++)
			{
				BAC[0][j-CornerPointNum[i]+1]=array9[0][j];
				BAC[1][j-CornerPointNum[i]+1]=array9[1][j];
			}
			if(ReturnAbsolute( ((atan(Return_LinearRegressionEquation_k(FOR,CornerPointNum [i]-CornerPointNum [i-1]+1)))*180/Pi)-((atan(Return_LinearRegressionEquation_k(BAC,CornerPointNum [i+1]-CornerPointNum [i]+1)))*180/Pi) )>=80&&\
			   ReturnAbsolute( ((atan(Return_LinearRegressionEquation_k(FOR,CornerPointNum [i]-CornerPointNum [i-1]+1)))*180/Pi)-((atan(Return_LinearRegressionEquation_k(BAC,CornerPointNum [i+1]-CornerPointNum [i]+1)))*180/Pi) )<=100)
			{
			    TCP_number++;
				TrueCPNum [10]=CornerPointNum [i];
				qsort(TrueCPNum,11,sizeof(TrueCPNum[0]),cmp_p);
			}
		}
	}
	
	
		/**********����**********/
		if(radarDebug==1)
		{
//				for(int i=0 ; i<=TCP_number+1 ; i++)
//				{
//					USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s","P","X",":");
//					GetFloat (array9 [0][TrueCPNum [i]], 3);
//					USART_OUT(UART4,(uint8_t*)"    %s%s%s","P","Y",":");
//					GetFloat (array9 [1][TrueCPNum [i]], 3);
//				}
		}
		/**********����**********/
		/**********����**********/
		if(radarDebug==1)
		{
				for(int i=1 ; i<=TCP_number ; i++)
				{
					USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s","C","X",":");
					GetFloat (array9 [0][TrueCPNum [i]], 3);
					USART_OUT(UART4,(uint8_t*)"    %s%s%s","C","Y",":");
					GetFloat (array9 [1][TrueCPNum [i]], 3);
				}
		}
		/**********����**********/
		
		
	//����TCPN��������
	for(int i=1 ; i<=TCP_number ; i++)
	{
		Corner_Num++;
		Corner[0][Corner_Num]=array9[0][TrueCPNum[i]];
		Corner[1][Corner_Num]=array9[1][TrueCPNum[i]];
	}
	for(int i=0 ; i<=TCP_number+1 ; i++)
	{
		Check_Num++;
		CheckPoint[0][Check_Num]=array9[0][TrueCPNum[i]];
		CheckPoint[1][Check_Num]=array9[1][TrueCPNum[i]];
	}
}

void CheckSquare()
{
	//���������
	if(CheckPermit==1)/*��־λ��Radar�ṩ��*/
	{
/////////////////////////////////////////////////////////////////////////////////////////	    
		/*�ǵ������μ��*/
        for(int i=1 ; i<=Corner_Num ; i++)
		{
			if(SquareLock==1)
			{break;}
			for (int j=1 ; j<=Corner_Num ; j++)
			{
				//�������4908+-
				if(Distance(Corner,i,Corner,j)>=4808&&Distance(Corner,i,Corner,j)<=5008)
				{
					//����CheckPoint����
					F_Num++;
					RangeF[0][F_Num]=Corner[0][j];
					RangeF[1][F_Num]=Corner[1][j];
				}
				//�������6941+-
				if(Distance(Corner,i,Corner,j)>=6841&&Distance(Corner,i,Corner,j)<=7041)
				{
					//����CheckPoint����
					S_Num++;
					RangeS[0][S_Num]=Corner[0][j];
					RangeS[1][S_Num]=Corner[1][j];
				}
			}
			//��RangeF��RangeS���������
			/*�����⵽�����Σ��ĸ�����ͬʱ���㣬�ĸ�����ͬʱ���㣬CheckPermit����*/
			//ѭ����RangeF���Ҵ�ֱ��
			for(int j=1 ; j<=F_Num ; j++)
			{
				if(SquareLock==1)
				{break;}
				for(int k=1 ; k<=F_Num ; k++)
				{
					if(SquareLock==1)
					{break;}
					//�ж������ഹֱ�ĵ�
					if(ReturnAbsolute(Return_Angle(Corner,i,RangeF,j)-Return_Angle(Corner,i,RangeF,k))>=85&&\
					   ReturnAbsolute(Return_Angle(Corner,i,RangeF,j)-Return_Angle(Corner,i,RangeF,k))<=95&&\
					   SquareLock==0)
					{
						//ѭ���ж�RangeS������Ϊ��ƽ���ߵĵ�
						for(int m=1 ; m<=S_Num ; m++)
						{
							if(ReturnAbsolute(Return_Angle(Corner,i,RangeS,m)-Return_Angle(Corner,i,RangeF,j))>=40&&\
							   ReturnAbsolute(Return_Angle(Corner,i,RangeS,m)-Return_Angle(Corner,i,RangeF,j))<=50&&\
							   ReturnAbsolute(Return_Angle(Corner,i,RangeS,m)-Return_Angle(Corner,i,RangeF,k))>=40&&\
							   ReturnAbsolute(Return_Angle(Corner,i,RangeS,m)-Return_Angle(Corner,i,RangeF,k))<=50&&\
                               SquareLock==0)
							{
								CheckPermit=0;
								SquareLock=1;
								
								SquarePos[0][0]=Corner[0][i];
								SquarePos[1][0]=Corner[1][i];
								SquarePos[0][1]=RangeF[0][j];
								SquarePos[1][1]=RangeF[1][j];
								SquarePos[0][2]=RangeS[0][m];
								SquarePos[1][2]=RangeS[1][m];
								SquarePos[0][3]=RangeF[0][k];
								SquarePos[1][3]=RangeF[1][k];
								
							}
							else if (SquareLock==0)//������ȷ�����һ���㣨�����飩
							{
								CheckPermit=0;
								SquareLock=1;
								
								SquarePos[0][0]=Corner[0][i];
								SquarePos[1][0]=Corner[1][i];
								SquarePos[0][1]=RangeF[0][j];
								SquarePos[1][1]=RangeF[1][j];
								SquarePos[0][2]=(RangeF[0][j]+RangeF[0][k])-Corner[0][i];
								SquarePos[1][2]=(RangeF[1][j]+RangeF[1][k])-Corner[1][i];
								SquarePos[0][3]=RangeF[0][k];
								SquarePos[1][3]=RangeF[1][k];
							}
						}
					}
				}
				for(int k=1 ; k<=S_Num ; k++)
				{
					if(SquareLock==1)
					{break;}
					//�ж����н�45�ȵĽ�
					if(ReturnAbsolute(Return_Angle(Corner,i,RangeF,j)-Return_Angle(Corner,i,RangeS,k))>=40&&\
					   ReturnAbsolute(Return_Angle(Corner,i,RangeF,j)-Return_Angle(Corner,i,RangeS,k))<=50&&\
					   SquareLock==0)
					{
								CheckPermit=0;
								SquareLock=1;
						
								SquarePos[0][0]=Corner[0][i];
								SquarePos[1][0]=Corner[1][i];
								SquarePos[0][1]=RangeF[0][j];
								SquarePos[1][1]=RangeF[1][j];
								SquarePos[0][2]=RangeS[0][k];
								SquarePos[1][2]=RangeS[1][k];
								SquarePos[0][3]=Corner[0][i]+RangeS[0][k]-RangeF[0][j];
								SquarePos[1][3]=Corner[1][i]+RangeS[1][k]-RangeF[1][j];
					}
				}
			}
			F_Num=0;
			S_Num=0;
			memset(RangeF,0,sizeof(RangeF));
			memset(RangeS,0,sizeof(RangeS));
		}
/////////////////////////////////////////////////////////////////////////////////////////
		for(int i=1 ; i<=Corner_Num ; i++)
		{
			if(SquareLock==1)
			{break;}
			for (int j=1 ; j<=Check_Num ; j++)
			{
				//�������4908+-
				if(Distance(Corner,i,CheckPoint,j)>=4808&&Distance(Corner,i,CheckPoint,j)<=5008)
				{
					//����CheckPoint����
					F_Num++;
					RangeF[0][F_Num]=CheckPoint[0][j];
					RangeF[1][F_Num]=CheckPoint[1][j];
				}
				//�������6941+-
				if(Distance(Corner,i,CheckPoint,j)>=6841&&Distance(Corner,i,CheckPoint,j)<=7041)
				{
					//����CheckPoint����
					S_Num++;
					RangeS[0][S_Num]=CheckPoint[0][j];
					RangeS[1][S_Num]=CheckPoint[1][j];
				}
			}
			//��RangeF��RangeS���������
			/*�����⵽�����Σ��ĸ�����ͬʱ���㣬�ĸ�����ͬʱ���㣬CheckPermit����*/
			//ѭ����RangeF���Ҵ�ֱ��
			for(int j=1 ; j<=F_Num ; j++)
			{
				if(SquareLock==1)
				{break;}
				for(int k=1 ; k<=F_Num ; k++)
				{
					if(SquareLock==1)
					{break;}
					//�ж������ഹֱ�ĵ�
					if(ReturnAbsolute(Return_Angle(Corner,i,RangeF,j)-Return_Angle(Corner,i,RangeF,k))>=85&&\
					   ReturnAbsolute(Return_Angle(Corner,i,RangeF,j)-Return_Angle(Corner,i,RangeF,k))<=95&&\
					   SquareLock==0)
					{
						//ѭ���ж�RangeS������Ϊ��ƽ���ߵĵ�
						for(int m=1 ; m<=S_Num ; m++)
						{
							if(ReturnAbsolute(Return_Angle(Corner,i,RangeS,m)-Return_Angle(Corner,i,RangeF,j))>=40&&\
							   ReturnAbsolute(Return_Angle(Corner,i,RangeS,m)-Return_Angle(Corner,i,RangeF,j))<=50&&\
							   ReturnAbsolute(Return_Angle(Corner,i,RangeS,m)-Return_Angle(Corner,i,RangeF,k))>=40&&\
							   ReturnAbsolute(Return_Angle(Corner,i,RangeS,m)-Return_Angle(Corner,i,RangeF,k))<=50  )
							{
								CheckPermit=0;
								SquareLock=1;
								
								SquarePos[0][0]=Corner[0][i];
								SquarePos[1][0]=Corner[1][i];
								SquarePos[0][1]=RangeF[0][j];
								SquarePos[1][1]=RangeF[1][j];
								SquarePos[0][2]=RangeF[0][k];
								SquarePos[1][2]=RangeF[1][k];
								SquarePos[0][3]=RangeS[0][m];
								SquarePos[1][3]=RangeS[1][m];
								
							}
							else if (SquareLock==0)//������ȷ�����һ���㣨�����飩
							{
								CheckPermit=0;
								SquareLock=1;
								
								SquarePos[0][0]=Corner[0][i];
								SquarePos[1][0]=Corner[1][i];
								SquarePos[0][1]=RangeF[0][j];
								SquarePos[1][1]=RangeF[1][j];
								SquarePos[0][2]=RangeF[0][k];
								SquarePos[1][2]=RangeF[1][k];
								SquarePos[0][3]=(RangeF[0][j]+RangeF[0][k])-Corner[0][i];
								SquarePos[1][3]=(RangeF[1][j]+RangeF[1][k])-Corner[1][i];
							}
						}
					}
				}
				for(int k=1 ; k<=S_Num ; k++)
				{
					if(SquareLock==1)
					{break;}
					//�ж����н�45�ȵĽ�
					if(ReturnAbsolute(Return_Angle(Corner,i,RangeF,j)-Return_Angle(Corner,i,RangeS,k))>=40&&\
					   ReturnAbsolute(Return_Angle(Corner,i,RangeF,j)-Return_Angle(Corner,i,RangeS,k))<=50&&
					   SquareLock==0)
					{
								CheckPermit=0;
								SquareLock=1;
						
								SquarePos[0][0]=Corner[0][i];
								SquarePos[1][0]=Corner[1][i];
								SquarePos[0][1]=RangeF[0][j];
								SquarePos[1][1]=RangeF[1][j];
								SquarePos[0][2]=RangeS[0][k];
								SquarePos[1][2]=RangeS[1][k];
								SquarePos[0][3]=Corner[0][i]+RangeS[0][k]-RangeF[0][j];
								SquarePos[1][3]=Corner[1][i]+RangeS[1][k]-RangeF[1][j];
					}
				}
			}
			F_Num=0;
			S_Num=0;
			memset(RangeF,0,sizeof(RangeF));
			memset(RangeS,0,sizeof(RangeS));
		}
		if(SquareLock==1)
		{
			Corner_Num=0;
			Check_Num=0;
			memset( Corner     ,0, sizeof(Corner)     );
			memset( CheckPoint ,0, sizeof(CheckPoint) );
			//Corner��CheckPoint����
			CheckPermit=0;
		
			/**********����**********/
			if(radarDebug==1)
			{
//					for(int i=0 ; i<=3 ; i++)
//					{
//						USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s","A","X",":");
//						GetFloat (SquarePos[0][i], 3);
//						USART_OUT(UART4,(uint8_t*)"    %s%s%s","A","Y",":");
//						GetFloat (SquarePos[1][i], 3);
//					}
			}
			/**********����**********/
		
		}
	}
}

void SecondOrder_Filter(void)//�˲�����
{
	if(ifAFirstOFOK==1)
	{
			if(sqrt((APositionRecord[0][2]-APositionRecord[0][1])*(APositionRecord[0][2]-APositionRecord[0][1])+(APositionRecord[1][2]-APositionRecord[1][1])*(APositionRecord[1][2]-APositionRecord[1][1]))<=150||\
			   sqrt((APositionRecord[0][3]-APositionRecord[0][2])*(APositionRecord[0][3]-APositionRecord[0][2])+(APositionRecord[1][3]-APositionRecord[1][2])*(APositionRecord[1][3]-APositionRecord[1][2]))<=150)
			{
					cntNumA++;
					ATargetPositionRecord[0][0]=APositionRecord[0][2];
					ATargetPositionRecord[1][0]=APositionRecord[1][2];
					for(int ai=0 ; ai<=1 ; ai++)
					{
						for(int aj=100; aj>=1 ; aj--)
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
							//�ǵ�/ֱ�Ǽ��
							{
								CheckCorner  (ATargetPositionRecord , cntNumA);
							}
							
							/**********����**********/
							if(radarDebug==1)
							{
									for(int i=cntNumA;i>=1;i--)
									{
										USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s","A","X",":");
										GetFloat (ATargetPositionRecord[0][i], 3);
										USART_OUT(UART4,(uint8_t*)"%s%s%s","A","Y",":");
										GetFloat (ATargetPositionRecord[1][i], 3);
									}
							}
							/**********����**********/

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
			if(sqrt((BPositionRecord[0][2]-BPositionRecord[0][1])*(BPositionRecord[0][2]-BPositionRecord[0][1])+(BPositionRecord[1][2]-BPositionRecord[1][1])*(BPositionRecord[1][2]-BPositionRecord[1][1]))<=150||\
			   sqrt((BPositionRecord[0][3]-BPositionRecord[0][2])*(BPositionRecord[0][3]-BPositionRecord[0][2])+(BPositionRecord[1][3]-BPositionRecord[1][2])*(BPositionRecord[1][3]-BPositionRecord[1][2]))<=150)
			{
					cntNumB++;
					BTargetPositionRecord[0][0]=BPositionRecord[0][2];
					BTargetPositionRecord[1][0]=BPositionRecord[1][2];
					for(int bi=0 ; bi<=1 ; bi++)
					{
						for(int bj=100; bj>=1 ; bj--)
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
								CheckCorner  (BTargetPositionRecord , cntNumB);
							}
							
							/**********����**********/
							if(radarDebug==1)
							{
									for(int i=cntNumB;i>=1;i--)
									{
										USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s","B","X",":");
										GetFloat (BTargetPositionRecord[0][i], 3);
										USART_OUT(UART4,(uint8_t*)"%s%s%s","B","Y",":");
										GetFloat (BTargetPositionRecord[1][i], 3);
									}
							}
							/**********����**********/
							
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
		if(t_post<=240.f){post_fortAngle+=0.5f;}
		if(t_post>240.f) {post_fortAngle=0.f;}
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

int cntScanTime=0;
int pointNum=0;
void RadarCorrection(PID_Value *pos)//�״�У��ϵͳ
{
	if(SquareLock==0)
	{
		DataProcessing (pos);
		//������������ɨ��ָ��
		Laser_data();
		setangle+=0.5f;
		if(setangle>360){setangle-=360;CheckPermit=1;}
		Set_FortAngle1=SetToFort_AngleProcessing(pos->Angle,r_fortAngle0,fort.yawPosReceive,setangle);//
//				USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","S","A","n","g",":");
//				GetFloat (Set_FortAngle1, 3);
		//����Ƕ�
//		r_fortAngle1=Return_SymmetricRangeValue((pos->Angle+Return_SymmetricRangeValue(-Return_NonSymmetricRangeValue(fort.yawPosReceive,360,0),180)),180);
		r_fortAngle1=FortToGround_AngleProcessing(pos->Angle,r_fortAngle0);/**///��̨�ԵؽǶȣ�������ϵ��
		
			/**********����**********/
			if(radarDebug==1)
			{
				//���ؽǶ�
//				USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","P","A","n","g",":");
//				GetFloat (pos->Angle, 3);
//				USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","F","A","n","g",":");
//				GetFloat (r_fortAngle1, 3);
			}
			/**********����**********/

			
		//��������
		r_standardFortAngle=Return_SymmetricRangeValue((r_fortAngle1+90.f),180.f);//��̨�ԵؽǶȣ���׼����ϵ��
		A_targetx=Avalue*(cos(Pi*r_standardFortAngle/180.f))+A_Laser_X;/////////|����������Ϊ����������
		A_targety=Avalue*(sin(Pi*r_standardFortAngle/180.f))+A_Laser_Y-2400.f;////|����������Ϊ����������
		B_targetx=Bvalue*(cos(Pi*r_standardFortAngle/180.f))+B_Laser_X;/////////|����������Ϊ����������
		B_targety=Bvalue*(sin(Pi*r_standardFortAngle/180.f))+B_Laser_Y-2400.f;////|����������Ϊ����������
		
			/**********����**********/
			if(radarDebug==1)
			{
				//��������
//				USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","A","T","a","r","X",":");
//				GetFloat (A_targetx, 3);
//				USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","A","T","a","r","Y",":");
//				GetFloat (A_targety, 3);
//				USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","B","T","a","r","X",":");
//				GetFloat (B_targetx, 3);
//				USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%s","B","T","a","r","Y",":");
//				GetFloat (B_targety, 3);
			}
			/**********����**********/
			
				
		//���ݷ���
		{
		FirstOrder_Filter(pos);
		SecondOrder_Filter();
		CheckSquare();
		}
	}
	if(SquareLock==1)
	{	
		cntScanTime++;
		pointNum=(int)((cntScanTime%1600)/400);
		
		/*��������SquarePos����*/
		
		Set_FortAngle1=TargetToFort_AngleProcessing(Fort_X,Fort_Y,SquarePos[0][pointNum],SquarePos[1][pointNum]+2400.f,pos->Angle,r_fortAngle0,fort.yawPosReceive);
		
			/**********����**********/
			if(radarDebug==1)
			{
				USART_OUT(UART4,(uint8_t*)"\r\n%s%s%s%s%s%s","S","Q","U","R","E",":");
				GetFloat (SquarePos[0][0], 3);
				GetFloat (SquarePos[1][0], 3);
				GetFloat (SquarePos[0][1], 3);
				GetFloat (SquarePos[1][1], 3);
				GetFloat (SquarePos[0][2], 3);
				GetFloat (SquarePos[1][2], 3);
				GetFloat (SquarePos[0][3], 3);
				GetFloat (SquarePos[1][3], 3);
				USART_OUT(UART4,(uint8_t*)"\r\n");
				GetFloat (pointNum, 0);
				GetFloat (Fort_X       , 3);
				GetFloat (Fort_Y-2400.f, 3);
				GetFloat (SquarePos[0][pointNum], 3);
				GetFloat (SquarePos[1][pointNum], 3);
				USART_OUT(UART4,(uint8_t*)"\r\n");
				GetFloat (pos->Angle, 3);
				GetFloat (r_fortAngle0, 3);
				GetFloat (fort.yawPosReceive, 3);
				GetFloat (Set_FortAngle1, 3);
			}
			/**********����**********/
		if(cntScanTime>=1600)
		{
			memset(SquarePos,0,sizeof(SquarePos));
			cntScanTime=0;
			pointNum=0;
			SquareLock=0;
			setangle=0;
		}
	}
}

/*
void CornerLock()
{
	
}

int possibility=0;
void Check_Target(PID_Value *pos)
{
	DataProcessing (pos);
	Laser_data();	
	setangle+=0.5;
	if(setangle>360){setangle-=360;}
	Set_FortAngle1=SetToFort_AngleProcessing(pos->Angle,r_fortAngle0,fort.yawPosReceive,setangle);
}
*/


//====================================================================================
//                                       ������
//====================================================================================
//��5��6
float Lef_v[51];				//��������ʶ�ȡ
float FLef_v[51];				//��������ʶ�ȡ
float L_Value;  				//�������ֵ
int cntL=0;						//L���ʼ�¼
int LIntegration=0;				//L���ּ���
int cnt_L_Cliff=0;				//L�ϵ����

float Rig_v[51];				//�ҹ������ʶ�ȡ
float FRig_v[51];				//�ҹ������ʶ�ȡ
float R_Value;					//�ҹ�����ֵ
int cntR=0;						//R���ʼ�¼
int RIntegration=0;				//R���ּ���
int cnt_R_Cliff=0;				//R�ϵ����

//int Golf_num;                    
int Golf_Num=0;					//����Golf������
int Entrepot[2]={0,0};			//����ֿ�Golf��������[0]��[1]�ң�

int RollerStuck[2]={0,0};		//�����־λ
int cnt_L_Stuck=0;				//L�����ʱ && L���ϴ����ʱ
int cnt_R_Stuck=0;				//R�����ʱ && R���ϴ����ʱ
int RollerBored[2]={0,0};		//��ȱ��־λ
int cnt_L_Leisure;				//L��ȱ��ʱ
int cnt_R_Leisure;				//R��ȱ��ʱ
int Bored=0;					//��ȱ��־λ
int RollerParalyze[2]={0,0};	//̱����־λ
int cnt_L_Paralyze=0;
int cnt_R_Paralyze=0;
int XiMaTa;

void CntGolf(void)
{
	if(RollerStuck[0]==0)	{VelCrl(CAN2, 5,   SET_ROLLER_SPEED * 32768);}
	if(RollerStuck[1]==0)	{VelCrl(CAN2, 6, 0-SET_ROLLER_SPEED * 32768);}
	
	//���ݼ�¼
	cntL++;
	Lef_v[0] = 0.f+GetMotor5Speed()/32768;
	cntR++;
	Rig_v[0] = 0.f-GetMotor6Speed()/32768;
	
	//����500ms����ת��
	for(int i=50 ; i>=1 ; i--)
	{
		Lef_v[i]=Lef_v[i-1];
		Rig_v[i]=Rig_v[i-1];
	}
	for(int i=50 ; i>=0 ; i--)
	{
		FLef_v[i]=Lef_v[i];
		FRig_v[i]=Rig_v[i];
	}
	
	//һά�˲�(�����˲�)
	if(FLef_v[2]-FLef_v[1]>=0 && FLef_v[2]-FLef_v[3]>=0){FLef_v[2]=(FLef_v[3]+FLef_v[1])/2;}
	if(FRig_v[2]-FRig_v[1]>=0 && FRig_v[2]-FRig_v[3]>=0){FRig_v[2]=(FRig_v[3]+FRig_v[1])/2;}
	
	
	//���ϴ���
	if(RollerStuck[0]==1)	
	{
		cnt_L_Stuck++ ; 
		if(cnt_L_Stuck<100)
		{
			MotorOff(CAN2,5);
		}			
		else if(cnt_L_Stuck==100)
		{
			MotorOn(CAN2,5);
		}
		else if(cnt_L_Stuck>100&&cnt_L_Stuck<=350)
		{
			VelCrl(CAN2, 5, 0-SET_ROLLER_SPEED * 32768);
		}
		else
		{
			VelCrl(CAN2, 5,   SET_ROLLER_SPEED * 32768);
			if(Lef_v[0]>=SET_ROLLER_SPEED-10)
			{
			cnt_L_Stuck=0;
			RollerStuck[0]=0;
			}
			else{/*̱���ж�*/}
		}
	}
	if(RollerStuck[1]==1)	
	{
		cnt_R_Stuck++ ;
		if(cnt_R_Stuck<100)
		{
			MotorOff(CAN2,6);
		}
		else if(cnt_R_Stuck==100)
		{
			MotorOn(CAN2,6);
		}
		else if(cnt_R_Stuck>100&&cnt_R_Stuck<=350)
		{
			VelCrl(CAN2, 6,   SET_ROLLER_SPEED * 32768);
		}
		else
		{
			VelCrl(CAN2, 6, 0-SET_ROLLER_SPEED * 32768);
			if(Rig_v[0]>=SET_ROLLER_SPEED-10)
			{
			cnt_R_Stuck=0;
			RollerStuck[1]=0;
			}
			else{/*̱���ж�*/}
		}
	}
	if(RollerStuck[0]==1&&RollerStuck[1]==1)
	{
		USART_OUT(UART4,(uint8_t*)"\r\n%s","! ! ! XiMaTa ! ! !");
	}
	
	/**********����**********/
	if(rollerDebug==1)
	{
		USART_OUT(UART4,(uint8_t*)"%s","L:");
		GetFloat (Lef_v[2] , 0);
		USART_OUT(UART4,(uint8_t*)"%s","R:");
		GetFloat (Rig_v[2] , 0);
		USART_OUT(UART4,(uint8_t*)"%s","FL:");
		GetFloat (FLef_v[2] , 0);
		USART_OUT(UART4,(uint8_t*)"%s","FR:");
		GetFloat (FRig_v[2] , 0);
	}
	/**********����**********/
	
	/**********����**********/
	if(rollerDebug==1)
	{
		USART_OUT(UART4,(uint8_t*)"%s","LEFT:");
		GetFloat (Entrepot[0] , 0);
		USART_OUT(UART4,(uint8_t*)"%s","RIGHT:");
		GetFloat (Entrepot[1] , 0);
	}
	/**********����**********/
	
	/*���ݴ���*/
	//�����ݽӽ��趨ֵ��ϵ�
	//������[2]Ϊ��ʼλ��[cntL]Ϊ��ֹλ,ʵ�ʳ���Ϊ(cntL-1)
	
	
	
	/*LEFT*/
	if(FLef_v[0]>=(SET_ROLLER_SPEED - LEFT_ROLLER_THRESHOLD))
	{
		//����ʱ������δ���򡪡�������Bored��־λ��
		cnt_L_Leisure+=cntL;
		//�յ��ж�
		//������鳤��С�ڡ���ֵ�����ж�Ϊ����δ����
		if(cntL-1<3/*��������*/)
		{
			if(cnt_L_Leisure>=1500/*����ʱ��*/){RollerBored[0]=1;}
		}
		
		//������鳤�ȴ��ڡ���ֵ��
		if(cntL-1>=3/*��������*/)
		{
			//����
			for(int i=cntL ; i>=2 ; i--)
			{
				LIntegration+=(SET_ROLLER_SPEED-FLef_v[i]);
			}
//			//�½��ϵ㡾ÿ����һ����ֵ���ϵ���+1��
//			//ƽ����/��ֵ�жϣ��ϵ㣩
//			//�������ж�
//			//�����ࡿ
//			for(int i=cntL ; i>=4 ; i--)
//			{
//				if(Lef_v[i]-Lef_v[i-1]>=5/*�����ϵ���ֵ*/ || Lef_v[i]-Lef_v[i-2]>=5/*�����ϵ���ֵ*/)	{}		/*�ϵ��ж�*/
//				
//				if(Lef_v[i]-Lef_v[i-1]>=0)	{cntDecline++;}			/*�½��жϣ��½���־λ++��������־λ=0����¼�յ㣩*/
//				
//				if(Lef_v[i]-Lef_v[i-1]>=0)	{cntRise++;}			/*�����жϣ�������־λ++���½���־λ=0����¼�յ㣩*/
//				
//				for()												/*ƽ�����ж�*/
//				{
//					
//				}
//			}
			//�����жϡ���������ͳ�ơ�++
			if((ReturnArrayMin(cntL,FLef_v))<=65||(cntL-1)>=3)
			{
				RollerBored[0]=0;
				cnt_L_Leisure=0;
				Entrepot[0]++;
				if((ReturnArrayMin(cntL,FLef_v))<=56/*54*/&&(cntL-1)>=8/*9*/&&LIntegration>=111)
				{
					Entrepot[0]++;
					if((ReturnArrayMin(cntL,FLef_v))<=43&&(cntL-1)>=11)
					{
						Entrepot[0]++;
						if((ReturnArrayMin(cntL,FLef_v))<=38&&(cntL-1)>=14&&LIntegration>=300)
						{
							Entrepot[0]++;
						}
					}
				}
			}
			
			/**********����**********/
			if(rollerDebug==1)
			{
				USART_OUT(UART4,(uint8_t*)"%s","LInt��");
				GetFloat ((LIntegration) , 0);
				USART_OUT(UART4,(uint8_t*)"%s","LNUM:");
				GetFloat ((cntL-1) , 0);
				USART_OUT(UART4,(uint8_t*)"%s","LMIN:");
				GetFloat ((ReturnArrayMin(cntL,FLef_v)) , 0);
			}
			/**********����**********/
			
		/*IF�������� {RollerBored[0]��0��cnt_L_Leisure��0}*/
				
			//�����жϣ�1.ת�ٷ�Χ/2.��ֵ�ж�/3.�������жϣ�������������ʱ��
			{
			//��������������תʱ������+[2+]����ת���������+[0]�������������ӳ١�
			//��תʱ���������½��жϡ�����������־λ��
			//�ж�̱���������������������������������ٶȸ���Ϊ��ת
			}
			//�������жϣ�ת���˲�������������ʵ�顿
			
		}
		//��0
		cntL=0;
		LIntegration=0;
	}
	
	
	
	/*RIGHT*/
	if(FRig_v[0]>=(SET_ROLLER_SPEED - RIGHT_ROLLER_THRESHOLD))
	{
		//����ʱ������δ���򡪡�������Bored��־λ��
		cnt_R_Leisure+=cntR;
		//�յ��ж�
		//������鳤��С�ڡ���ֵ�����ж�Ϊ����δ����
		if(cntR-1<3/*��������*/)
		{
			if(cnt_R_Leisure>=1500/*����ʱ��*/){RollerBored[1]=1;}
		}
		
		//������鳤�ȴ��ڡ���ֵ��
		if(cntR-1>=3/*��������*/)
		{
			//����
			for(int i=cntR ; i>=2 ; i--)
			{
				RIntegration+=(SET_ROLLER_SPEED-FRig_v[i]);
			}
			
			//�����жϡ���������ͳ�ơ�++
			if((ReturnArrayMin(cntR,FRig_v))<=64&&(cntR-1)>=3)
			{
				RollerBored[1]=0;
				cnt_R_Leisure=0;
				Entrepot[1]++;
				if((ReturnArrayMin(cntR,FRig_v))<=59/*56*/&&(cntR-1)>=10/*12*/&&RIntegration>=111)
				{
					Entrepot[1]++;
					if((ReturnArrayMin(cntR,FRig_v))<=53&&(cntR-1)>=18)
					{
						Entrepot[1]++;
						if((ReturnArrayMin(cntR,FRig_v))<=40&&cntR-1>=23&&cnt_R_Leisure>=404)
						{
							Entrepot[1]++;
						}
					}
				}
			}
			
			/**********����**********/
			if(rollerDebug==1)
			{
				USART_OUT(UART4,(uint8_t*)"%s","RInt��");
				GetFloat ((RIntegration) , 0);
				USART_OUT(UART4,(uint8_t*)"%s","RNUM:");
				GetFloat ((cntR-1) , 0);
				USART_OUT(UART4,(uint8_t*)"%s","RMIN:");
				GetFloat ((ReturnArrayMin(cntR,FRig_v)) , 0);
			}
			/**********����**********/
			
		/*IF�������� {RollerBored[0]��0��cnt_L_Leisure��0}*/
		
			
			//�����жϣ�1.ת�ٷ�Χ/2.��ֵ�ж�/3.�������жϣ�������������ʱ��
			{
			//��������������תʱ������+[2+]����ת���������+[0]�������������ӳ١�
			//��תʱ���������½��жϡ�����������־λ��
			//�ж�̱���������������������������������ٶȸ���Ϊ��ת
			}
			//�������жϣ�ת���˲�������������ʵ�顿
		}
		//��0
		cntR=0;
		RIntegration=0;
	}
	//�����ж�Stuck
	if(RollerStuck[0]==0)
	{
		if(FLef_v[0]<=15)
		{
			cnt_L_Stuck++;
			if(cnt_L_Stuck>=200)
			{
				RollerStuck[0]=1;
				cnt_L_Stuck=0;
			}
		}
		else if (FLef_v[0]>=(SET_ROLLER_SPEED-10)){cnt_L_Stuck=0;}
	}
	if(RollerStuck[1]==0)
	{
		if(FRig_v[0]<=15)
		{
			cnt_R_Stuck++;
			if(cnt_R_Stuck>=200)
			{
				RollerStuck[1]=1;
				cnt_R_Stuck=0;
			}
		}
		else if (FRig_v[0]>=(SET_ROLLER_SPEED-10)){cnt_R_Stuck=0;}
	}
}

//====================================================================================
//                                     ������
//====================================================================================
extern PID_Value PID_A;
float shooterVel_v[11];		//������ת��Record
int ifPushFlag;				//�����־λ
int ifPushFlag_remain=0;	//�����־λ�ӳ�
int t_ifpushflag=0;			//�����־λ�ӳټ���
int ifShootFlag=0;			//�����־λ

void Remain_ifPushFlag(void)
{
	ifPushFlag=PID_A.fire_command;
	
	if(ifPushFlag==1){ifPushFlag_remain=1;}
	if(ifPushFlag_remain==1)
	{
		t_ifpushflag++;
		if(t_ifpushflag<=35){ifPushFlag=1;}
		if(t_ifpushflag>35 ){ifPushFlag_remain=0;t_ifpushflag=0;}
	}
}

void Filter(void)			//�˲�����
{}
void Data_Processing(void)	//���ݴ���
{
	//�˲�
	//����
	//΢��
}

//�������
int Bucket_ShootNum[4];
//���д���
int Bucket_HitNum[4];

void Shoot_Judge(void)//������
{
	shooterVel_v[0]=Gundata.ShooterVelRec;
	for(int i=10 ; i>=1 ; i--)
	{
		shooterVel_v[i]=shooterVel_v[i-1];
	}
	
	Remain_ifPushFlag();
	
	if(ifPushFlag==1)
	{
		if(shooterVel_v[10]-shooterVel_v[9]>=6||shooterVel_v[10]-shooterVel_v[8]>=6)//Ͷ�������½�
		{
			if(shooterVel_v[10]-shooterVel_v[0]<=6){ifShootFlag=1;}
		}
	}
	
	if(ifShootFlag==1)
	{
		ifPushFlag_remain=0;
		t_ifpushflag=0;
		Bucket_ShootNum[Gundata.BucketNum]++;
		ifShootFlag=0;
	}
}






void SendLogo(void)
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
