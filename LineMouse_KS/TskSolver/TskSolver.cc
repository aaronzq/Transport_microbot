/*
 * solver.cc
 *
 *  Created on: Apl 11, 2017
 *      Author: wzq
 */

#include <xdc/std.h>
#include <xdc/runtime/System.h>
#include <stdio.h>
#include <string.h>
#include <math.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include "../physparams.h"

#include <inc/hw_sysctl.h>
#include <inc/hw_pwm.h>
#include <inc/hw_memmap.h>
#include <inc/hw_types.h>
#include <linemouse_chassis.h>
#include <ti/drivers/GPIO.h>

#include "TskSolver.h"
#include "../TskMotor/TskMotor.h"
#include "../TskAction/TskAction.h"
#include "../TskTop/DbgUart.h"

namespace TskSolver
{

const int tskPrio = 13;
const int tskStkSize = 512;
Task_Handle tsk;
Mailbox_Handle MbCmd;

void task(UArg arg0, UArg arg1)
{
	StateMsg::MsgType msg;     //state message from TskAction
	TskAction::ActionMsg::MsgType actionMsg;  //actionMsg to TskAction
	TskMotor::MotorMsg::MsgType motMsg;

	actionMsg = TskAction::ActionMsg::Go_for_ball;
	Mailbox_post(TskAction::MbCmd, &actionMsg, BIOS_NO_WAIT);

	while(true)
	{
		if(Mailbox_pend(MbCmd,&msg,BIOS_WAIT_FOREVER))
		{
			switch(msg)
			{
			case StateMsg::Destination_no_ball:
				actionMsg = TskAction::ActionMsg::wait4ball;
				Mailbox_post(TskAction::MbCmd, &actionMsg, BIOS_NO_WAIT);
				break;
			case StateMsg::Destination_with_ball:
				actionMsg = TskAction::ActionMsg::take_back_ball;
				Mailbox_post(TskAction::MbCmd, &actionMsg, BIOS_NO_WAIT);
				break;
			case StateMsg::Origin_with_ball:
				motMsg = TskMotor::MotorMsg::Opengate;
				Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
				actionMsg = TskAction::ActionMsg::lay_down_ball;
				Mailbox_post(TskAction::MbCmd, &actionMsg, BIOS_NO_WAIT);
				break;
			case StateMsg::Origin_no_ball:
				motMsg = TskMotor::MotorMsg::Closegate;
				Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
				actionMsg = TskAction::ActionMsg::Go_for_ball;
				Mailbox_post(TskAction::MbCmd, &actionMsg, BIOS_NO_WAIT);
				break;
			default:break;
			}
		}
	}

}

void Init()
{

    MbCmd = Mailbox_create(4, 4, NULL, NULL);   //4 Bytes * 4
    if(MbCmd == NULL)
        System_abort("create TskMotor::MbCmd failed.\n");

    Task_Params tskParams;
//    Error_init(&eb);
    Task_Params_init(&tskParams);
    tskParams.priority = tskPrio;
    tskParams.stackSize = tskStkSize;
    tsk = Task_create(task, &tskParams, NULL);

    if(tsk == NULL)
        System_abort("TskMotor failed.");

}

}
