/*
 * CMotor.h
 *
 *  Created on: 2014/7/26
 *      Author: Richard
 */

#ifndef CMOTOR_H_
#define CMOTOR_H_

#include "mmap.h"

class CMotor {
protected:
	int m_nMotorID;
	float m_fAngle;
	float m_fAngleCorrected;
	static bool m_bDebugDump;
	uint32_t m_ReadyTime;
	uint32_t m_Speed;

public:
	static CMMap *m_mmap;
	static int m_MotorCnt;


	CMotor(int nMonotrID);
	virtual ~CMotor();
	void Reset(void);
	void Move(float fAngle);
	bool IsReady(void);
	uint32_t ReadyTime(void);
	float GetfAngle(void);
	void SetSpeed(int Speed);
	uint32_t GetSpeed(void);
	void Sleep(void);
	void WakeUp(void);
	
};

#endif /* CMOTOR_H_ */
