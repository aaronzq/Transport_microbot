/*
 * MotorPwm.cc
 *
 *  Created on: Aug 3, 2016
 *      Author: loywong
 */


#include <inc/hw_memmap.h>
#include <inc/hw_pwm.h>
#include <inc/hw_types.h>
#include <inc/hw_gpio.h>
#include <inc/hw_sysctl.h>

#include <xdc/std.h>

void ServoPwmReset()
{
	HWREG(PWM1_BASE | PWM_O_1_CMPA) = 44800;  //pwm 50%
	HWREG(PWM1_BASE | PWM_O_1_GENA) = 0x083; //matches zero --> turn high   matches comparatorA --> turn inverse
	HWREG(PWM1_BASE | PWM_O_CTL) = 0x2;
}

void ServoPwmSetDuty(int s)  //0-64000
{
	HWREG(PWM1_BASE | PWM_O_1_CMPA) = s;
	HWREG(PWM1_BASE | PWM_O_1_GENA) = 0x083; //matches zero --> turn high   matches comparatorA --> turn inverse

	HWREG(PWM1_BASE | PWM_O_CTL) = 0x2;
}



void MotorPwmCoast()
{   // p - low, n - low
    // zero - fall
    // motor right
    HWREG(PWM1_BASE | PWM_O_2_GENA) = 0x002;    // zero - fall
    HWREG(PWM1_BASE | PWM_O_2_GENB) = 0x002;    // zero - fall
    // motor left
    HWREG(PWM1_BASE | PWM_O_3_GENA) = 0x002;    // zero - fall
    HWREG(PWM1_BASE | PWM_O_3_GENB) = 0x002;    // zero - fall
    // update
    HWREG(PWM1_BASE | PWM_O_CTL) = 0xC; // update gen3, gen2
}

// r,l -> [-319, 319]
void MotorPwmSetDuty(short r, short l)
{
    l=-l;
    if(r < 0)
    {
        // cmp a -> gen b -> pwm7 -> rp : -> abs(r)
        //          gen a -> pwm6 -> rn : steady high
        HWREG(PWM1_BASE | PWM_O_2_CMPA) = -r;
        HWREG(PWM1_BASE | PWM_O_2_GENA) = 0x083;
        HWREG(PWM1_BASE | PWM_O_2_GENB) = 0x003;
    }
    else
    {
        //          gen b -> pwm7 -> rp : steady high
        // cmp a -> gen a -> pwm6 -> rn : -> abs(r)
        HWREG(PWM1_BASE | PWM_O_2_CMPA) = r;
        HWREG(PWM1_BASE | PWM_O_2_GENA) = 0x003;
        HWREG(PWM1_BASE | PWM_O_2_GENB) = 0x083;
    }
    if(l < 0)
    {
        // cmp a -> gen a -> pwm4 -> rp : -> abs(l)
        //          gen b -> pwm5 -> rn : steady high
        HWREG(PWM1_BASE | PWM_O_3_CMPA) = -l;
        HWREG(PWM1_BASE | PWM_O_3_GENA) = 0x003;
        HWREG(PWM1_BASE | PWM_O_3_GENB) = 0x083;
    }
    else
    {
        //          gen b -> pwm7 -> rp : steady high
        // cmp a -> gen a -> pwm6 -> rn : -> abs(l)
        HWREG(PWM1_BASE | PWM_O_3_CMPA) = l;
        HWREG(PWM1_BASE | PWM_O_3_GENA) = 0x083;
        HWREG(PWM1_BASE | PWM_O_3_GENB) = 0x003;
    }
    // update
    HWREG(PWM1_BASE | PWM_O_CTL) = 0xC; // update gen3, gen2
}

void MotorPwmInit()
{
	HWREG(SYSCTL_RCGCPWM) = 0x00000002;    // enable PWM1, disable PWM0
	HWREG(SYSCTL_RCGCGPIO) |= 0x00000021;   //GPIO  A&F enable

    //-----------------------WZQ-KS-append------------M1PWM2 PA6-------------------------------
    HWREG(GPIO_PORTA_BASE | GPIO_O_AFSEL) |= 0x00000040;    // port a[6] as alternate func
    HWREG(GPIO_PORTA_BASE | GPIO_O_DEN) |= 0x00000040;    // port a[6] digital enable
    HWREG(GPIO_PORTA_BASE | GPIO_O_PCTL) = HWREG(GPIO_PORTA_BASE | GPIO_O_PCTL) & 0xF0FFFFFF | 0x05000000;  // port a[6] as af5(pwm)
    //-----------------------------------------------------------------------------------

    HWREG(GPIO_PORTF_BASE | GPIO_O_AFSEL) |= 0x0000000F;    // port f[3..0] as alternate func
    HWREG(GPIO_PORTF_BASE | GPIO_O_DEN) |= 0x0000000F;    // port f[3..0] digital enable
    HWREG(GPIO_PORTF_BASE | GPIO_O_PCTL) = HWREG(GPIO_PORTF_BASE | GPIO_O_PCTL) & 0xFFFF0000 | 0x00005555;  // port f[3..0] as af5(pwm)

    HWREG(PWM1_BASE | PWM_O_ENUPD) = 0x0000FF00;    // pwm 4~7 enable update by glb sync

//    HWREG(SYSCTL_RCC) &= ~SYSCTL_RCC_USEPWMDIV; // pwm clock use sysclock
    HWREG(SYSCTL_RCC) |= SYSCTL_RCC_USEPWMDIV;  //pwm div sysclock
    HWREG(SYSCTL_RCC) =  HWREG(SYSCTL_RCC) & 0xFFF0FFFF | SYSCTL_RCC_PWMDIV_4;

    //-----------------------WZQ-KS-append------------M1PWM2 PA6-------------------------------
    HWREG(PWM1_BASE | PWM_O_1_LOAD) = 64000;   // pwm gen 1 freq 80M/64000/4 = 312.5hz
    HWREG(PWM1_BASE | PWM_O_1_CTL) = 0x000003FD;   // enable pwm gen 2; cnt down mode; run in debug; load, cmp, gen update by glb sync
    //-----------------------------------------------------------------------------------

    HWREG(PWM1_BASE | PWM_O_2_LOAD) = 319;   // pwm gen 2 freq 80M/320/4 = 62.5k
    HWREG(PWM1_BASE | PWM_O_3_LOAD) = 319;   // pwm gen 3 freq 80M/320 = 62.5k

    HWREG(PWM1_BASE | PWM_O_2_CTL) = 0x000003FD;   // enable pwm gen 2; cnt down mode; run in debug; load, cmp, gen update by glb sync
    HWREG(PWM1_BASE | PWM_O_3_CTL) = 0x000003FD;   // enable pwm gen 3; ...

    HWREG(PWM1_BASE | PWM_O_SYNC) = 0x0000000E; // sycn cnt1 & cnt 2 & cnt 3    //edited by wzq-ks

    MotorPwmCoast();
    ServoPwmReset();

    HWREG(PWM1_BASE | PWM_O_ENABLE) = 0xF4; // enable output 4~7 2 // edited by wzq-ks
//    HWREG(PWM1_BASE | PWM_O_CTL) = 0xC; // update gen3, gen2

}

