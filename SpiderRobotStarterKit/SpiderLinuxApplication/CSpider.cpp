/*
 * CSpider.cpp
 *
 *  Created on: 2014/7/26
 *      Author: Richard
 */

#include "terasic_os.h"
#include <stdint.h>
#include <stdio.h>
#include "CSpider.h"
//#include <Math.h>


#define HipF_Base 	   -20
#define HipM_Base  		0
#define HipB_Base  		20
#define Knee_Up_Base    60
#define Knee_Down_Base  45
#define Ankle_Base  	45

#define Dance_Angle 	20
#define Body_Up_Base   -45
#define Body_Down_Base  45

#define HipF_Fold 	  -60
#define HipM_Fold  		0
#define HipB_Fold  		60
#define Knee_Fold  		80
#define Ankle_Fold  	90

#define Knee_Extend  		-10
#define Ankle_Extend  	-60

#define KneeF_Full_Grab  		-80
#define KneeM_Full_Grab  		-65
#define KneeB_Full_Grab  		-80
#define Ankle_Full_Grab  	  -15



bool CSpider::m_bDebugDump = false;


CSpider::CSpider():
	m_bAbort(false)
{
	// TODO Auto-generated constructor stub
	int i;

	int szMotorID[] = {
	 /* LEG_RF */ 0,  1,  2,
	 /* LEG_RM */ 3,  4,  5,
	 /* LEG_RB */ 6,  7,  8,
	 /* LEG_LF */ 9,  10, 11,
	 /* LEG_LM */ 12, 13, 14,
	 /* LEG_LB */ 15, 16, 17
	};

	for(i=0;i<LEG_NUM;i++){
		m_szLeg[i] = new CSpiderLeg(szMotorID[i*3], szMotorID[i*3+1], szMotorID[i*3+2]);
	}
}

CSpider::~CSpider()
{
	// TODO Auto-generated destructor stub
	int i;
	for(i=0;i<LEG_NUM;i++){
		delete m_szLeg[i];
	}
}

void CSpider::Sleep(void)
{
	int i;
		for(i=0;i<LEG_NUM;i++){
		   m_szLeg[i]->Sleep();
		} // for i
}

void CSpider::WakeUp(void)
{
	int i;
	uint32_t Start_Time;
		for(i=0;i<LEG_NUM;i++){
		   m_szLeg[i]->WakeUp();
		   Start_Time = OS_GetTickCount()+100;
		   while(OS_GetTickCount() < Start_Time){}
		} // for i
}

bool CSpider::Init(void)
{
	bool bSuccess;
	int i,j,nJointNum;
	CSpiderLeg::JOINT_ID szJoint[] = {CSpiderLeg::Hip, CSpiderLeg::Knee, CSpiderLeg::Ankle};
  uint32_t Timeout,Start_Time;
	if (m_bDebugDump)
		printf("[Spider]Init\r\n");
	////////////////////////
	//// Init -- The servo angle need enter 0.0 to enable.
	nJointNum = sizeof(szJoint)/sizeof(szJoint[0]);
	for(j=0;j<nJointNum;j++){
		for(i=0;i<LEG_NUM;i++){
		   m_szLeg[i]->MoveJoint(szJoint[j], 0.0);
		   WaitReady(ReadyTime());
		   Start_Time = OS_GetTickCount()+100;
		   while(OS_GetTickCount() < Start_Time)
		   {}
		} // for i
	} // for j
	////////////////////////
	//// Init -- Adjust Knee Ankle


	bSuccess = WaitReady(ReadyTime());

	return bSuccess;
}

void CSpider::SetFootY(uint8_t Leg,float Angle)
{
  float KneeAngle  = Angle;
  float AnkleAngle = Angle;
  if(Angle < 90 && Angle > -90)
  {
	  if(KneeAngle > 90)
		Angle =  90;
	  else if(Angle <-90)
		Angle = -90;
	  m_szLeg[Leg]->MoveJoint(CSpiderLeg::Ankle,AnkleAngle);
	  m_szLeg[Leg]->MoveJoint(CSpiderLeg::Knee,KneeAngle);
  }
}

bool CSpider::WaitReady(uint32_t Timeout)
{

	bool bReady = false;
	uint32_t TimeStart;
	//	uint32_t Timeout;
	//	Timeout = alt_nticks() + (uint32_t)(fTimeoutSecond * (float)alt_ticks_per_second());
	//	Timeout = OS_GetTickCount() + (uint32_t)(fTimeoutSecond * (float)OS_TicksPerSecond());

	Timeout = OS_GetTickCount() + (Timeout *3);
	while(!bReady){
		bReady = IsReady();
		if (!bReady && (OS_GetTickCount() > Timeout))
			break;
	}
	if (!bReady){
		printf("[CSpider]!WaitReady, TimeStart=%u, Timeout=%u, CurrentTime=%u\r\n", TimeStart , Timeout, OS_GetTickCount());
	}

	return bReady;
}

bool CSpider::IsReady(void)
{
	bool bReady = true;
	int i;

	for(i=0;i<LEG_NUM && bReady;i++){
		if (!m_szLeg[i]->IsReady())
			bReady = false;
	}
	return bReady;
}

void CSpider::MoveForward(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
	  printf("MoveForward \n");
	int num;
	for(num=0;num<Repeat_Num && m_bAbort!= true;num++)
	{
		MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
		MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
		MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
	}
	m_bAbort = false;

}

void CSpider::MoveBackward(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
	  printf("MoveBackward \n");
	int num=0;
	for(num=0;num<Repeat_Num && m_bAbort!= true;num++)
	{
	  MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
	  MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
	  MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
	  WaitReady(ReadyTime());
	}
	m_bAbort = false;

}

void CSpider::RotatelRight(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
	  printf("RotatelRight \n");
	int num=0;
	for(num=0;num<Repeat_Num && m_bAbort!= true;num++)
	{
	   if(m_szLeg[LEG_RM]->GetfAngle(CSpiderLeg::Hip)!=0 && m_szLeg[LEG_LM]->GetfAngle(CSpiderLeg::Hip)!=0)
		   Reset();
	   MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
	   WaitReady(ReadyTime());
	   MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base-30,HipM_Base+30,HipB_Base-30);
	   WaitReady(ReadyTime());
	   MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
	   WaitReady(ReadyTime());
	   MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
	   WaitReady(ReadyTime());
	   MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base,HipM_Base,HipB_Base);
	   WaitReady(ReadyTime());
	   MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
	   WaitReady(ReadyTime());
	}
	m_bAbort = false;

}

void CSpider::RotatelLeft(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
	  printf("RotateLeft \n");
	int num=0;
	for(num=0;num<Repeat_Num && m_bAbort!= true;num++)
	{
		if(m_szLeg[LEG_RM]->GetfAngle(CSpiderLeg::Hip)!=0 && m_szLeg[LEG_LM]->GetfAngle(CSpiderLeg::Hip)!=0)
			Reset();
		MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base+30,HipM_Base-30,HipB_Base+30);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
	    MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
	    WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base,HipM_Base,HipB_Base);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
	}
	m_bAbort = false;
}

void CSpider::MoveParallelL(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
	  printf("MoveParallelL \n");
	//	float fAngleAdjust_F = -40.0;
	//	float fAngleAdjust_B =  40.0;

	int num=0;
	for(num=0;num<Repeat_Num;num++)
	{
		  MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		  WaitReady(ReadyTime());
		  MoveTripod(TRIPOD1,CSpiderLeg::Ankle,Ankle_Base-15,Ankle_Base+15,Ankle_Base-15);
		  WaitReady(ReadyTime());
		  MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		  WaitReady(ReadyTime());
		  MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Up_Base+15,Knee_Up_Base-15,Knee_Up_Base+15);
		  WaitReady(ReadyTime());
		  MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		  WaitReady(ReadyTime());

		  SetFootY(LEG_RF,Knee_Down_Base);
		  SetFootY(LEG_LM,Knee_Down_Base);
		  SetFootY(LEG_RB,Knee_Down_Base);
		  WaitReady(ReadyTime());
	}
}

void CSpider::MoveParallelR(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
	  printf("MoveParallelR \n");
	int num=0;
	for(num=0;num<Repeat_Num;num++)
	{
		  MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		  WaitReady(ReadyTime());
		  MoveTripod(TRIPOD2,CSpiderLeg::Ankle,Ankle_Base-15,Ankle_Base+15,Ankle_Base-15);
		  WaitReady(ReadyTime());
		  MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		  WaitReady(ReadyTime());
		  MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Up_Base+15,Knee_Up_Base-15,Knee_Up_Base+15);
		  WaitReady(ReadyTime());

		  MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		  WaitReady(ReadyTime());

		  SetFootY(LEG_LF,Knee_Down_Base);
		  SetFootY(LEG_RM,Knee_Down_Base);
		  SetFootY(LEG_LB,Knee_Down_Base);
		  WaitReady(ReadyTime());
	}
}

void CSpider::BodyUpDown(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
	  printf("BodyUp \n");
	int num=0;
	for(num=0;num<Repeat_Num;num++)
	{
	  SetFootY(LEG_LF,Body_Up_Base+20);//
	  SetFootY(LEG_LM,Body_Up_Base+20);
	  SetFootY(LEG_LB,Body_Up_Base+20);
	  SetFootY(LEG_RF,Body_Up_Base+20);
	  SetFootY(LEG_RM,Body_Up_Base+20);
	  SetFootY(LEG_RB,Body_Up_Base+20);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base+Dance_Angle,HipM_Base-Dance_Angle,HipB_Base+Dance_Angle);
	  MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base-Dance_Angle,HipM_Base+Dance_Angle,HipB_Base-Dance_Angle);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base-Dance_Angle,HipM_Base+Dance_Angle,HipB_Base-Dance_Angle);
	  MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base+Dance_Angle,HipM_Base-Dance_Angle,HipB_Base+Dance_Angle);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base,HipM_Base,HipB_Base);
	  MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base,HipM_Base,HipB_Base);
	  WaitReady(ReadyTime());
	  SetFootY(LEG_LF,Body_Down_Base);
	  SetFootY(LEG_LM,Body_Down_Base);
	  SetFootY(LEG_LB,Body_Down_Base);
	  SetFootY(LEG_RF,Body_Down_Base);
	  SetFootY(LEG_RM,Body_Down_Base);
	  SetFootY(LEG_RB,Body_Down_Base);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base+Dance_Angle,HipM_Base-Dance_Angle,HipB_Base+Dance_Angle);
	  MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base-Dance_Angle,HipM_Base+Dance_Angle,HipB_Base-Dance_Angle);
	  WaitReady(ReadyTime());
	  MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base-Dance_Angle,HipM_Base+Dance_Angle,HipB_Base-Dance_Angle);
	  MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base+Dance_Angle,HipM_Base-Dance_Angle,HipB_Base+Dance_Angle);
	  WaitReady(ReadyTime());

	  MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base,HipM_Base,HipB_Base);
	  MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base,HipM_Base,HipB_Base);
	  WaitReady(ReadyTime());

	}
}

void CSpider::BodyForward()
{
	if (m_bDebugDump)
	  printf("BodyForward \n");
	m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Hip,HipF_Base+Dance_Angle);
	m_szLeg[LEG_RM]->MoveJoint(CSpiderLeg::Hip,HipM_Base+Dance_Angle);
	m_szLeg[LEG_RB]->MoveJoint(CSpiderLeg::Hip,HipB_Base+Dance_Angle);
	m_szLeg[LEG_LF]->MoveJoint(CSpiderLeg::Hip,HipF_Base+Dance_Angle);
	m_szLeg[LEG_LM]->MoveJoint(CSpiderLeg::Hip,HipM_Base+Dance_Angle);
	m_szLeg[LEG_LB]->MoveJoint(CSpiderLeg::Hip,HipB_Base+Dance_Angle);
	WaitReady(ReadyTime());
}

void CSpider::BodyNone()
{
	if (m_bDebugDump)
	  printf("BodyNone \n");
	m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Hip,HipF_Base);
	m_szLeg[LEG_RM]->MoveJoint(CSpiderLeg::Hip,HipM_Base);
	m_szLeg[LEG_RB]->MoveJoint(CSpiderLeg::Hip,HipB_Base);
	m_szLeg[LEG_LF]->MoveJoint(CSpiderLeg::Hip,HipF_Base);
	m_szLeg[LEG_LM]->MoveJoint(CSpiderLeg::Hip,HipM_Base);
	m_szLeg[LEG_LB]->MoveJoint(CSpiderLeg::Hip,HipB_Base);
	WaitReady(ReadyTime());
}

void CSpider::BodyBackward()
{
	if (m_bDebugDump)
	  printf("BodyBackward \n");
	m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Hip,HipF_Base-Dance_Angle);
	m_szLeg[LEG_RM]->MoveJoint(CSpiderLeg::Hip,HipM_Base-Dance_Angle);
	m_szLeg[LEG_RB]->MoveJoint(CSpiderLeg::Hip,HipB_Base-Dance_Angle);
	m_szLeg[LEG_LF]->MoveJoint(CSpiderLeg::Hip,HipF_Base-Dance_Angle);
	m_szLeg[LEG_LM]->MoveJoint(CSpiderLeg::Hip,HipM_Base-Dance_Angle);
	m_szLeg[LEG_LB]->MoveJoint(CSpiderLeg::Hip,HipB_Base-Dance_Angle);
	WaitReady(ReadyTime());
}

void CSpider::TiltRight()
{
	if (m_bDebugDump)
	  printf("TiltRight \n");
	SetFootY(LEG_LF,Knee_Down_Base-Dance_Angle);
	SetFootY(LEG_LM,Knee_Down_Base-Dance_Angle);
	SetFootY(LEG_LB,Knee_Down_Base-Dance_Angle);
	SetFootY(LEG_RF,Knee_Down_Base+Dance_Angle);
	SetFootY(LEG_RM,Knee_Down_Base+Dance_Angle);
	SetFootY(LEG_RB,Knee_Down_Base+Dance_Angle);
	WaitReady(ReadyTime());
}

void CSpider::TiltNone()
{
	if (m_bDebugDump)
	  printf("TiltNone \n");
	SetFootY(LEG_LF,Knee_Down_Base);
	SetFootY(LEG_LM,Knee_Down_Base);
	SetFootY(LEG_LB,Knee_Down_Base);
	SetFootY(LEG_RF,Knee_Down_Base);
	SetFootY(LEG_RM,Knee_Down_Base);
	SetFootY(LEG_RB,Knee_Down_Base);
	WaitReady(ReadyTime());
}

void CSpider::TiltLeft()
{
	if (m_bDebugDump)
	  printf("TiltLeft \n");
	SetFootY(LEG_LF,Knee_Down_Base+Dance_Angle);
	SetFootY(LEG_LM,Knee_Down_Base+Dance_Angle);
	SetFootY(LEG_LB,Knee_Down_Base+Dance_Angle);
	SetFootY(LEG_RF,Knee_Down_Base-Dance_Angle);
	SetFootY(LEG_RM,Knee_Down_Base-Dance_Angle);
	SetFootY(LEG_RB,Knee_Down_Base-Dance_Angle);

	WaitReady(ReadyTime());
}

void CSpider::TiltForward()
{
	if (m_bDebugDump)
	  printf("TiltForward \n");
	SetFootY(LEG_LF,Knee_Down_Base+Dance_Angle);
	SetFootY(LEG_LM,Knee_Down_Base   );
	SetFootY(LEG_LB,Knee_Down_Base-Dance_Angle);
	SetFootY(LEG_RF,Knee_Down_Base+Dance_Angle);
	SetFootY(LEG_RM,Knee_Down_Base   );
	SetFootY(LEG_RB,Knee_Down_Base-Dance_Angle);
	WaitReady(ReadyTime());
}

void CSpider::TiltBackward()
{
	if (m_bDebugDump)
	  printf("TiltBackward \n");
	SetFootY(LEG_LF,Knee_Down_Base-Dance_Angle);
	SetFootY(LEG_LM,Knee_Down_Base   );
	SetFootY(LEG_LB,Knee_Down_Base+Dance_Angle);
	SetFootY(LEG_RF,Knee_Down_Base-Dance_Angle);
	SetFootY(LEG_RM,Knee_Down_Base   );
	SetFootY(LEG_RB,Knee_Down_Base+Dance_Angle);
	WaitReady(ReadyTime());
}

void CSpider::MoveTripod(TRIPOD_ID Pod,CSpiderLeg::JOINT_ID Joint,float AngleF,float AngleM,float AngleB)
{
	if(Pod == 0)
	{
		m_szLeg[LEG_RF]->MoveJoint(Joint,AngleF);
		m_szLeg[LEG_LM]->MoveJoint(Joint,AngleM);
		m_szLeg[LEG_RB]->MoveJoint(Joint,AngleB);
	}
	else if(Pod == 1)
	{
		m_szLeg[LEG_LF]->MoveJoint(Joint,AngleF);
		m_szLeg[LEG_RM]->MoveJoint(Joint,AngleM);
		m_szLeg[LEG_LB]->MoveJoint(Joint,AngleB);
	}
	else if(Pod == 2) //bipod 1
	{
		m_szLeg[LEG_RF]->MoveJoint(Joint,AngleF);
		m_szLeg[LEG_LB]->MoveJoint(Joint,AngleB);
	}
	else if(Pod == 3) //bipod 2
	{
		m_szLeg[LEG_LF]->MoveJoint(Joint,AngleF);
		m_szLeg[LEG_RB]->MoveJoint(Joint,AngleB);
	}
	else if(Pod == 4) //bipod 4
	{
		m_szLeg[LEG_LM]->MoveJoint(Joint,AngleM);
		m_szLeg[LEG_RM]->MoveJoint(Joint,AngleM);
	}
	else if(Pod == 5) //quadpod
	{
		m_szLeg[LEG_LF]->MoveJoint(Joint,AngleF);
		m_szLeg[LEG_RB]->MoveJoint(Joint,AngleB);
		m_szLeg[LEG_RF]->MoveJoint(Joint,AngleF);
		m_szLeg[LEG_LB]->MoveJoint(Joint,AngleB);
	}
}

void CSpider::Stomp(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
	  printf("Stomp \n");
	int num=0;
	for(num=0;num<Repeat_Num;num++)
	{
		m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Knee,Knee_Up_Base);
		WaitReady(ReadyTime());
		m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Knee,Knee_Down_Base);
		WaitReady(ReadyTime());
	}
}

uint32_t CSpider::ReadyTime(void)
{
	uint32_t TotalReadyTime = 0, ReadyTime;
	int i;

	for(i=0;i<LEG_NUM;i++){
		ReadyTime = m_szLeg[i]->ReadyTime();
		if (ReadyTime > TotalReadyTime)
			TotalReadyTime = ReadyTime;
	}
	return TotalReadyTime;
}

bool CSpider::Standup(void)
{
	bool bSuccess;
	int i;
	if (m_bDebugDump)
		printf("[Spider]Standup\r\n");
  ////////////////////////
	//// Stand up  -- Adjust Hip
	const int fAngleAdjust_F = -20;
	const int fAngleAdjust_B =  20;
	float fszJoin0Angle[] =
	{fAngleAdjust_F,0,fAngleAdjust_B,
	 fAngleAdjust_F,0,fAngleAdjust_B};
	for(i=0;i<LEG_NUM;i++)
	  m_szLeg[i]->MoveJoint(CSpiderLeg::Hip, fszJoin0Angle[i]);
	bSuccess = WaitReady(ReadyTime());

	////////////////////////
	//// Stand up  -- Adjust Knee ankle
	float KneeAngle = 90;
	const float AnkleAngle = 45.0;
	while(bSuccess && KneeAngle >= 45.0){
		for(i=0;i<LEG_NUM;i++){
			m_szLeg[i]->MoveJoint(CSpiderLeg::Knee, KneeAngle);
			m_szLeg[i]->MoveJoint(CSpiderLeg::Ankle,AnkleAngle);
		}// for
		bSuccess = WaitReady(ReadyTime());
		KneeAngle -= 5.0;
	}
	if (bSuccess)
		Reset();

	return bSuccess;

}

void CSpider::Reset(void)
{
	if (m_bDebugDump)
	  printf("Reset\n");
	int i;
	const int fAngleAdjust_F = -20;
	const int fAngleAdjust_B =  20;
	float fszJoin0Angle[] =
	{fAngleAdjust_F,0,fAngleAdjust_B,
	 fAngleAdjust_F,0,fAngleAdjust_B};

	////////////////////////
	////Reset Hip Knee ankle
	for(i=0;i<LEG_NUM-3;i++)
	{
		m_szLeg[i]->MoveJoint(CSpiderLeg::Knee, Knee_Up_Base);
		m_szLeg[LEG_NUM-i-1]->MoveJoint(CSpiderLeg::Knee, Knee_Up_Base);
		m_szLeg[i]->MoveJoint(CSpiderLeg::Hip, fszJoin0Angle[i]);
		m_szLeg[LEG_NUM-i-1]->MoveJoint(CSpiderLeg::Hip, fszJoin0Angle[LEG_NUM-i-1]);
		m_szLeg[i]->MoveJoint(CSpiderLeg::Ankle, Ankle_Base);
		m_szLeg[LEG_NUM-i-1]->MoveJoint(CSpiderLeg::Ankle, Ankle_Base);
		WaitReady(ReadyTime());
		m_szLeg[i]->MoveJoint(CSpiderLeg::Knee, Knee_Down_Base);
		m_szLeg[LEG_NUM-i-1]->MoveJoint(CSpiderLeg::Knee, Knee_Down_Base);
		WaitReady(ReadyTime());
	}
}

void CSpider::ByeBye(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
		printf("[Spider]Good Bye\n");
	int num=0;
	for(num=0;num<Repeat_Num;num++)
	{
	  m_szLeg[LEG_LB]->MoveJoint(CSpiderLeg::Hip,HipB_Base-20);
	  m_szLeg[LEG_RB]->MoveJoint(CSpiderLeg::Hip,HipB_Base-20);
	  m_szLeg[LEG_LM]->MoveJoint(CSpiderLeg::Hip,HipM_Base+20);
	  m_szLeg[LEG_RM]->MoveJoint(CSpiderLeg::Hip,HipM_Base+20);
	  WaitReady(ReadyTime());
	  SetFootY(LEG_LM,Knee_Down_Base   );
	  SetFootY(LEG_LB,Knee_Down_Base+Dance_Angle);
	  SetFootY(LEG_RM,Knee_Down_Base   );
	  SetFootY(LEG_RB,Knee_Down_Base+Dance_Angle);
	  WaitReady(ReadyTime());
	  m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Knee,Knee_Up_Base);
	  WaitReady(ReadyTime());
	  m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Ankle,Ankle_Base-90);
	  WaitReady(ReadyTime());
	  m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Hip,HipF_Base+30);
	  WaitReady(ReadyTime());
	  m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Hip,HipF_Base-10);
	  WaitReady(ReadyTime());

	}
}

void CSpider::SetSpeed(int Speed)
{
	if (m_bDebugDump)
	  printf("[Spider]SetSpeed\n");
	int i,j,nJointNum=0;
	CSpiderLeg::JOINT_ID szJoint[] = {CSpiderLeg::Hip, CSpiderLeg::Knee, CSpiderLeg::Ankle};
	nJointNum = sizeof(szJoint)/sizeof(szJoint[0]);
	if(Speed > 100)
		Speed = 100;
	else if(Speed <10)
		Speed = 10;
	for(j=0;j<nJointNum;j++){
		for(i=0;i<LEG_NUM;i++){
			m_szLeg[i]->SetSpeed(szJoint[j],Speed);
		} // for i
	} // for j
}

void CSpider::Abort(void)
{
	m_bAbort = true;
}


void CSpider::DEMO_Rollover(void)
{
	int i;

	m_szLeg[LEG_LF]->MoveJoint(CSpiderLeg::Knee,Knee_Up_Base);
	m_szLeg[LEG_RB]->MoveJoint(CSpiderLeg::Knee,Knee_Up_Base);
	WaitReady(ReadyTime());
	m_szLeg[LEG_LF]->MoveJoint(CSpiderLeg::Hip,HipF_Base+20);
	m_szLeg[LEG_RB]->MoveJoint(CSpiderLeg::Hip,HipB_Base+10);
	WaitReady(ReadyTime());
	m_szLeg[LEG_LF]->MoveJoint(CSpiderLeg::Knee,Knee_Down_Base);
	m_szLeg[LEG_RB]->MoveJoint(CSpiderLeg::Knee,Knee_Down_Base);
	WaitReady(ReadyTime());

	SetFootY(LEG_LF,Knee_Down_Base);
	SetFootY(LEG_LM,Knee_Down_Base-20);
	SetFootY(LEG_LB,Knee_Down_Base);
	SetFootY(LEG_RF,Knee_Down_Base-30);
	SetFootY(LEG_RM,Knee_Down_Base-30);
	SetFootY(LEG_RB,Knee_Down_Base-30);
	WaitReady(ReadyTime());

	SetFootY(LEG_RM,Knee_Down_Base-80);
	SetFootY(LEG_RB,Knee_Down_Base-80);
	SetFootY(LEG_LF,Knee_Down_Base-90);
	SetFootY(LEG_RB,Knee_Down_Base-90);
	WaitReady(ReadyTime());

	m_szLeg[LEG_RF]->SetSpeed(CSpiderLeg::Knee,90);
	m_szLeg[LEG_RM]->SetSpeed(CSpiderLeg::Knee,90);
	m_szLeg[LEG_RF]->SetSpeed(CSpiderLeg::Ankle,90);
	m_szLeg[LEG_RM]->SetSpeed(CSpiderLeg::Ankle,70);
	for(i=0;i<7;i++)
	{
	  m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Ankle,Ankle_Base-20);
	  m_szLeg[LEG_RM]->MoveJoint(CSpiderLeg::Ankle,Ankle_Base+20);
	  m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Knee,Knee_Down_Base-10);
	  m_szLeg[LEG_RM]->MoveJoint(CSpiderLeg::Knee,Knee_Up_Base+10);
	  WaitReady(ReadyTime());
	  m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Ankle,Ankle_Base+20);
	  m_szLeg[LEG_RM]->MoveJoint(CSpiderLeg::Ankle,Ankle_Base-20);
	  m_szLeg[LEG_RF]->MoveJoint(CSpiderLeg::Knee,Knee_Up_Base+20);
	  m_szLeg[LEG_RM]->MoveJoint(CSpiderLeg::Knee,Knee_Down_Base-40);
	  WaitReady(ReadyTime());
	}
	m_szLeg[LEG_RF]->SetSpeed(CSpiderLeg::Knee,50);
	m_szLeg[LEG_RM]->SetSpeed(CSpiderLeg::Knee,50);
	m_szLeg[LEG_RF]->SetSpeed(CSpiderLeg::Ankle,50);
	m_szLeg[LEG_RM]->SetSpeed(CSpiderLeg::Ankle,50);

	SetFootY(LEG_LF,Knee_Down_Base);
	SetFootY(LEG_RB,Knee_Down_Base);
	WaitReady(ReadyTime());
	SetFootY(LEG_LM,Knee_Down_Base);
	SetFootY(LEG_LB,Knee_Down_Base);
	SetFootY(LEG_RF,Knee_Down_Base);
	SetFootY(LEG_RM,Knee_Down_Base);
	WaitReady(ReadyTime());
	Reset();
}

void CSpider::DEMO_Dance(uint8_t Repeat_Num)
{
	if (m_bDebugDump)
		printf("[Spider]Dance\n");
	int num=0;
	for(num=0;num<Repeat_Num;num++)
	{
       Stomp(8);
       Reset();
       RotatelRight(2);
       RotatelLeft(2);
       BodyForward();
       usleep(1000000);
       BodyNone();
       BodyBackward();
       usleep(1000000);
       BodyNone();
       DEMO_Rollover();
       MoveForward(1);
       Reset();
       TiltForward();
       TiltNone();
       TiltBackward();
       TiltNone();
       BodyUpDown(1);
       MoveBackward(1);
       Reset();
       TiltRight();
       TiltNone();
       TiltLeft();
       TiltNone();
       TiltRight();
       TiltNone();
       TiltLeft();
       TiltNone();
       ByeBye(3);
       Reset();
	}
}


// Spider Controller Functions

void CSpider::SetLegsBase()
{
	for(int leg = 0; leg < 6; leg++)
	{
		// Hips
		if(leg == LEG_RF || leg == LEG_LF)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipF_Base);
		}
		else if(leg == LEG_RM || leg == LEG_LM)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipM_Base);
		}
		else if(leg == LEG_RB || leg == LEG_LB)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipB_Base);
		}

		// Knee
		m_szLeg[leg]->MoveJoint(CSpiderLeg::Knee,Knee_Down_Base);

		// Ankle
		m_szLeg[leg]->MoveJoint(CSpiderLeg::Ankle,Ankle_Base);

	}
	WaitReady(ReadyTime());
}

void CSpider::Extend()
{
	for(int leg = 0; leg < 6; leg++)
	{
		// Hips
		if(leg == LEG_RF || leg == LEG_LF)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipF_Base);
		}
		else if(leg == LEG_RM || leg == LEG_LM)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipM_Base);
		}
		else if(leg == LEG_RB || leg == LEG_LB)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipB_Base);
		}

		// Knee
		m_szLeg[leg]->MoveJoint(CSpiderLeg::Knee,Knee_Extend);

		// Ankle
		m_szLeg[leg]->MoveJoint(CSpiderLeg::Ankle,Ankle_Extend);

	}
	WaitReady(ReadyTime());
}

void CSpider::Fold()
{
	for(int leg = 0; leg < 6; leg++)
	{
		// Hips
		if(leg == LEG_RF || leg == LEG_LF)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipF_Fold);
		}
		else if(leg == LEG_RM || leg == LEG_LM)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipM_Fold);
		}
		else if(leg == LEG_RB || leg == LEG_LB)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipB_Fold);
		}

		// Knee
		m_szLeg[leg]->MoveJoint(CSpiderLeg::Knee,Knee_Fold);

		// Ankle
		m_szLeg[leg]->MoveJoint(CSpiderLeg::Ankle,Ankle_Fold);

	}
	WaitReady(ReadyTime());
}

void CSpider::Grab()
{
	for(int leg = 0; leg < 6; leg++)
	{
		// Hips and Knees
		if(leg == LEG_RF || leg == LEG_LF)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipF_Base);
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Knee,KneeF_Full_Grab);

		}
		else if(leg == LEG_RM || leg == LEG_LM)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipM_Base);
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Knee,KneeM_Full_Grab);

		}
		else if(leg == LEG_RB || leg == LEG_LB)
		{
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Hip,HipB_Base);
			m_szLeg[leg]->MoveJoint(CSpiderLeg::Knee,KneeB_Full_Grab);

		}

		// Ankle
		m_szLeg[leg]->MoveJoint(CSpiderLeg::Ankle,Ankle_Full_Grab);

	}
	WaitReady(ReadyTime());
}

void CSpider::SetJointPosition(int leg, int joint, int position)
{
	m_szLeg[leg]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(joint), position);
	WaitReady(ReadyTime());
}

void CSpider::raiseLegs(){
	
	SetJointPosition(LEG_RM,2,-90);
	SetJointPosition(LEG_LM,2,-90);
}

void CSpider::fourLeggedStatic(uint8_t Repeat_Num){
	for (int i = 0; i < Repeat_Num; i++) {
		MoveTripod(QUADPOD,CSpiderLeg::Hip,-30,0,10);
		WaitReady(ReadyTime());
		
		SetJointPosition(LEG_RB,1, 45);//set back leg balance
		SetJointPosition(LEG_LB,1, 45);
		
		//move LB half way
		SetJointPosition(LEG_RF,1, 50);// lean opposite of LB
		SetJointPosition(LEG_LB,1, 60);// raise leg 
		SetJointPosition(LEG_LB,0, 20);// move hip half way
		SetJointPosition(LEG_LB,1, 45);// knee down
		SetJointPosition(LEG_RF,1, 35);// re-balance
		
		//move RB full
		SetJointPosition(LEG_LF,1, 60);// tilt opposite of RB
		SetJointPosition(LEG_RB,1, 35);// push off RB to tilt
		SetJointPosition(LEG_RB,0, 40);// move hip forward 
		SetJointPosition(LEG_RB,1, 45);// knee down
		SetJointPosition(LEG_LF,1, 45);// re-balance
		
		//move LB rest of way
		SetJointPosition(LEG_RF,1, 50);// lean opposite of LB
		SetJointPosition(LEG_LB,1, 60);// raise leg 
		SetJointPosition(LEG_LB,0, 40);// move hip full way
		SetJointPosition(LEG_LB,1, 45);// knee down
		SetJointPosition(LEG_RF,1, 35);// re-balance
		
		//move RF
		SetJointPosition(LEG_RF,1, 50);// raise knee 
		SetJointPosition(LEG_RF,0, -10);// move hip
		SetJointPosition(LEG_RB,1, 35);// lower knee
		
		//move LF
		SetJointPosition(LEG_LF,1, 60);// raise knee 
		SetJointPosition(LEG_LF,0, -10);// move hip
		SetJointPosition(LEG_LF,1, 45);// lower knee
		
	}
}

void CSpider::moveOneLeg(int legToMove, int opposite){
	if (legToMove == LEG_LF) {
		SetJointPosition(opposite, 1, 75);
		WaitReady(ReadyTime());
		SetJointPosition(legToMove, 1, 60);
		WaitReady(ReadyTime());
		m_szLeg[legToMove]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base-12);
		
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_RF]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_RB]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_LB]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		
		WaitReady(ReadyTime());
		SetJointPosition(legToMove, 1, 45);
		SetJointPosition(opposite, 1, 45);
	}
	else if (legToMove == LEG_LB) {
		SetJointPosition(opposite, 1, 65);
		WaitReady(ReadyTime());
		SetJointPosition(legToMove, 1, 60);
		WaitReady(ReadyTime());
		m_szLeg[legToMove]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base-12);
		
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_RF]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_RB]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_LF]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		
		WaitReady(ReadyTime());
		SetJointPosition(legToMove, 1, 45);
		SetJointPosition(opposite, 1, 35);
	}
	else if (legToMove == LEG_RB) {
		SetJointPosition(opposite, 1, 75);
		WaitReady(ReadyTime());
		SetJointPosition(legToMove, 1, 60);
		WaitReady(ReadyTime());
		m_szLeg[legToMove]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base-12);
		
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_RF]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_LB]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_LF]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		
		WaitReady(ReadyTime());
		SetJointPosition(legToMove, 1, 45);
		SetJointPosition(opposite, 1, 45);
	}
	else if (legToMove == LEG_RF) {
		SetJointPosition(opposite, 1, 75);
		WaitReady(ReadyTime());
		SetJointPosition(legToMove, 1, 50);
		WaitReady(ReadyTime());
		m_szLeg[legToMove]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base-20);
		
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_RB]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_LB]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), m_szLeg[LEG_LF]->GetfAngle(static_cast<CSpiderLeg::JOINT_ID>(0))+8);
		
		WaitReady(ReadyTime());
		SetJointPosition(legToMove, 1, 35);
		SetJointPosition(opposite, 1, 45);
	}

}

void CSpider::fourLeggedDynamic(uint8_t Repeat_Num){
	for(int i = 0; i < Repeat_Num; i++){
		MoveTripod(BIPOD1,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(BIPOD1,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
		MoveTripod(BIPOD2,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
		WaitReady(ReadyTime());
		MoveTripod(BIPOD1,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
		MoveTripod(BIPOD2,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(BIPOD1,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
		MoveTripod(BIPOD2,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
		WaitReady(ReadyTime());
		MoveTripod(BIPOD2,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
	}

}

void CSpider::sixLeggedDynamic(uint8_t Repeat_Num){
	for(int i = 0; i < Repeat_Num; i++){
		//QUADPOD motion
		MoveTripod(QUADPOD, CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(QUADPOD,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
		MoveTripod(BIPOD3,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
		WaitReady(ReadyTime());
		MoveTripod(QUADPOD,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		//BIPOD3 motion
		WaitReady(ReadyTime());
		MoveTripod(BIPOD3,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(QUADPOD,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
		MoveTripod(BIPOD3,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
		WaitReady(ReadyTime());
		MoveTripod(BIPOD3,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
	}
}

void CSpider::sixLeggedStatic(uint8_t Repeat_Num){
	for(int i=0;i<Repeat_Num;i++)
	{
		MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
		MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Up_Base,Knee_Up_Base,Knee_Up_Base);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD1,CSpiderLeg::Hip,HipF_Base-20,HipM_Base-20,HipB_Base-20);
		MoveTripod(TRIPOD2,CSpiderLeg::Hip,HipF_Base+20,HipM_Base+20,HipB_Base+20);
		WaitReady(ReadyTime());
		MoveTripod(TRIPOD2,CSpiderLeg::Knee,Knee_Down_Base,Knee_Down_Base,Knee_Down_Base);
		WaitReady(ReadyTime());
	}
}

/* void CSpider::fourLeggedStatic(uint8_t Repeat_Num)
{
	for (int i = 0; i < Repeat_Num; i++) 
	{
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Up_Base); // Move RF off ground
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base+10); // adjust RF hip forward 10 
		WaitReady(ReadyTime());
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base-10); //Displace body in triangle of stability
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base-10); //
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base-10); //
		WaitReady(ReadyTime());
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Down_Base-10); // Place RF on ground
		WaitReady(ReadyTime());
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Up_Base); // Move LF off ground
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base+10); // adjust F hip forward 20 (could be 10 if BD adjusts this)
		WaitReady(ReadyTime()); // maybe not neccesary but two movements of one joint seems like it might be an issue
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Down_Base); //Place LF on ground (will know here if aligned with RF)
		WaitReady(ReadyTime()); 
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Up_Base); // Repeat seqence for RB
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base+10); 
		WaitReady(ReadyTime()); 
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Down_Base); 
		WaitReady(ReadyTime()); 
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Up_Base); // Repeat seqence for LB
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base+10); 
		WaitReady(ReadyTime()); 
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Down_Base); 
		WaitReady(ReadyTime());
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base); //restore relative postion 
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base);
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base);
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base);
		// switch legs 
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Up_Base); 
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base+10);  
		WaitReady(ReadyTime());
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base-10); 
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base-10); 
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base-10); 
		WaitReady(ReadyTime());
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Down_Base); 
		WaitReady(ReadyTime());
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Up_Base); 
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base+10); 
		WaitReady(ReadyTime());
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Down_Base); 
		WaitReady(ReadyTime()); 
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Up_Base); 
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base+10); 
		WaitReady(ReadyTime()); 
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Down_Base); 
		WaitReady(ReadyTime()); 
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Up_Base); 
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base+10); 
		WaitReady(ReadyTime()); 
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(1), Knee_Down_Base); 
		WaitReady(ReadyTime());
		m_szLeg[LEG_RF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base); 
		m_szLeg[LEG_LF]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipF_Base);
		m_szLeg[LEG_RB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base);
		m_szLeg[LEG_LB]->MoveJoint(static_cast<CSpiderLeg::JOINT_ID>(0), HipB_Base);
	}
} */