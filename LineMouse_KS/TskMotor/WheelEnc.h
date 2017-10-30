/*
 * MotorEnc.h
 *
 *  Created on: Aug 3, 2016
 *      Author: loywong
 */

#include "../physparams.h"

#ifndef MOTORENCIMU_WHEELENC_H_
#define MOTORENCIMU_WHEELENC_H_

extern Semaphore_Handle SemMotTick;
extern Semaphore_Handle SemIrTick;
extern Semaphore_Handle SemActTick;

const float EncRes = 2048.f;
const float EncUnit = PP::RWheel * 2.f * 3.1415926536f / EncRes / PP::Ts;
const float GearRatio =15.f/100.f;

void WheelEncInit();
float WheelEncGetVel(volatile float &r, volatile float &l, volatile int &rp, volatile int &lp);

#endif /* MOTORENCIMU_WHEELENC_H_ */
