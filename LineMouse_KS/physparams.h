/*
 * phys_params.h
 *
 *  Created on: Aug 7, 2014
 *      Author: loywong
 */

#ifndef PHYSPARAMS_H_
#define PHYSPARAMS_H_

#define ENABLE_CORRECTION	1

#define RAD2DEG(x) 	((x) * (180.0f / 3.1415926f))
#define ABSF(x) 		((x) < 0.0f ? -(x) : (x))
#define ABSI(x) 		((x) < 0 ? -(x) : (x))
#define MAX(x, y) 	((x) > (y) ? (x) : (y))

namespace PP
{
//parameter for debug----------------------------
const float YawUnit=0.052f;   //the absolute value for yaw deviation
const float TurnRadius = 0.012f;  //the radius of turn 90
const float TurnBackRadius = 0.06f;  //radius of turning back

const float TurnCompensateCoff = 0.98f;

const float start_acc_path = 0.1f;
const float start_uni_path = 0.1f;
const float start_yaw_begin= 0.05f;
const float start_yaw_end  = 0.18f;

const float forward_uni_path =0.2f;
const float forward_yaw_begin=0.03f;
const float forward_yaw_end  =0.15f;
const float forward_border_detect_begin = 0.01f;
const float forward_backturn_trigger_begin = 0.05f;

const float turn90_uni_path = 0.f;
const float turn180_uni_path = 0.f;

const float stop_path = 0.02f;

const float back_time = 0.5f;

const float run_vel = 0.3f;



//-----------------------------------------------

//================  physics parameters
// friction coefficient
const float Mu =     0.70f; // 0.72f comment @2015-09-16 17:17

// gravity constant
const float g =      9.80665f;

// gravity center height
const float H =      0.005f;

// distance between 2 tires
const float W =      0.0735f;
// wheel
//const float RWheel = 0.01175f;
const float RWheel = 0.017f;

// system sampling period, DO NOT change this independently
// if Ts changed, consider:
//  * Encoder sampling period
//  * PID controller
//  * Imu auto adj zero
//  * Motion seq calc
const float Ts =     0.001f;
//================ end of physics

//================ geometry informations
// gravity center to the center of 4 wheels
const float GvCenterFwd =   0.000f;

// all geometry of mouse related to GRAVITY CENTER
const float BodyLength =   0.0925f;
const float BodyWidth =    0.0725f;
const float HeadFwd =       (0.0625f - GvCenterFwd);
// geometry center forward
const float GeoFwd =        (0.000f - GvCenterFwd);
// !!! straight segment must be no less then 2x GeoFwd !!!
// tail
const float TailBack =      (0.030f + GvCenterFwd);

// ir position related to gravity center
// forward ir forward dist to gravity center, approx
const float IrFFwd =        (0.040f - GvCenterFwd);
// side ir forward dist to gravity center, approx
const float IrSFwd =        (0.055f - GvCenterFwd);

// forward ir side dist to gravity center, approx
const float IrFSide =        0.0275f;
// side ir side dist to gravity center, approx
const float IrSSide =        0.0175f;

const float IrFwdLRDist =  0.055f;

const float IrSizeAngle =  (42.5f / 180.0f * 3.1415926536f);

// grids
const float GridSize =         0.180f;
const float WallThick =        0.012f;
const float CenterToWall =     ((GridSize - WallThick) / 2.0f);
const float StartTotalDist =   (GridSize - WallThick / 2.0f - TailBack);
const float StartAcclDist =    0.030f;
const float StopTotalDist =    (GridSize / 2.0f);
const float StopAccDist =      0.020f;
const float RestartDist =      (GridSize / 2.0f);

//================ end geometry infos

//================ action speeds & etc.
const int SeqArrayLen    = 512;
const int SearchSpeed    = 0.6f;	//0.36f comment @20150916 17:18
//================

}

#define CORR_BACKBYIR_EN		1

namespace CP
{
//================ basic correction coefficients
const float EncoderUnitCompensation = 1.010f;		// reduce to run farther
const float GyroUnitCompensation    = 1.000f;		// reduce to turn/rotate more
const float AcclUnitCompensation    = 1.000f;

// static omega
const float StaticOmegaCoef       = 0.0f;

//================ end basic correction coefficients

//================ action correction parameters

// 3 params defines how far will the side ir corr for heading dir effects during diff act
// after these fwd end corr starts
const float HEADING_BY_SIRSIDE_START_DIST = 0.020f;
const float HEADING_BY_SIRSIDE_STOP_DIST  = 0.020f;
const float HEADING_BY_SIRSIDE_FWD_DIST   = 0.060f;
// 2 params defines segs where side ir corr for heading dir effects during centipede
const float HEADING_BY_SIRFWD_BGNSTAT_POS = 0.040f;
const float HEADING_BY_SIRFWD_BEGIN_POS   = 0.140f;
const float HEADING_BY_SIRFWD_END_POS     = (PP::GridSize * 0.667 +  HEADING_BY_SIRFWD_BEGIN_POS * 0.333);
//#define HEADING_BY_SIRFWD_B2EHALF_DIST ((HEADING_BY_SIRFWD_END_POS - HEADING_BY_SIRFWD_BEGIN_POS) * 0.5f)

// left & right turn straight segment when no fwd wall for turnwait
// abs of these must be less than GeoFwd
const float TURNL90_PRE_ADJ       = -0.008f;
const float TURNR90_PRE_ADJ       = -0.009f;
const float TURNL90_POST_ADJ      = 0.004f;
const float TURNR90_POST_ADJ      = 0.006f;
// params defines distances added at fwd end corr occur
const float LFWDEND_DIST_W2NW     = 0.046f;
const float RFWDEND_DIST_W2NW     = 0.048f;
//#define FWDEND_NW2W_ENABLE    0
//#define FWDEND_DIST_NW2W      0.045f
const float STOPEND_DIST_ADJ      = -0.016f;
// turn wait dist adjustment
const float TURNLWAIT_DIST_ADJ    = -0.003f;    // more positive to turn later
const float TURNRWAIT_DIST_ADJ    = -0.011f;    // more positive to turn later
// during Act Back
const float LBACKANGLE_LRDIFF     = 0.004f;    // more positive to skew left more
const float RBACKANGLE_LRDIFF     = 0.006f;    // more positive to skew left more
const float LBACKCENTER_ADJ       = -0.007f;   // more positive to near the side wall
const float RBACKCENTER_ADJ       = -0.008f;   // more positive to near the side wall
//================ end action correction parameters
}
#endif /* PHYS_PARAMS_H_ */
