/*
 * motor.c
 *
 *  Created on: Aug 1, 2014
 *      Author: loywong
 *  Edited on : June  , 2017
 *  	Author: aaronzq
 *
 */

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <stdio.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include "../Pid/Pid.h"
#include "../Kalman/Kalman1Var.h"
#include <math.h>
//#include "../indicator/indicator.h"
//#include "../action/action.h"
//#include "../solve/solve.h"
//#include "../ir/ir.h"
//#include "../debug/debug.h"
#include "../physparams.h"

#include <inc/hw_sysctl.h>
#include <inc/hw_pwm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <linemouse_chassis.h>
#include <ti/drivers/GPIO.h>

#include "TskMotor.h"
#include "MotorPwm.h"
//#include "Imu.h"
#include "WheelEnc.h"

#include "../Queue/Queue.h"
#include "../TskTop/DbgUart.h"
#include "../TskTop/TskTop.h"

#include "../physparams.h"

namespace TskMotor
{


const float lvPidP = 191.519075602699f;
const float lvPidI = 3300.4379543161f;
const float lvPidD = 0.f;//-1.79891050754587f;
const float lvPidN = 0.f;//688.458615803393f;

const float avPidP = 3.46875141538602f;
const float avPidI = 76.762778295071f;
const float avPidD = 0.f;//-0.065326709136153f;
const float avPidN = 0.f;//192.064591913073f;

const float dLvDefault = 0.0f;
const float dAvDefault = 0.0f;

volatile bool EncRAct = false;
volatile bool EncLAct = false;
volatile float EncRVel = 0;
volatile float EncLVel = 0;
volatile float EncVel; // avg velocity from 2 encoders
volatile float DistanceAcc = 0.0f;

volatile int ServoPwmTest=0;

//-------------------------lab2 test--------2017.3.9-----------
volatile float DistanceAccL = 0.0f;
volatile float DistanceAccR = 0.0f;
//--------------------------------------------------------------

volatile float CurrentV = 0.0f;
volatile float OmgAdj = 0.0f;    // omega correction from TskAction
volatile float LV, AV;   // LV & AV feedback for PIDs
volatile float Temperature = 0.0f;

bool MotorEnabled = false;
bool PwmtestEnable= false;
bool Unit_rspEnable = false;


const int tskPrio = 12;
const int tskStkSize = 2560;
Task_Handle tsk;
//Error_Block eb;

Mailbox_Handle MbCmd;


Queue<VelOmega> *QMotor;
//VelOmega desiresrc(0.0f,0.0f);

inline float saturate(float v, float max, float min)
{
    return  v > max ? max :
            v < min ? min : v;
}

void task(UArg arg0, UArg arg1)
{
    MotorMsg::MsgType msg;


    int encRPulse;
    int encLPulse;


    VelOmega desire(dLvDefault, dAvDefault);    // desired lv & av from queue
    float lvPidOut, avPidOut;
    float lPwm, rPwm;
    Int16 leftPwm, rightPwm;

    int Servopwm;

    Pid lvPid(lvPidP, lvPidI, lvPidD, lvPidN, PP::Ts, -319.f, 319.f);
    Pid avPid(avPidP, avPidI, avPidD, avPidN, PP::Ts, -319.f, 319.f);

    // initial motor pwm
    MotorPwmInit();

    // initial qei
    WheelEncInit();

    Task_sleep(2);

    while(1)
    {
//        GPIO_write(DBMOUSE_LED_0, DBMOUSE_LED_OFF);
        if(!Semaphore_pend(SemMotTick, 2))
            System_abort("pend SemMotTick failed!\n");
//        GPIO_write(DBMOUSE_LED_0, DBMOUSE_LED_ON);

        // read encoder
        EncVel = WheelEncGetVel(EncRVel, EncLVel, encRPulse, encLPulse);  //EncVel is average of left and right velocity
        if(encRPulse) EncRAct = true;
        if(encLPulse) EncLAct = true;

        LV = EncVel;

        AV = (EncRVel - EncLVel) * (.5f / PP::W);


        DistanceAccL += EncLVel * PP::Ts;
		DistanceAccR += EncRVel * PP::Ts;
        DistanceAcc += LV * PP::Ts;



        if(MotorEnabled)
        {

            // read dlv&dav from fifo
            QMotor->De(desire); // dequeue, if empty, desire will not change
            if(Unit_rspEnable)
			{
				if (TskTop::secread<200)
				{
					desire.Velocity = 0.f;
					desire.Omega = 0.f;
				}
				else if(TskTop::secread<1000)
				{
					if(TskTop::Lvtest)
					{
						desire.Velocity = 0.2f;
						desire.Omega = 0.f;
					}
					else
					{
						desire.Velocity = 0.f;
						desire.Omega = 0.5f;
					}
				}
				else
				{
					desire.Velocity = 0.f;
					desire.Omega = 0.f;
				}
			}
            CurrentV = desire.Velocity;
            lvPidOut = lvPid.Tick(desire.Velocity - LV);
            avPidOut = avPid.Tick(desire.Omega + OmgAdj - AV);

            rPwm = (lvPidOut + avPidOut);
            lPwm = (lvPidOut - avPidOut);
            rightPwm = (int)saturate(rPwm, 319.f, -319.f);
            leftPwm = (int)saturate(lPwm, 319.f, -319.f);
            MotorPwmSetDuty(rightPwm, leftPwm);


            if(PwmtestEnable)
            {
				MotorPwmSetDuty(90, 30);
				Servopwm=ServoPwmTest;
				ServoPwmSetDuty(Servopwm);
            }

        }


        if(Mailbox_pend(MbCmd, &msg, BIOS_NO_WAIT))
        {
            switch(msg & 0xFFFF0000)
            {
            case MotorMsg::EnableMotors:
                QMotor->Clear();
                MotorPwmSetDuty(0, 0);
                MotorEnabled = true;
                break;
            case MotorMsg::DisableMotors:
                MotorPwmCoast();
                lvPid.Reset();
                avPid.Reset();
                MotorEnabled = false;
                break;
            case MotorMsg::Opengate:
            	Servopwm=23000;
            	ServoPwmSetDuty(Servopwm);
            	break;
            case MotorMsg::Closegate:
            	Servopwm=44800;
            	ServoPwmSetDuty(Servopwm);
            	break;
            case MotorMsg::EnablePwmtest:
            	PwmtestEnable = true;
            	break;
            case MotorMsg::DisablePwmtest:
            	PwmtestEnable = false;
            	break;
            case MotorMsg::EnableUnit_rsp:
            	Unit_rspEnable= true;
            	break;
            case MotorMsg::DisableUnit_rsp:
            	Unit_rspEnable= false;
            	break;
            default:
                break;
            }
        }
    }
}

void Init()
{
    Task_Params tskParams;

    MbCmd = Mailbox_create(4, 4, NULL, NULL);
    if(MbCmd == NULL)
        System_abort("create TskMotor::MbCmd failed.\n");

    QMotor = new Queue<VelOmega>(800);

//    Error_init(&eb);
    Task_Params_init(&tskParams);
    tskParams.priority = tskPrio;
    tskParams.stackSize = tskStkSize;
    tsk = Task_create(task, &tskParams, NULL);

    if(tsk == NULL)
        System_abort("TskMotor failed.");

}

}
