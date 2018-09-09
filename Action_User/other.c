#include "other.h"
extern GunneryData gundata;
//====================================================================================
//                                     ���ݴ���
//====================================================================================
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
float Return_SymmetricRangeValue(float Num8,float Range1)//����
{
	do
	{
	if((Num8)>(Range1))  {Num8-=(2*Range1);}
	if((Num8)<(-Range1)) {Num8+=(2*Range1);}
	}
	while( (Num8)<(-Range1) || (Num8)>(Range1) );
	return Num8;
}
float Return_NonSymmetricRangeValue(float Num9,float upper_bound,float lower_bound)
{
	do
	{
		if((Num9)>(upper_bound)){Num9-=(upper_bound-lower_bound);}
		if((Num9)<(lower_bound)){Num9+=(upper_bound-lower_bound);}
	}
	while( (Num9)>(upper_bound) || (Num9)<(lower_bound) );
	return Num9;
}
void GetFloat (float Num10, int places)
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
//====================================================================================
//                                     ���ⴥ��
//====================================================================================
float Avalue;
float Bvalue;
void Laser_data(void)
{
	Avalue=2.5114*fort.laserAValueReceive+51.623;	
	Bvalue=2.4269*fort.laserBValueReceive+371.39;
	/*\
	\*/
}

void Laser_Trigger(void)
{
	//���㼤�����(mm)
	Laser_data();
}
//====================================================================================
//                                   ��Ԫ����
//====================================================================================

void Sweep (void)//����++(���뷽�������α�־λ&������Բ�α�־λ&����)
{

}

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
{

}

void Collision_Process(void)//��ײ����++
{

}

//====================================================================================
//                          ����ɨ�裨Ŀ���⣩��ȫ��λ��
//====================================================================================
extern FortType fort;
int t_Breakdown=0;          //��λϵͳ�쳣����
int ifBreakdownFlag=0;      //��λϵͳ�쳣Flag
float d_angle=0;            //�Ƕ�����
float Set_FortInitialAngle1;//�趨��ʼ�Ƕȣ�δ������//�Ե�
float Set_FortAngle1;       //�趨����ɨ�裩�Ƕȣ�δ������//�Ե�
float FortAngle1;           //�����̨��ǰ�Ƕȣ�δ������//�Ե�
int t_laserlock=0;          //������������
int laserLock=0;            //����ɨ���ж�
float target_X;             //����Ŀ��X����
float target_Y;             //����Ŀ��Y����
float laserValue;           //���ⷵ��ֵ������Ŀ�����꣩
int TellFortToShoot=0;      //�����־λ���ж�shoot()��

void Target_Lock(GunneryData *Gun,PID_Value *pos)
{
	//���㼤�����(mm)
	Laser_data();
	//����нǼ���
	if(GetWZ()>=250) {t_Breakdown++;}
	if(GetWZ()<250)  {t_Breakdown=0;}
	if(t_Breakdown>=3){ifBreakdownFlag=1;}
	if(!ifBreakdownFlag)//��λϵͳ����
	{
		if(gundata.BucketNum==0){}
		if(gundata.BucketNum==1){}
		if(gundata.BucketNum==2){}
		if(gundata.BucketNum==3){}
	}
	if(ifBreakdownFlag)//��λϵͳ�쳣
	{
		//δ����Ŀ�꣨ɨ���߼���
		if(!laserLock)
		{
			d_angle++;
			switch(Gun->BucketNum)
			{
				case 0:
					if(pos->direction == ACW)Set_FortInitialAngle1=-180;
					if(pos->direction == CW)Set_FortInitialAngle1=-90;
					break;
				
				case 1:
					if(pos->direction == ACW)Set_FortInitialAngle1=-90;
					if(pos->direction == CW)Set_FortInitialAngle1=0;
					break;
				
				case 2:
					if(pos->direction == ACW)Set_FortInitialAngle1=0;
					if(pos->direction == CW)Set_FortInitialAngle1=90;
					break;
				
				case 3:
					if(pos->direction == ACW)Set_FortInitialAngle1=90;
					if(pos->direction == CW)Set_FortInitialAngle1=180;
					break;
			}
			if(GetWZ()<100)//���ٶ�С��100
			{
				if(pos->direction == ACW)
				{
					if(d_angle<90)   {d_angle++;}
					if(d_angle==90)  {d_angle=0;}
				}
				if(pos->direction == CW)
				{
					if(d_angle>-90)  {d_angle--;}
					if(d_angle==-90) {d_angle=0;}
				}
			}
//			if(GetWZ()>100)//���ٶȴ���100
//			{
//				
//			}
				Set_FortAngle1=Set_FortInitialAngle1+d_angle;
				Set_FortAngle1=Return_SymmetricRangeValue(Set_FortAngle1,180.f);
				
				//���㺽�����Գ��Ƕ�
				/*****************/
				//�趨�������Ƕ�
				/*****************\
				 *
				 *
				 *
				 *
				\*****************/
			
				//���Ŀ�꣨�ж��߼���
			{
				if(ReturnJudgeResult(Avalue,4800,1000)&&ReturnJudgeResult(Bvalue,4800,1000))
				{
					if(ReturnAbsolute(Avalue-Bvalue)<75){t_laserlock++;}
					else t_laserlock=0;
					if(t_laserlock==3)//��¼��ǰ��̨�Ƕȡ����Ƕȡ����������
					{
						FortAngle1=Return_SymmetricRangeValue(((-Return_SymmetricRangeValue(fort.yawPosReceive,180.f))+(pos->Angle)),180);
						laserValue=(Avalue+Bvalue)/2;
					}
					if(t_laserlock>=6){t_laserlock=0;laserLock=1;}
				}
			}
		}
		//������Ŀ�꣨��ȡ��Ϣ��
		if(laserLock)
		{
			//����Ŀ������
			target_X=Gun->Fort_X;
			target_Y=Gun->Fort_Y;
			
			//Ͷ���laserlock��0
		}
	}
}

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
	shooterVel.now=gundata.ShooterVelRec;
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

void Filter(void)//�˲�����
{}
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
		if(gundata.BucketNum==0){Bucket_0_ShootNum++;}
		if(gundata.BucketNum==1){Bucket_1_ShootNum++;}	
		if(gundata.BucketNum==2){Bucket_2_ShootNum++;}
		if(gundata.BucketNum==3){Bucket_3_ShootNum++;}	
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
extern FortType fort;

float laserAvaluereceive;
float laserBvaluereceive; 
float Xvaluereceive;
float Yvaluereceive; 
/*
void GetLaserData (void)
{
	laserAvaluereceive=fort.laserAValueReceive;
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%d%s","A","a","d","c",":",(int)laserAvaluereceive,".");
	while(laserAvaluereceive-1>=0)
	{laserAvaluereceive--;}
	laserAvaluereceive*=10000;
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)laserAvaluereceive);	
	
	laserBvaluereceive=fort.laserBValueReceive;
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%d%s","B","a","d","c",":",(int)laserBvaluereceive,".");
	while(laserBvaluereceive-1>=0)
	{laserBvaluereceive--;}
	laserBvaluereceive*=10000;
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)laserBvaluereceive);
}

void GetPositionValue(PID_Value *pid_out)//�����������
{
	Xvaluereceive=pid_out->X;
	Yvaluereceive=pid_out->Y; 
	
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%d%s","p","o","s","X",":",(int)Xvaluereceive,".");
	while(Xvaluereceive-1>=0)
	{Xvaluereceive--;}
	Xvaluereceive*=10000;
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)Xvaluereceive);
	
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s%d%s","p","o","s","Y",":",(int)Yvaluereceive,".");
	while(Yvaluereceive-1>=0)
	{Yvaluereceive--;}
	Yvaluereceive*=10000;
	USART_OUT(UART4,(uint8_t*)"%d  ",(int)Yvaluereceive);
}
*/

void GetLaserData2 (void)
{
	laserAvaluereceive=fort.laserAValueReceive;
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","A","a","d","c",":");	
	GetFloat (laserAvaluereceive, 3);
	
	laserBvaluereceive=fort.laserBValueReceive;
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","B","a","d","c",":");
	GetFloat (laserBvaluereceive, 3);
}

void GetPositionValue2(PID_Value *pid_out)//�����������
{
	Xvaluereceive=pid_out->X;
	Yvaluereceive=pid_out->Y; 
	
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","p","o","s","X",":");
	GetFloat (Xvaluereceive, 3);
	
	USART_OUT(UART4,(uint8_t*)"%s%s%s%s%s","p","o","s","Y",":");
	GetFloat (Yvaluereceive, 3);
}
