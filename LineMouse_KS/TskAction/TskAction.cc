 /*
 * TskAction.cc
 *
 *  Created on: Nov 5th, 2016
 *      Author: wzq
 */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Timestamp.h>

#include <inc/hw_sysctl.h>
#include <inc/hw_pwm.h>
#include <inc/hw_qei.h>
#include <inc/hw_gpio.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <linemouse_chassis.h>
#include <ti/drivers/GPIO.h>
#include <math.h>


#include "TskAction.h"
#include "../TskMotor/WheelEnc.h"
#include "../Queue/Queue.h"
#include "../TskMotor/TskMotor.h"
#include "../TskIr/TskIr.h"
#include "../TskTop/TskTop.h"
#include "../TskTop/DbgUart.h"
#include "../TskSolver/TskSolver.h"
#include <string.h>
#include <stdio.h>


#define YawCorrection

namespace TskAction
{

const int tskPrio = 14;
const int tskStkSize = 1560;
Task_Params tskParams;
Task_Handle tsk;
//Error_Block eb;

Mailbox_Handle MbCmd;

TskMotor::VelOmega desiresrc(0.0f,0.0f);
const float OmgadjP = -10.5f;

inline void Wait4queue()
{
	while(TskMotor::QMotor->Len()>1)
	{
		Semaphore_pend(SemActTick,2);
	}
}

inline void ClearQueue()
{
	TskMotor::QMotor->Clear();
}

void Enqueue_acc(float vel_0,float vel_1,float accpath,float unipath)
{
	int i = 1;
	float time_ac = accpath/(0.5f*(vel_0+vel_1));
	float ac = (vel_1-vel_0)/time_ac;
	int time_count_ac = (int)(time_ac*1000.f);
	int time_count_uni = (int)((unipath/vel_1)*1000.f);
	for (i=1;i<=(time_count_ac+time_count_uni);i++)
	{
		if(i<=time_count_ac)
		{
			desiresrc.Omega = 0.f;
			desiresrc.Velocity = vel_0 + ac*i*0.001f;
		}
		else
		{
			desiresrc.Omega = 0.f;
			desiresrc.Velocity = vel_1;
		}
		TskMotor::QMotor->En(desiresrc);
	}
}

void Enqueue_uni(float vel,float unipath)
{
	int i=1;

	int time_count_uni = (int)((unipath/vel)*1000.f);
	for (i=1;i<=time_count_uni;i++)
	{
		desiresrc.Omega = 0.f;
		desiresrc.Velocity = vel;
		TskMotor::QMotor->En(desiresrc);
	}
}

void Enqueue_turn(float vel,bool isleft)
{
	int i=0;
	float beta = 2.226423894674839f*vel*vel/(PP::TurnRadius)/(PP::TurnRadius);
	if(!isleft) beta = -beta;
	float time_beta = 0.839954983918006*PP::TurnRadius/vel;
	int time_count_turn = (int)(time_beta*1000.f);
	for (i=1;i<=time_count_turn;i++)
	{
		if(isleft) desiresrc.Omega = beta*i*0.001f*PP::TurnCompensateCoffL;
		else desiresrc.Omega = beta*i*0.001f*PP::TurnCompensateCoffR;
		desiresrc.Velocity = vel;
		TskMotor::QMotor->En(desiresrc);
	}
	for (i=1;i<=time_count_turn;i++)
	{
		if(isleft) desiresrc.Omega = (beta*time_beta-beta*i*0.001f)*PP::TurnCompensateCoffL;
		else desiresrc.Omega = (beta*time_beta-beta*i*0.001f)*PP::TurnCompensateCoffR;
		desiresrc.Velocity = vel;
		TskMotor::QMotor->En(desiresrc);
	}
}

void Enqueue_back(float vel,bool isleft)
{
	int i=0;
	float beta = 0.603409094747437f*vel*vel/(PP::TurnBackRadius)/(PP::TurnBackRadius);
	if(!isleft) beta = -beta;
	float time_beta = 2.281754998052114*PP::TurnBackRadius/vel;
	int time_count_turn = (int)(time_beta*1000.f);
	for (i=1;i<=time_count_turn;i++)

	{
		desiresrc.Omega = beta*i*0.001f;
		desiresrc.Velocity = vel;
		TskMotor::QMotor->En(desiresrc);
	}
	for (i=1;i<=time_count_turn;i++)
	{
		desiresrc.Omega = beta*time_beta-beta*i*0.001f;
		desiresrc.Velocity = vel;
		TskMotor::QMotor->En(desiresrc);
	}
}

void Start(float vel, float acc_path, float uni_path, bool left_side)
{
	Enqueue_acc(0.f,vel,acc_path,uni_path);
	float ActStart=TskMotor::DistanceAcc;
	float ActDistance=0.f;
	while((ActDistance=TskMotor::DistanceAcc-ActStart)<(acc_path+uni_path))
	{
		if(!Semaphore_pend(SemActTick, 2))
			System_abort("pend SemActTick failed!\n");

#ifdef YawCorrection
		if(ActDistance>PP::start_yaw_begin && ActDistance <PP::start_yaw_end)
		{
			if(left_side)
			{
				TskMotor::OmgAdj = OmgadjP*TskIr::IrYawL;
			}
			else
			{
				TskMotor::OmgAdj = OmgadjP*TskIr::IrYawR;
			}
		}
		else
		{
			TskMotor::OmgAdj = 0.f;
		}
#endif

	}

}

void Forward(float vel, float uni_path, bool left_side, bool border_detect, bool backturn_trigger)
{
	Enqueue_uni(vel,uni_path);
	float ActStart=TskMotor::DistanceAcc;
	float ActDistance=0.f;
	while((ActDistance=TskMotor::DistanceAcc-ActStart)<uni_path)
	{
		if(!Semaphore_pend(SemActTick, 2))
			System_abort("pend SemActTick failed!\n");

#ifdef YawCorrection
		if(ActDistance>PP::forward_yaw_begin && ActDistance <PP::forward_yaw_end)
		{
			if(left_side)
			{
				TskMotor::OmgAdj = OmgadjP*TskIr::IrYawL;
			}
			else
			{
				TskMotor::OmgAdj = OmgadjP*TskIr::IrYawR;
			}
		}
		else
		{
			TskMotor::OmgAdj = 0.f;
		}
#endif

		if(border_detect)
		{
			if((ActDistance>PP::forward_border_detect_begin)&&(TskIr::IrBorder==true))
			{
				ClearQueue();
				TskMotor::OmgAdj = 0.f;
				break;
			}
		}

		if(backturn_trigger)
		{
			if((ActDistance>PP::forward_backturn_trigger_begin)&&(TskIr::IrNoLine==true))
			{
				ClearQueue();
				break;
			}
		}

	}
}

void ForwardBack(float vel, float uni_path, bool left_side, bool border_detect, bool backturn_trigger)
{
	Enqueue_uni(vel,uni_path);
	float ActStart=TskMotor::DistanceAcc;
	float ActDistance=0.f;
	while((ActDistance=TskMotor::DistanceAcc-ActStart)<uni_path)
	{
		if(!Semaphore_pend(SemActTick, 2))
			System_abort("pend SemActTick failed!\n");


		if(border_detect)
		{
			if((ActDistance>PP::forward_border_detect_begin)&&(TskIr::IrBorder==true))
			{
				ClearQueue();
				break;
			}
		}

		if(backturn_trigger)
		{
			if((ActDistance>PP::forward_backturn_trigger_begin)&&(TskIr::IrNoLine==true))
			{
				ClearQueue();
				break;
			}
		}

	}
}

void Turn90(float vel, float uni_path, bool LeftTurn, bool left_side)
{
	Enqueue_turn(vel,LeftTurn);
	Wait4queue();
	Enqueue_uni(vel,uni_path);

	float ActStart=TskMotor::DistanceAcc;
	float ActDistance=0.f;
	while((ActDistance=TskMotor::DistanceAcc-ActStart)<uni_path)
	{
		if(!Semaphore_pend(SemActTick, 2))
			System_abort("pend SemActTick failed!\n");

//#ifdef YawCorrection
//		if(ActDistance>PP::forward_yaw_begin && ActDistance <PP::forward_yaw_end)
//		{
//			if(left_side)
//			{
//				TskMotor::OmgAdj = OmgadjP*TskIr::IrYawL;
//			}
//			else
//			{
//				TskMotor::OmgAdj = OmgadjP*TskIr::IrYawR;
//			}
//		}
//		else
//		{
//			TskMotor::OmgAdj = 0.f;
//		}
//#endif

	}
}


void Turn180(float vel, float uni_path, bool LeftTurn, bool left_side)
{
	Enqueue_turn(vel,LeftTurn);
//	Enqueue_back(vel,LeftTurn);
	Wait4queue();
	Enqueue_uni(vel,0.6f);
	Wait4queue();
	Enqueue_turn(vel,LeftTurn);
//	Enqueue_uni(vel,uni_path);

	float ActStart=TskMotor::DistanceAcc;
	float ActDistance=0.f;
	while((ActDistance=TskMotor::DistanceAcc-ActStart)<uni_path)
	{
		if(!Semaphore_pend(SemActTick, 2))
			System_abort("pend SemActTick failed!\n");

//#ifdef YawCorrection
//		if(ActDistance>PP::forward_yaw_begin && ActDistance <PP::forward_yaw_end)
//		{
//			if(left_side)
//			{
//				TskMotor::OmgAdj = OmgadjP*TskIr::IrYawL;
//			}
//			else
//			{
//				TskMotor::OmgAdj = OmgadjP*TskIr::IrYawR;
//			}
//		}
//		else
//		{
//			TskMotor::OmgAdj = 0.f;
//		}
//#endif

	}
}

void Stop(float vel,float acc_path)
{
	Enqueue_acc(vel,0.f,acc_path,0.f);
	desiresrc.Omega=0.f;
	desiresrc.Velocity=0.f;
	TskMotor::QMotor->En(desiresrc);
}

void Back(float turn_time)
{
	int i=1;

	float alpha = 4.f*3.1415926f/(turn_time*turn_time);
	float omega_max = 2.f*3.1415926f/(turn_time);
	int time_count_turnback = (int)(turn_time*1000.f);

	for (i=1;i<=time_count_turnback;i++)
	{
		if(i<=0.5*time_count_turnback)
		{
			desiresrc.Omega = alpha*i*0.001f*1.f;
			desiresrc.Velocity = 0.f;
		}
		else
		{
			desiresrc.Omega = (omega_max - alpha*(i-0.5*time_count_turnback)*0.001f)*1.f;
			desiresrc.Velocity = 0.f;
		}
		TskMotor::QMotor->En(desiresrc);
	}
	desiresrc.Omega = 0.f;
	desiresrc.Velocity = 0.f;
	TskMotor::QMotor->En(desiresrc);
	desiresrc.Omega = 0.f;
	desiresrc.Velocity = 0.f;
	TskMotor::QMotor->En(desiresrc);
}


void ActionSelect(ActType::Action *Seq, unsigned char SeqNum)
{
	TskTop::LedColor::Type Indicator=TskTop::LedColor::Red;

	for(int i=0;i<SeqNum;i++)
	{
		switch(Seq[i])
		{
		case ActType::start_left_side:
			Start(PP::run_vel, PP::start_acc_path, PP::start_uni_path, true);
			break;
		case ActType::start_right_side:
			Start(PP::run_vel, PP::start_acc_path, PP::start_uni_path, false);
			break;
		case ActType::forward_left_side:
			Forward(PP::run_vel, PP::forward_uni_path, true, false, false);
			break;
		case ActType::forward_left_side_border:
			Forward(PP::run_vel, PP::forward_uni_path, true, true, false);
			break;
		case ActType::forward_left_side_backturn:
			Forward(PP::run_vel, PP::forward_uni_path, true, false, true);
			break;
		case ActType::forward_right_side:
			Forward(PP::run_vel, PP::forward_uni_path, false, false, false);
			break;
		case ActType::forward_right_side_border:
			Forward(PP::run_vel, PP::forward_uni_path, false, true, false);
			break;
		case ActType::forward_right_side_backturn:
			Forward(PP::run_vel, PP::forward_uni_path, false, false, true);
			break;
		case ActType::left_turn90:
			Turn90(PP::run_vel, PP::turn90_uni_path, true, false);
			break;
		case ActType::right_turn90:
			Turn90(PP::run_vel, PP::turn90_uni_path, false, true);
			break;
		case ActType::left_turn180:
			Turn180(PP::run_vel, PP::turn180_uni_path, true, true);
			break;
		case ActType::right_turn180:
			Turn180(PP::run_vel, PP::turn180_uni_path, false, false);
			break;
		case ActType::stop:
			Stop(PP::run_vel,PP::stop_path);
			break;
		case ActType::back:
			Back(PP::back_time);
			break;
		case ActType::forward_right_side_back:
			ForwardBack(PP::run_vel, PP::forward_uni_path, false, false, false);
			break;
		default:break;

		}
		TskTop::SetLeds(Indicator,Indicator);
		if(Indicator==TskTop::LedColor::Blue)
		{
		    Indicator = TskTop::LedColor::Red;
		}
		else
		{
		    Indicator=(TskTop::LedColor::Type)(Indicator<<1);
		}
		Wait4queue();
	}

}

void Go_for_ball()
{

    ActType::Action Sequence[15];
    Sequence[0]=ActType::start_left_side;
    Sequence[1]=ActType::forward_left_side;
    Sequence[2]=ActType::forward_left_side_border;
    Sequence[3]=ActType::right_turn90;
    Sequence[4]=ActType::forward_left_side;
    Sequence[5]=ActType::forward_left_side;
    Sequence[6]=ActType::forward_left_side;
    Sequence[7]=ActType::forward_left_side;
    Sequence[8]=ActType::forward_left_side_border;
    Sequence[9]=ActType::right_turn90;
    Sequence[10]=ActType::forward_left_side;
    Sequence[11]=ActType::forward_left_side;
    Sequence[12]=ActType::forward_left_side_border;
    Sequence[13]=ActType::stop;
    Sequence[14]=ActType::back;


    ActionSelect(Sequence,15);

}

void wait4ball()
{
	TskTop::LedColor::Type Indicator=TskTop::LedColor::Red;
	TskTop::SetLeds(Indicator,Indicator);
	desiresrc.Omega = 0.f;
	desiresrc.Velocity = 0.f;
	TskMotor::QMotor->En(desiresrc);
	while(true)
	{
		Task_sleep(1000);
		if(TskIr::IrBall == true)
		{
			Indicator = TskTop::LedColor::Green;
			TskTop::SetLeds(Indicator,Indicator);
			Task_sleep(1000);
			break;
		}
	}

}

void take_back_ball()
{
    ActType::Action Sequence[15];
    Sequence[0]=ActType::start_right_side;
    Sequence[1]=ActType::forward_right_side;
    Sequence[2]=ActType::forward_right_side_border;
    Sequence[3]=ActType::left_turn90;
    Sequence[4]=ActType::forward_right_side;
    Sequence[5]=ActType::forward_right_side;
    Sequence[6]=ActType::forward_right_side;
    Sequence[7]=ActType::forward_right_side;
    Sequence[8]=ActType::forward_right_side_border;
    Sequence[9]=ActType::left_turn90;
    Sequence[10]=ActType::forward_right_side;
    Sequence[11]=ActType::forward_right_side;
    Sequence[12]=ActType::forward_right_side_border;
    Sequence[13]=ActType::stop;
    Sequence[14]=ActType::back;

    ActionSelect(Sequence,15);

}

void lay_down_ball()
{
	TskTop::LedColor::Type Indicator=TskTop::LedColor::Dark;
	desiresrc.Omega = 0.f;
	desiresrc.Velocity = 0.f;
	TskMotor::QMotor->En(desiresrc);
	while(TskIr::IrBall == true)
	{
		Task_sleep(1000);
	}
	TskTop::SetLeds(Indicator,Indicator);
}

void idle()
{
	Task_sleep(1);
}


void task(UArg arg0, UArg arg1)
{
	TskSolver::StateMsg::MsgType stateMsg;
    ActionMsg::MsgType msg;
    while(true)
    {
        if(Mailbox_pend(MbCmd, &msg, BIOS_WAIT_FOREVER))
        {
            switch(msg & 0x0000FFFF)
            {
            case ActionMsg::Go_for_ball:
                Go_for_ball();
                stateMsg = TskSolver::StateMsg::Destination_no_ball;
                Mailbox_post(TskSolver::MbCmd, &stateMsg, BIOS_NO_WAIT);
                break;
            case ActionMsg::wait4ball:
            	wait4ball();
                stateMsg = TskSolver::StateMsg::Destination_with_ball;
                Mailbox_post(TskSolver::MbCmd, &stateMsg, BIOS_NO_WAIT);
                break;
            case ActionMsg::take_back_ball:
                take_back_ball();
                stateMsg = TskSolver::StateMsg::Origin_with_ball;
                Mailbox_post(TskSolver::MbCmd, &stateMsg, BIOS_NO_WAIT);
                break;
            case ActionMsg::lay_down_ball:
            	lay_down_ball();
                stateMsg = TskSolver::StateMsg::Origin_no_ball;
                Mailbox_post(TskSolver::MbCmd, &stateMsg, BIOS_NO_WAIT);
            	break;
            case ActionMsg::Idle:
            	idle();
            	break;
            default:break;
            }
        }
    }

}

void Init()
{
//    Error_init(&eb);
    Task_Params_init(&tskParams);

    MbCmd = Mailbox_create(4, 4, NULL, NULL);
    if(MbCmd == NULL)
        System_abort("create TskAction::MbCmd failed.\n");

    tskParams.priority = tskPrio;
    tskParams.stackSize = tskStkSize;
    tsk = Task_create(task, &tskParams, NULL);
    if(tsk == NULL)
    {
        System_abort("Task Action failed");
    }
}

}

