/*
 * action.h
 *
 *  Created on: Apl 11, 2017
 *      Author: wzq
 */

#ifndef TSKACTION_TSKACTION_H_
#define TSKACTION_TSKACTION_H_

namespace TskAction
{

class ActType
{
public:
	enum Action
	{
		start_left_side = 0x01,
		start_right_side,
		forward_left_side,
		forward_left_side_border,
		forward_left_side_backturn,
		forward_right_side,
		forward_right_side_back,
		forward_right_side_border,
		forward_right_side_backturn,
		left_turn90,
		right_turn90,
		left_turn180,
		right_turn180,
		stop,
		back

	};
};


class ActionMsg
{
public:
	enum MsgType
	{
		Idle = 0x00000001,
		Go_for_ball,
		wait4ball,
		take_back_ball,
		lay_down_ball
	};
};

extern Mailbox_Handle MbCmd;
void Init();

}


#endif /* TSKACTION_TSKACTION_H_ */
