/*
 * motor.h
 *
 *  Created on: Aug 1, 2014
 *      Author: loywong
 */

#ifndef MOTOR_H_
#define MOTOR_H_

#include <xdc/std.h>
#include "../Queue/Queue.h"

namespace TskMotor{

// task motor: motor, encoder & imu acq

extern volatile float OmgAdj;    // omega correction from TskAction

class MotorMsg
{
public:
    enum MsgType
	{
		EnableMotors    = 0x00010000,
		DisableMotors   = 0x00020000,
        Opengate        = 0x00030000,
		Closegate       = 0x00040000,
		EnablePwmtest   = 0x00050000,
		DisablePwmtest  = 0x00060000,
		EnableUnit_rsp  = 0x00070000,
		DisableUnit_rsp = 0x00080000
	};
};

struct VelOmega
{
    float Velocity;
    float Omega;
    VelOmega(float vel = 0.f, float omg = 0.f)
    {
        Velocity = vel;
        Omega = omg;
    }
};

extern volatile bool EncRAct;
extern volatile bool EncLAct;
extern volatile float EncLVel;
extern volatile float EncRVel;
extern volatile float EncVel; // avg velocity from 2 encoders
extern volatile float LV, AV;   // lv & av feedback for PIDs
extern volatile float DistanceAcc;
extern volatile int ServoPwmTest;

//---------------------lab2 test-----2017.3.9-----------------------------
extern volatile float DistanceAccL;
extern volatile float DistanceAccR;
//------------------------------------------------------------------------



extern volatile float CurrentV;	// Current Desired Linear Velocity

extern Mailbox_Handle MbCmd;
extern Queue<VelOmega> *QMotor;

void Init();

}

#endif /* MOTOR_H_ */
