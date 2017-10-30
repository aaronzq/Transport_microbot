/*
 * solver.h
 *
 *  Created on: Apl 11, 2017
 *      Author: wzq
 */

#ifndef SOLVER_H_
#define SOLVER_H_

#include <xdc/std.h>
#include "../TskAction/TskAction.h"

namespace TskSolver{

class StateMsg
{
public:
	enum MsgType
	{
		Origin_no_ball         = 0x00000001,
		Destination_no_ball    = 0x00000002,
		Destination_with_ball  = 0x00000003,
		Origin_with_ball       = 0x00000004
	};
};

extern Mailbox_Handle MbCmd;
//extern TskAction::MotorAct::ActType ActSeq[100];
void Init();

}
#endif /* TSKSOLVER_TSKSOLVER_H_ */
