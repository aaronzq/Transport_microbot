/*
 * TskIr.h
 *
 *  Created on: Aug 13, 2016
 *      Author: loywong
 */

#ifndef TSKIR_TSKIR_H_
#define TSKIR_TSKIR_H_

#include <ti/sysbios/knl/Mailbox.h>

#define IrApproxOrder 2

namespace TskIr
{

//class IrCh
//{
//public:
//    enum Type
//    {
//        lway = 0x1,
//        left = 0x2,
//        center = 0x4,
//        right = 0x8,
//        rway = 0x10
//    };
//};


union IrIntensity
{
    unsigned short ch[6];
    struct
    {
        unsigned short lway;
        unsigned short left;
        unsigned short center;
        unsigned short right;
        unsigned short rway;
        unsigned short middle;
    };
};

#pragma pack(push)
#pragma pack(1)

union IrBinaryTh
{
    unsigned int Th;
    struct
    {
        unsigned short ThLo;
        unsigned short ThHi;
    };
};
union IrBinaryThs
{
    IrBinaryTh ch[6];
    struct
    {
        IrBinaryTh lway;
        IrBinaryTh left;
        IrBinaryTh center;
        IrBinaryTh right;
        IrBinaryTh rway;
        IrBinaryTh middle;
    };
};
#pragma pack(pop)

union IrBinaries
{
    bool ch[6]; // true: black line; false: white board
    struct
    {
        bool lway;
        bool left;
        bool center;
        bool right;
        bool rway;
        bool middle;
    };
};

extern volatile IrIntensity IrInts;
extern volatile IrBinaryThs IrBinThs;
extern volatile IrBinaries IrBins;
extern volatile float IrYawL,IrYawR;
extern volatile bool IrBorder;
extern volatile bool IrBall;
extern volatile bool IrNoLine;

//extern Mailbox_Handle MbCmd;

extern const char *IrChNames[];

unsigned char irBranchmsg();

void Init();

//// this func test ir touch frist,
//// if touched, it will wait until untouch,
//// otherwise, it return immediately.
//// chMask: 0x1-fl, 0x2-fr, 0x4-sl, 0x8-sr
//// hTh, lTh: th for touch and untouch, unit in IrIntensity
//bool TestIrTouch(unsigned char chMask, int hTh, int lTh);
//// this func will block!
//void WaitIrTouch(unsigned char chMask, int hTh, int lTh);

}

#endif /* TSKIR_TSKIR_H_ */
