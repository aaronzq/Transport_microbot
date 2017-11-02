 /*
 * top.cc
 *
 *  Created on: Jul 21, 2016
 *      Author: loywong
 *  Edited on : June  , 2017
 *  	Author: aaronzq
 */
#include <ti/sysbios/knl/Task.h>
#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <xdc/runtime/Error.h>
#include <xdc/runtime/System.h>
#include <ti/sysbios/BIOS.h>
#include <xdc/runtime/Timestamp.h>
#include <ti/sysbios/knl/Semaphore.h>

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

#include "TskTop.h"
#include "../TskMotor/WheelEnc.h"
#include "../TskMotor/MotorPwm.h"
#include "../TskMotor/TskMotor.h"
#include "../TskIr/TskIr.h"
#include "../TskAction/TskAction.h"
#include "../TskSolver/TskSolver.h"

#include "DbgUart.h"
#include <string.h>
#include <stdio.h>



namespace TskTop
{

const int tskPrio = 4;
const int tskStkSize = 2560; //1024

Task_Params tskParams;
Task_Handle tsk;
//Error_Block eb;

char dbgStr[80];
char database[1000][10];
volatile int secread=0;
volatile bool Lvtest = false;

volatile MouseMode::ModeType Mode;
volatile bool Execute=false;

void encImuMonitor()
{
    int i = 0;

    TskMotor::MotorMsg::MsgType motMsg;
    Task_sleep(1000);
    motMsg = TskMotor::MotorMsg::DisableMotors;
    Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
    Task_sleep(1000);

    while(true)
    {
        if(!(i++ & 0xf))
        {
            DbgUartPutLine( "\n EncLVel\t EncRVel\t EncVel\t DisAcc\n", true);
        }
        sprintf(dbgStr, "%7.3f\t%7.3f\t%7.4f\t%8.3f\n",TskMotor::EncLVel,TskMotor::EncRVel,TskMotor::LV,TskMotor::DistanceAcc);
        DbgUartPutLine(dbgStr, true);
    	Task_sleep(500);
    }
}

void irMonitor()
{
    int i = 0;

    TskMotor::MotorMsg::MsgType motMsg;

    Task_sleep(1000);

    motMsg = TskMotor::MotorMsg::DisableMotors;
    Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);

    while(true)
    {
        if(!(i++ & 0xf))
        {
            DbgUartPutLine( "\n LWay:b\t Left:b\tCentr:b\tRight:b\t RWay:b\t Middle:b\n", true);
        }
        sprintf(dbgStr, "%5d:%1d\t%5d:%1d\t%5d:%1d\t%5d:%1d\t%5d:%1d\t%5d:%1d\n",
                TskIr::IrInts.lway, TskIr::IrBins.lway,
                TskIr::IrInts.left, TskIr::IrBins.left,
                TskIr::IrInts.center, TskIr::IrBins.center,
                TskIr::IrInts.right, TskIr::IrBins.right,
                TskIr::IrInts.rway, TskIr::IrBins.rway,
				TskIr::IrInts.middle, TskIr::IrBins.middle
        );
        DbgUartPutLine(dbgStr, true);
        Task_sleep(500);
    }

}

void pwmtest()
{
	TskMotor::MotorMsg::MsgType motMsg;

	motMsg = TskMotor::MotorMsg::EnableMotors;
	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
	motMsg = TskMotor::MotorMsg::EnablePwmtest;
	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
	Task_sleep(100);

	while(true)
	{
		if(TskMotor::ServoPwmTest == 44800) TskMotor::ServoPwmTest = 23000;
		else TskMotor::ServoPwmTest= 44800;
		Task_sleep(1000);
	}

}

void unit_rsptest()
{
	TskMotor::MotorMsg::MsgType motMsg;

	motMsg = TskMotor::MotorMsg::EnableMotors;
	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
	motMsg = TskMotor::MotorMsg::EnableUnit_rsp;
	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
	Task_sleep(100);

    while(true)
    {
		if(!Semaphore_pend(SemActTick, 2))
			System_abort("pend SemActTick failed!\n");

		if(secread<1000)
		{
			if(Lvtest)
			{
				sprintf(database[secread], "%7.4f\n",TskMotor::EncVel);
			}
			else
			{
				sprintf(database[secread], "%7.4f\n",TskMotor::AV);
			}
			secread++;
		}
		else if(secread == 1000)
		{
			secread++;
//			DbgUartPutLine( "LV Unit_response result: \n", true);
			DbgUartPutLine( "AV Unit_response result: \n", true);
			for(int count=0;count<1000;count++)
			{
				DbgUartPutLine(database[count], true);
			}
		}
		else
		{

		}
    }
}

void run_init()
{
	TskMotor::MotorMsg::MsgType motMsg;
	motMsg = TskMotor::MotorMsg::DisablePwmtest;
	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
	motMsg = TskMotor::MotorMsg::DisableUnit_rsp;
	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
	motMsg = TskMotor::MotorMsg::EnableMotors;
	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
	Task_sleep(50);
    TskAction::Init();
    Task_sleep(50);
    TskSolver::Init();
    while(true)
    {
    	Task_sleep(50);
    }
}

void SetLeds(LedColor::Type led0, LedColor::Type led1)
{
    unsigned char rgb0, rgb1;
    rgb0 = (unsigned char)led0;
    rgb1 = (unsigned char)led1;
    GPIO_write(DBMOUSE_LED0_R, (rgb0 & 0x1) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED0_G, (rgb0 & 0x2) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED0_B, (rgb0 & 0x4) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED1_R, (rgb1 & 0x1) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED1_G, (rgb1 & 0x2) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED1_B, (rgb1 & 0x4) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
}

void SetLedL(LedColor::Type led)
{
	unsigned char rgb;
	rgb = (unsigned char)led;
    GPIO_write(DBMOUSE_LED0_R, (rgb & 0x1) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED0_G, (rgb & 0x2) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED0_B, (rgb & 0x4) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
}

void SetLedR(LedColor::Type led)
{
	unsigned char rgb;
	rgb = (unsigned char)led;
    GPIO_write(DBMOUSE_LED1_R, (rgb & 0x1) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED1_G, (rgb & 0x2) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
    GPIO_write(DBMOUSE_LED1_B, (rgb & 0x4) ? DBMOUSE_LED_ON : DBMOUSE_LED_OFF);
}


void modeName(MouseMode::ModeType mode, char *str)
{
    switch(mode)
    {
    case MouseMode::Idle:
        sprintf(str, "Mode Idle.\n");
        break;
    case MouseMode::EncImuMonitor:
        sprintf(str, "Mode Enc & Imu Monitor.\n\tRotate right wheel to start\n");
        break;
    case MouseMode::IrMonitor:
        sprintf(str, "Mode Ir Monitor.\n\tRotate right wheel to start\n");
        break;
    case MouseMode::Pwmtest:
        sprintf(str, "Mode Pwm Test. \n\tRotate right wheel to start\n");
        break;
    case MouseMode::Unit_rsptest:
        sprintf(str, "Mode Unit Response Test.\n\t Rotate right wheel to start\n");
        break;
    case MouseMode::Run:
        sprintf(str, "Mode Run.\n\t Rotate right wheel to start\n");
        break;
    case MouseMode::Undef0:
        sprintf(str, "Mode Undef0.\n");
        break;
    case MouseMode::Undef1:
        sprintf(str, "Mode Undef1.\n");
        break;
    default:
        sprintf(str, "Undefined mode.\n");
        break;
    }
}

inline void dbgPutModeName(MouseMode::ModeType mode)
{
    modeName(mode, dbgStr);
    DbgUartPutLine(dbgStr, true);
}


void task(UArg arg0, UArg arg1)
{

//    TskIr::IrMsg::MsgType irMsg;
	TskMotor::MotorMsg::MsgType motMsg;


    InitDbgUart();

    SetLeds(LedColor::Red, LedColor::Red);
    Task_sleep(1000);
    SetLeds(LedColor::Green, LedColor::Green);
    Task_sleep(1000);
    SetLeds(LedColor::Blue, LedColor::Blue);
    Task_sleep(1000);

    DbgUartPutLine("Hello from dbmouse!\n", true);

    TskMotor::Init();
    TskIr::Init();
//    irMsg = TskIr::IrMsg::EnableEmitt;
//    Mailbox_post(TskIr::MbCmd, &irMsg, BIOS_NO_WAIT);

    Task_sleep(500);

//---------------to roll wheel to change mode------------------------
    DbgUartPutLine("Roll left wheel to change mode...\n", true);

    MouseMode::ModeType lastMode = Mode = MouseMode::Idle;
    SetLeds((LedColor::Type)Mode, (LedColor::Type)Mode);
    dbgPutModeName(Mode);

    TskMotor::EncLAct = false;
    TskMotor::EncRAct = false;
//-------------------------------------------------------------------

//	motMsg = TskMotor::MotorMsg::DisablePwmtest;
//	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
//	motMsg = TskMotor::MotorMsg::DisableUnit_rsp;
//	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
//	motMsg = TskMotor::MotorMsg::EnableMotors;
//	Mailbox_post(TskMotor::MbCmd, &motMsg, BIOS_NO_WAIT);
//	Task_sleep(50);
//    TskAction::Init();
//    Task_sleep(50);
//    TskSolver::Init();

    while(true)
    {

        Mode = (MouseMode::ModeType)(lroundf(TskMotor::DistanceAccL * 100.f) & 15);
        if(Mode != lastMode)
        {
        	SetLedL((LedColor::Type)Mode);
            dbgPutModeName(Mode);
        }
        lastMode = Mode;

        Execute = ((lroundf((TskMotor::DistanceAccR * 100.f)) & 15) > 7);
        if(Execute) SetLedR(LedColor::Green);
        else SetLedR(LedColor::Red);

        if(Execute)
        {
        	Task_sleep(1000);
            switch(Mode)
            {
            case MouseMode::Idle:
                break;
            case MouseMode::EncImuMonitor:
                encImuMonitor();
                break;
            case MouseMode::IrMonitor:
                irMonitor();
                break;
            case MouseMode::Pwmtest:
            	pwmtest();
            	break;
            case MouseMode::Unit_rsptest:
            	unit_rsptest();
            	break;
            case MouseMode::Run:
            	run_init();
            	break;
            default:
                break;
            }
        }

    	Task_sleep(30);
    }
}

void Init()
{
//    Error_init(&eb);
    Task_Params_init(&tskParams);
    tskParams.priority = tskPrio;
    tskParams.stackSize = tskStkSize;
    tsk = Task_create((Task_FuncPtr)task, &tskParams, NULL);
    if(tsk == NULL)
    {
        System_abort("Task Top failed");
    }
}

}


