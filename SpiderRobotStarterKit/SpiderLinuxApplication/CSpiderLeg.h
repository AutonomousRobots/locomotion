/*
 * CSpiderLeg.h
 *
 *  Created on: 2014/7/26
 *      Author: Richard
 */

#ifndef CSPIDERLEG_H_
#define CSPIDERLEG_H_

#include "CMotor.h"

class CSpiderLeg {
public:
	typedef enum{
		Hip=0,
		Knee,
		Ankle,
		JOINT_NUM
	}JOINT_ID;
protected:


	CMotor *m_szMotor[JOINT_NUM];


public:
	CSpiderLeg(int Joint0_MotorID,int Joint1_MotorID,int Joint2_MotorID);
	virtual ~CSpiderLeg();

	void Reset(void);
	void MoveJoint(JOINT_ID JointID, float fAngle);
	bool IsReady(void);
	float GetfAngle(JOINT_ID JointID);
	uint32_t ReadyTime(void);
	void SetSpeed(JOINT_ID JointID,int Speed);
	uint32_t GetSpeed(JOINT_ID JointID);
	void Sleep(void);
	void WakeUp(void);
};

#endif /* CSPIDERLEG_H_ */
