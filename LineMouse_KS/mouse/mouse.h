/*
 * mouse.h
 *
 *  Created on: Aug 5, 2016
 *      Author: zou
 */

#ifndef MOUSE_H_
#define MOUSE_H_

#include <math.h>

#include "../mmaze/mmaze.h"


namespace Micromouse
{

	class MouseProcState
	{
		public:enum Type
		{
			Idle = 0,
			ForwardSearching = 1,
			BackwardSearching = 2,
			Running = 3
		};
	};

	struct MouseState
	{
		MouseProcState::Type Proc;
		short RunTimes;
		MouseState()
		{
			Proc = MouseProcState::Idle;
			RunTimes = 0;
		}
	};


	class Mouse
	{

	private:
		short colNum;
		short rowNum;

		MMaze *Maze;
		unsigned short *height;
		unsigned short *SearchingTimes;

		MouseState state;
		Direction::Type currHeading;
		GridCoor currGrid;
		GridCoor Target;

		Turning::Type turningForwardFirst[4];// = {Turning::Forward,Turning::Left,Turning::Right,Turning::Backward};
		Turning::Type *turningPrior;

		int Priority;

	public:
		Mouse(short col, short row, const GridCoor &target);

		void PutTheGridInfo( GridInfo *gridsaddr,int *size);
		void GetTheGridInfo( GridInfo *gridsaddr);
		WallType::Type GetWall(const GridCoor& coor, Direction::Type dir);

		void PutSearchTimes(unsigned short *searchingtimes);
		void GetSearchingTimes(unsigned short *searchingtimes);

		void PutHeight(unsigned short *Height);

		Direction::Type CurrHeading()
		{
			return this->currHeading;
		}

		GridCoor CurrGrid()
		{
			return this->currGrid;
		}

		MouseProcState::Type mousepro()
		{
			return this->state.Proc;
		}

		short Runningtimes()
		{
			return this->state.RunTimes ++;
		}

		short PriorityCal(unsigned m)
		{
			short t = 0;
			t = short(0.5f + powf(m,0.707f));
			return t;
		}

		short GetRow()
		{
			return rowNum;
		}
		short GetCol()
		{
			return colNum;
		}

		int index(const GridCoor &coor);
		int index(short x, short y);

		Turning::Type Step(WallType::Type left,WallType::Type forward,WallType::Type right,bool *srchFinish);
		void PathGenerate(Turning::Type *TurnSeq, unsigned short *height,const GridCoor& start, const GridCoor& target,unsigned char *SeqNum);
		virtual ~Mouse();
	};

}

#endif /* MOUSE_H_ */
