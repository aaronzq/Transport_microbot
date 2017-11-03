/*
 * TskIr.cc
 *
 *  Created on: Aug 13, 2016
 *      Author: loywong
 */

#include <xdc/runtime/System.h>

#include <ti/sysbios/BIOS.h>
#include <ti/sysbios/knl/Mailbox.h>
#include <ti/sysbios/knl/Semaphore.h>
#include <ti/sysbios/knl/Task.h>

#include <ti/sysbios/hal/Hwi.h>
#include <ti/sysbios/hal/Timer.h>

#include <ti/drivers/GPIO.h>

#include <inc/hw_types.h>
#include <inc/hw_ints.h>
#include <inc/hw_memmap.h>
#include <inc/hw_sysctl.h>
#include <inc/hw_gpio.h>
#include <inc/hw_adc.h>
#include <linemouse_chassis.h>
#include "TskIr.h"
#include "../TskMotor/TskMotor.h"
#include "../TskMotor/WheelEnc.h"
#include "../physparams.h"
//#include "../TskTop/DbgUart.h"
//#include "../TskTop/TskTop.h"
#include "../Flash/Flash.h"

#include "../TskSolver/TskSolver.h"

#include <math.h>
//#include <stdio.h>

namespace TskIr
{

const int tskPrio = 10;
const int tskStkSize = 1024;
Task_Handle tsk;

Semaphore_Handle SemIrAdcFinish;

Mailbox_Handle MbCmd;

Timer_Handle irTimer;
Timer_Params irTimerParams;
const int irTimerId = 5;//Timer_ANY;

volatile IrIntensity IrInts = {{1, 1, 1, 1, 1, 1}};
volatile IrBinaryThs IrBinThs;
volatile IrBinaries IrBins;
volatile float IrYawL,IrYawR;
volatile bool IrBorder;
volatile bool IrBall;
volatile bool IrNoLine;

char dbgStr[48];

//          \ | /
//      -           -
//   lway   l c r   rway
const char *IrChNames[] = {
        "lway ",
        "left ",
        "centr",
        "right",
        "rway ",
		"middle"
};

volatile int ch = 0;
const int adcCh[6] = {0x5, 0x6, 0x7, 0x8, 0x9, 0x0};

inline void adcStart(int ch)
{
    HWREG(ADC0_BASE | ADC_O_SSMUX3) = ch;
    HWREG(ADC0_BASE | ADC_O_PSSI)   = 0x00000008;   // initiate ss3
}

inline unsigned short adcReadCode()
{
    return HWREG(ADC0_BASE | ADC_O_SSFIFO3);
}

void irTimerHooker(UArg arg)
{
    if(ch > 0)
    {   // 1, 2, 3, 4, 5, 6
        IrInts.ch[ch - 1] = adcReadCode();
    }
    if(ch < 6)
    {   // 0, 1, 2, 3, 4, 5
        adcStart(adcCh[ch]);
    }

    if(ch < 6)
    {
        ch++;
        Timer_start(irTimer);
    }
    else
    {
        ch = 0;
        // inform tskIr
        Semaphore_post(SemIrAdcFinish);
    }
}

void irDetStart()
{
    ch = 0;
//    irEmitterNo = DBMOUSE_IR_FL;
//    GPIO_write(irEmitterNo, irEmitEnable? 1 : 0);
//    Timer_setPeriodMicroSecs(irTimer, irEmitTime);
    Timer_start(irTimer);
}

void irDetInit()
{
    Semaphore_Params semParams;
    Semaphore_Params_init(&semParams);
    semParams.mode = Semaphore_Mode_BINARY;
    SemIrAdcFinish = Semaphore_create(0, &semParams, NULL);
    if(SemIrAdcFinish == NULL)
        System_abort("create SemIrAdcFinish failed.\n");

    Timer_Params_init(&irTimerParams);
    irTimerParams.period = 5;   // unit: micro sec
    irTimerParams.runMode = Timer_RunMode_ONESHOT;//Timer_RunMode_DYNAMIC;
    irTimerParams.startMode = Timer_StartMode_USER;
    irTimer = Timer_create(irTimerId, irTimerHooker, &irTimerParams, NULL);
//    Hwi_enableInterrupt(INT_QEI1_TM4C123);
    if(irTimer == NULL)
        System_abort("create irTimer failed!\n");

//    Hwi_setPriority(108, 192);

//    Timer_Status sts[6];
//    int num = Timer_getNumTimers();
//    for(int i = 0; i < num ; i++)
//    {
//        sts[i] = Timer_getStatus(i);
//    }

    // init ADC
    HWREG(SYSCTL_RCGCADC) = 0x01;   // enable ADC0

    HWREG(GPIO_PORTD_BASE | GPIO_O_AFSEL) |=  0x00000004;    // PD2
    HWREG(GPIO_PORTD_BASE | GPIO_O_DEN)   &= ~0x00000004;    // PD2
    HWREG(GPIO_PORTD_BASE | GPIO_O_AMSEL) |=  0x00000004;    // PD2, AIN5

    HWREG(GPIO_PORTD_BASE | GPIO_O_AFSEL) |=  0x00000002;    // PD1
    HWREG(GPIO_PORTD_BASE | GPIO_O_DEN)   &= ~0x00000002;    // PD1
    HWREG(GPIO_PORTD_BASE | GPIO_O_AMSEL) |=  0x00000002;    // PD1, AIN6

    HWREG(GPIO_PORTD_BASE | GPIO_O_AFSEL) |=  0x00000001;    // PD0
    HWREG(GPIO_PORTD_BASE | GPIO_O_DEN)   &= ~0x00000001;    // PD0
    HWREG(GPIO_PORTD_BASE | GPIO_O_AMSEL) |=  0x00000001;    // PD0, AIN7

    HWREG(GPIO_PORTE_BASE | GPIO_O_AFSEL) |=  0x00000020;    // PE5
    HWREG(GPIO_PORTE_BASE | GPIO_O_DEN)   &= ~0x00000020;    // PE5
    HWREG(GPIO_PORTE_BASE | GPIO_O_AMSEL) |=  0x00000020;    // PE5, AIN8

    HWREG(GPIO_PORTE_BASE | GPIO_O_AFSEL) |=  0x00000010;    // PE4
    HWREG(GPIO_PORTE_BASE | GPIO_O_DEN)   &= ~0x00000010;    // PE4
    HWREG(GPIO_PORTE_BASE | GPIO_O_AMSEL) |=  0x00000010;    // PE4, AIN9

    HWREG(GPIO_PORTE_BASE | GPIO_O_AFSEL) |=  0x00000008;    // PE3
    HWREG(GPIO_PORTE_BASE | GPIO_O_DEN)   &= ~0x00000008;    // PE3
    HWREG(GPIO_PORTE_BASE | GPIO_O_AMSEL) |=  0x00000008;    // PE3, AIN0

    HWREG(ADC0_BASE | ADC_O_ACTSS)  = 0x00000000;   // disable all ss
    HWREG(ADC0_BASE | ADC_O_EMUX)   = 0x00000000;   // initiate by sw
    HWREG(ADC0_BASE | ADC_O_SSMUX3) = 0x00000005;   // ss3 sel ain5 1st
    HWREG(ADC0_BASE | ADC_O_SSCTL3) = 0x00000002;   // ss3 1st smp as end
    HWREG(ADC0_BASE | ADC_O_ACTSS)  = 0x00000008;   // enable ss3
}



float irCalcYawL()
{
    // TODO:
	float Yawdev=0;
	if(IrBins.ch[0]==0)
	{
		if(IrBins.ch[1]==0) Yawdev=PP::YawUnit;
		else Yawdev=0.f;
	}
	else
	{
		if(IrBins.ch[1]==0) Yawdev=0.f;   //this condition is actually impossible
		else Yawdev= -PP::YawUnit;
	}
    return Yawdev;
}

float irCalcYawR()
{
	float Yawdev=0;
	if(IrBins.ch[4]==0)
	{
		if(IrBins.ch[3]==0) Yawdev= -PP::YawUnit;
		else Yawdev=0.f;
	}
	else
	{
		if(IrBins.ch[3]==0) Yawdev=0.f;   //this condition is actually impossible
		else Yawdev= PP::YawUnit;
	}
    return Yawdev;
}

bool irBorder()
{
	return (IrBins.ch[2] == 0);
}

bool irNoLine()
{
	return ((IrBins.ch[0]==1)&&(IrBins.ch[1]==1)&&(IrBins.ch[2]==1)&&(IrBins.ch[3]==1)&&(IrBins.ch[4]==1));

}

bool irBall()
{
	return (IrBins.ch[5] == 0);
}

void irCalcs()
{
    int i;
    // dist binary
    for(i = 0; i < 6; i++)
    {
        if(IrBins.ch[i])    // black line
        {
            if(IrInts.ch[i] < IrBinThs.ch[i].ThLo)
                IrBins.ch[i] = 0;
        }
        else
        {
            if(IrInts.ch[i] > IrBinThs.ch[i].ThHi)
                IrBins.ch[i] = 1;
        }
    }
    // side yaw
    IrYawL = irCalcYawL();
    IrYawR = irCalcYawR();
    // border ahead
    IrBorder = irBorder();
    // before turn back
    IrNoLine = irNoLine();
    // ball
    IrBall = irBall();

}

void task(UArg arg0, UArg arg1)
{
//    IrMsg::MsgType msg;

    IrBins.ch[0] = 0; IrBinThs.ch[0].ThLo = 3000; IrBinThs.ch[0].ThHi = 3500;
    IrBins.ch[1] = 1; IrBinThs.ch[1].ThLo = 3000; IrBinThs.ch[1].ThHi = 3500;
    IrBins.ch[2] = 1; IrBinThs.ch[2].ThLo = 3200; IrBinThs.ch[2].ThHi = 3500;
    IrBins.ch[3] = 1; IrBinThs.ch[3].ThLo = 3000; IrBinThs.ch[3].ThHi = 3500;
    IrBins.ch[4] = 1; IrBinThs.ch[4].ThLo = 3000; IrBinThs.ch[4].ThHi = 3500;
    IrBins.ch[5] = 0; IrBinThs.ch[5].ThLo = 3200; IrBinThs.ch[5].ThHi = 3600;

    irDetInit();

    while(true)
    {
        if(!Semaphore_pend(SemIrTick, 2))
            System_abort("pend SemIrTick failed!\n");

        irDetStart();

        if(!Semaphore_pend(SemIrAdcFinish, 2))
            System_abort("pend SemIrAdcFinish failed!\n");

        // calculate bins
        irCalcs();

    }
}

void Init()
{
    Task_Params tskParams;

//    MbCmd = Mailbox_create(4, 4, NULL, NULL);
//    if(MbCmd == NULL)
//        System_abort("create TskIr::MbCmd failed.\n");

    Task_Params_init(&tskParams);
    tskParams.priority = tskPrio;
    tskParams.stackSize = tskStkSize;
    tsk = Task_create(task, &tskParams, NULL);
}

//// this func test ir touch frist,
//// if touched, it will wait until untouch,
//// otherwise, it return immediately.
//bool TestIrTouch(unsigned char chMask, int hTh, int lTh)
//{
//    if(     ((IrInts.fl > hTh) && (chMask & IrCh::FL)) ||
//            ((IrInts.fr > hTh) && (chMask & IrCh::FR)) ||
//            ((IrInts.sl > hTh) && (chMask & IrCh::SL)) ||
//            ((IrInts.sr > hTh) && (chMask & IrCh::SR))
//            )
//    {
//        while(true)
//        {
//            Task_sleep(50);
//            if(     ((IrInts.fl < lTh) || !(chMask & IrCh::FL)) &&
//                    ((IrInts.fr < lTh) || !(chMask & IrCh::FR)) &&
//                    ((IrInts.sl < lTh) || !(chMask & IrCh::SL)) &&
//                    ((IrInts.sr < lTh) || !(chMask & IrCh::SR))
//                    )
//                break;
//        }
//        return true;
//    }
//    else
//    {
//        return false;
//    }
//}
//
//// this func will block!
//void WaitIrTouch(unsigned char chMask, int hTh, int lTh)
//{
//    while(true)
//    {
//        Task_sleep(50);
//        if(     ((IrInts.fl < lTh) || !(chMask & IrCh::FL)) &&
//                ((IrInts.fr < lTh) || !(chMask & IrCh::FR)) &&
//                ((IrInts.sl < lTh) || !(chMask & IrCh::SL)) &&
//                ((IrInts.sr < lTh) || !(chMask & IrCh::SR))
//                )
//            break;
//    }
//    while(true)
//    {
//        Task_sleep(50);
//        if(     ((IrInts.fl > hTh) && (chMask & IrCh::FL)) ||
//                ((IrInts.fr > hTh) && (chMask & IrCh::FR)) ||
//                ((IrInts.sl > hTh) && (chMask & IrCh::SL)) ||
//                ((IrInts.sr > hTh) && (chMask & IrCh::SR))
//                )
//            break;
//    }
//    while(true)
//    {
//        Task_sleep(50);
//        if(     ((IrInts.fl < lTh) || !(chMask & IrCh::FL)) &&
//                ((IrInts.fr < lTh) || !(chMask & IrCh::FR)) &&
//                ((IrInts.sl < lTh) || !(chMask & IrCh::SL)) &&
//                ((IrInts.sr < lTh) || !(chMask & IrCh::SR))
//                )
//            break;
//    }
//}

}
