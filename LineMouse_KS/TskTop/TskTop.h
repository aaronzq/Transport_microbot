/*
 * top.h
 *
 *  Created on: Jul 21, 2016
 *      Author: loywong
 */

#ifndef TSKTOP_TSKTOP_H_
#define TSKTOP_TSKTOP_H_
//-----------------------------------------functionList-----------------------------------------
//#define PIDtest
//#define	LVtest
//#define EncOB
//--------------------------------------------------------------------------------------
namespace TskTop
{

class LedColor
{
public:
    enum Type
    {
        Dark   = 0b000,
        Red    = 0b001,
        Green  = 0b010,
        Yellow = 0b011,
        Blue   = 0b100,
        Purple = 0b101,
        Cyan   = 0b110,
        Light  = 0b111
    };
};

class MouseMode
{
public:
    enum ModeType
    {
        Idle = 0,
        EncImuMonitor,
        IrMonitor,
        Pwmtest,
        Unit_rsptest,
        Run,
        Undef0,
        Undef1
    };
};

void Init();
void SetLeds(LedColor::Type led0, LedColor::Type led1);
void SetLedL(LedColor::Type led);
void SetLedR(LedColor::Type led);

extern volatile MouseMode::ModeType Mode;
extern volatile int secread;     //added by wzq for motortop time
extern volatile bool Lvtest;
}



#endif /* TSKTOP_TSKTOP_H_ */
