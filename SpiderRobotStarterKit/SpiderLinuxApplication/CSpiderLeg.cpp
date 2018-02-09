/*
 * CSpiderLeg.cpp
 *
 *  Created on: 2014/7/26
 *      Author: Richard
 */

#include "terasic_os.h"
#include "CSpiderLeg.h"


CSpiderLeg::CSpiderLeg(int Joint0_MotorID,int Joint1_MotorID,int Joint2_MotorID)
{
	// TODO Auto-generated constructor stub
	int i, MotorID;

	for(i=0;i<JOINT_NUM;i++){
		switch(i){
		case 0: MotorID = Joint0_MotorID; break;
		case 1: MotorID = Joint1_MotorID; break;
		case 2: MotorID = Joint2_MotorID; break;
		default: assert(0); break;
		}
		m_szMotor[i] = new CMotor(MotorID);
	}
}

CSpiderLeg::~CSpiderLeg() {
	// TODO Auto-generated destructor stub
	int i;
	for(i=0;i<JOINT_NUM;i++){
		delete m_szMotor[i];
	}
}


void CSpiderLeg::Sleep(void) {
	int i;
	for(i=0;i<JOINT_NUM;i++){
		m_szMotor[i]->Sleep();
	}
}

void CSpiderLeg::WakeUp(void) {
	int i;
	for(i=0;i<JOINT_NUM;i++){
		m_szMotor[i]->WakeUp();
	}
}


void CSpiderLeg::Reset(void) {
	// TODO Auto-generated destructor stub
	int i;

	for(i=0;i<JOINT_NUM;i++){
		m_szMotor[i]->Reset();
	}
}

bool CSpiderLeg::IsReady(void){
	bool bReady = true;
	int i;

	for(i=0;i<JOINT_NUM && bReady;i++){
		if (!m_szMotor[i]->IsReady())
			bReady = false;
	}
	return bReady;
}

uint32_t CSpiderLeg::ReadyTime(void){
	uint32_t TotalReadyTime = 0, ReadyTime;
	int i;

	for(i=0;i<JOINT_NUM;i++){
		ReadyTime = m_szMotor[i]->ReadyTime();
		if (ReadyTime > TotalReadyTime)
			TotalReadyTime = ReadyTime;
	}
	return TotalReadyTime;
}

void CSpiderLeg::MoveJoint(JOINT_ID JointID, float fAngle){
	m_szMotor[JointID]->Move(fAngle);
}

float CSpiderLeg::GetfAngle(JOINT_ID JointID)
{
	return m_szMotor[JointID]->GetfAngle();
}

void CSpiderLeg::SetSpeed(JOINT_ID JointID,int Speed)
{
	m_szMotor[JointID]->SetSpeed(Speed);
}
uint32_t CSpiderLeg::GetSpeed(JOINT_ID JointID)
{
	return m_szMotor[JointID]->GetSpeed();
}

