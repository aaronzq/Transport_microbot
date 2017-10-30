/*
 * MotorPwm.h
 *
 *  Created on: Aug 3, 2016
 *      Author: loywong
 */

#ifndef MOTORENCIMU_MOTORPWM_H_
#define MOTORENCIMU_MOTORPWM_H_

void MotorPwmCoast();
void ServoPwmReset();

// r,l -> [-319, 319]
void MotorPwmSetDuty(short r, short l);
void ServoPwmSetDuty(int s);  //0-64000

void MotorPwmInit();

#endif /* MOTORENCIMU_MOTORPWM_H_ */
