/*
 * CMotor.cpp
 *
 *  Created on: 2014/7/26
 *      Author: Richard
 */

#include "terasic_os.h"
#include "CMotor.h"
#include <math.h>

#include "hwlib.h"
#include "socal/socal.h"
#include "socal/hps.h"
#include "socal/alt_gpio.h"
#include "hps_0.h"

#define HW_REGS_BASE ( ALT_STM_OFST )
#define HW_REGS_SPAN ( 0x04000000 )
#define HW_REGS_MASK ( HW_REGS_SPAN - 1 )


const uint32_t szPWM_Base[] = {
		PWM0_BASE,
		PWM1_BASE,
		PWM2_BASE,
		PWM3_BASE,
		PWM4_BASE,
		PWM5_BASE,
		PWM6_BASE,
		PWM7_BASE,
		PWM8_BASE,
		PWM9_BASE,
		PWM10_BASE,
		PWM11_BASE,
		PWM12_BASE,
		PWM13_BASE,
		PWM14_BASE,
		PWM15_BASE,
		PWM16_BASE,
		PWM17_BASE
};


#define REG_TOTAL_DUR	0
#define REG_HIGH_DUR	1
#define REG_ADJ_SPEED   2
#define REG_ADJ_STATUS  2
#define REG_ABORT       3

#define DUR_CLOCK_NUM (50000000/50)    // clock count in 20 ms (1000/50 = 20ms)
//#define PWM_DEGREE_0      (DUR_CLOCK_NUM*15/200)    // 1.5ms
//#define PWM_DEGREE_MAX    (DUR_CLOCK_NUM*20/200) // 2.0 ms
//#define PWM_DEGREE_MIN    (DUR_CLOCK_NUM*10/200)  // 1.0 ms
#define PWM_DEGREE_MAX    (DUR_CLOCK_NUM*25/200) // 2.5 ms 125000
#define PWM_DEGREE_MIN    (DUR_CLOCK_NUM*5/200)  // 0.5 ms 25000
#define PWM_SPEED_MAX     550
#define PWM_SPEED_MIN     1150
#define SPEED_MAX         100
#define SPEED_MIN         0

#define DEGREE_MIN		-90
#define DEGREE_MAX		 90

CMMap* CMotor::m_mmap=NULL;
int CMotor::m_MotorCnt=0;



bool CMotor::m_bDebugDump = false;


CMotor::CMotor(int MonotrID):
	m_nMotorID(MonotrID),
	m_fAngle(180.0),
	m_fAngleCorrected(0.0),
	m_ReadyTime(0),
	m_Speed(1800)
{
	// TODO Auto-generated constructor stub

	m_MotorCnt++;
	if (m_MotorCnt == 1){
		m_mmap = new CMMap(CMMap::MAP_H2F_LW_BRIDGE);
	}
		
	m_mmap->Reg32_Write(szPWM_Base[m_nMotorID], REG_TOTAL_DUR, DUR_CLOCK_NUM);
	m_mmap->Reg32_Write(szPWM_Base[m_nMotorID], REG_ADJ_SPEED, PWM_SPEED_MAX+PWM_SPEED_MIN);// PWM_SPEED_MAX+PWM_SPEED_MIN/2
	m_mmap->Reg32_Write(szPWM_Base[m_nMotorID], REG_ABORT, 0);
	m_mmap->Reg32_Write(szPWM_Base[m_nMotorID], REG_HIGH_DUR, 0x00);

}

CMotor::~CMotor() {
	// TODO Auto-generated destructor stub
	m_MotorCnt--;
	if (m_MotorCnt == 0){
		delete m_mmap;
		m_mmap = 0;
	}

}



void CMotor::Sleep(void){
    m_mmap->Reg32_Write(szPWM_Base[m_nMotorID], REG_ABORT, 1);
}
void CMotor::WakeUp(void){
    m_mmap->Reg32_Write(szPWM_Base[m_nMotorID], REG_ABORT, 0);
}

void CMotor::Reset(void){
	Move(0.0);
}

void CMotor::Move(float fAngle) {
	uint32_t PWM;
	const float MIN_ANGLE = DEGREE_MIN;
	const float MAX_ANGLE = DEGREE_MAX;
	const float MAX_PWM = PWM_DEGREE_MAX;
	const float MIN_PWM = PWM_DEGREE_MIN;
	
	//Operating speed 0.17sec/60degree(4.8v)
	const float fMotorSpeed = 0.17/60 * (float)m_Speed * 5.0; // second per angle  (2.0 is my torance) consider IP delay(speed)
	uint32_t nActionTickDur;
	nActionTickDur = (int)(fabs(m_fAngle-fAngle) * (float)OS_TicksPerSecond() * fMotorSpeed) ;
	if (nActionTickDur < OS_TicksPerSecond()/200)
		nActionTickDur = OS_TicksPerSecond()/200;
	
	m_ReadyTime = OS_GetTickCount() + nActionTickDur;
	if(m_nMotorID <9) //servo mirror
	{
		m_fAngle = -fAngle;
		  fAngle = -fAngle;
	}
	else
	    m_fAngle =  fAngle;

	/* motor spec
	 * angle:    min_angle ~ 0 ~ max_angle
	 * high dur: 0.5ms      1.5ms      2.5 ms
	 * value:    min_pwm  ~  0 ~ max_pwm
	 */

	PWM = (uint32_t)((fAngle + m_fAngleCorrected - MIN_ANGLE)/(float)(MAX_ANGLE-MIN_ANGLE)*(float)(MAX_PWM - MIN_PWM)) + MIN_PWM;

	if (m_bDebugDump)
		printf("Motor[%d] move %f angle (pwm=%d, wait %d tick)\r\n", (int)m_nMotorID, fAngle, PWM, nActionTickDur);
	m_mmap->Reg32_Write(szPWM_Base[m_nMotorID], REG_HIGH_DUR, PWM);
	//refresh sleep timer
}

bool CMotor::IsReady(void){
	bool bReady = false;
	uint32_t Status;
	
	Status = m_mmap->Reg32_Read(szPWM_Base[m_nMotorID],REG_ADJ_STATUS);
	if (Status == 1)
		bReady = true;
	else
		bReady = false;
	return bReady;
}

uint32_t CMotor::ReadyTime(void){
	return m_ReadyTime;
}

float CMotor::GetfAngle(void)
{
	return m_fAngle;
}

void CMotor::SetSpeed(int Speed)
{
	if(Speed >SPEED_MAX)
	  Speed = SPEED_MAX-1;
	else if(Speed <SPEED_MIN)
	  Speed  = SPEED_MIN;
	m_Speed = (uint32_t)(((PWM_SPEED_MIN-PWM_SPEED_MAX)/SPEED_MAX) * (SPEED_MAX-Speed))+PWM_SPEED_MAX;

	if (m_bDebugDump)
	  printf("Motor[%d] Speed %d (div=%d) \r\n", (int)m_nMotorID, Speed,m_Speed);
    m_mmap->Reg32_Write(szPWM_Base[m_nMotorID], REG_ADJ_SPEED,m_Speed);
}

uint32_t CMotor::GetSpeed(void)
{
	return m_Speed;
}


