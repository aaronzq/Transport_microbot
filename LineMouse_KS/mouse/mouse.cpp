/*
 * mouse.cpp
 *
 *  Created on: Aug 5, 2016
 *      Author: zou
 */

#include "mouse.h"
#include "../mmaze/mmaze.h"

namespace Micromouse
{

	Mouse::Mouse(short col , short row , const GridCoor &target)
	{
		turningForwardFirst[0] = Turning::Forward;// = {Turning::Forward,Turning::Left,Turning::Right,Turning::Backward};
		turningForwardFirst[1] = Turning::Left;
		turningForwardFirst[2] = Turning::Right;
		turningForwardFirst[3] = Turning::Backward;

		this->Maze = new MMaze(col,row,target);

		this->colNum = col;
		this->rowNum = row;

		this->SearchingTimes = new unsigned short[this->colNum * this->rowNum];
		this->height = new unsigned short[this->colNum * this->rowNum];

		for(int y = 0; y <= rowNum; y++)
			for(int x = 0; x <= colNum; x++)
			{
				this->Maze->SetWall(GridCoor(x, y), WallType::Unknown, WallType::Unknown, WallType::Unknown, WallType::Unknown);
				if(x == 0)
				{
					this->Maze->SetWall(GridCoor(x, y), Direction::West, WallType::Blocked);
				}
				if(x == col)
					this->Maze->SetWall(GridCoor(x, y), Direction::West, WallType::Blocked);

				if(y == 0)
				{
					this->Maze->SetWall(GridCoor(x, y), Direction::South, WallType::Blocked);
				}
				if( y == row)
					this->Maze->SetWall(GridCoor(x, y), Direction::South, WallType::Blocked);
				if(y < rowNum  && x < colNum)
				this->SearchingTimes[x + y * colNum] = 0;
			}

		this->state.Proc = MouseProcState::Idle;
		this->currHeading = Direction::North;
		this->currGrid.X = 0;
		this->currGrid.Y = 0;
		this->Target = target;

		this->turningPrior = this->turningForwardFirst;
		this->Priority =10000;
	}

	void Mouse::PutSearchTimes(unsigned short *searchingtimes)
	{
		for(int i =0; i < rowNum; i++)
			for(int j = 0; j < colNum; j++)
				searchingtimes[i + j * colNum] = this->SearchingTimes[i + j * colNum];
	}
	void Mouse::GetSearchingTimes(unsigned short *searchingtimes)
	{
		for(int i =0; i < rowNum; i++)
			for(int j = 0; j < colNum; j++)
				this->SearchingTimes[i + j * colNum] = searchingtimes[i + j * colNum];
	}

	void Mouse::PutHeight(unsigned short *Height)
	{
		for(int i = 0;i < rowNum; i++)
			for(int j = 0; j < colNum; j++)
				Height[i + j * colNum] = this->height[i + j * colNum];
	}

	void Mouse::PutTheGridInfo( GridInfo *gridsaddr,int *size)
	{
		this->Maze->PutTheGridInfo(gridsaddr,size);
	}

	void Mouse::GetTheGridInfo( GridInfo *gridsaddr)
	{
		this->Maze->GetTheGridInfo(gridsaddr);
	}

	WallType::Type Mouse::GetWall(const GridCoor& coor, Direction::Type dir)
	{

		return this->Maze->GetWall(coor,dir);
	}

	Mouse::~Mouse()
	{
		delete this->Maze;
		delete [] this->height;
		delete [] this->SearchingTimes;
	}

	inline int Mouse::index(const GridCoor &coor)
	{
		return coor.X + coor.Y * this->colNum;
	}

	inline int Mouse::index(short x, short y)
	{
		return x + y * this->colNum;
	}

	Turning::Type Mouse::Step(WallType::Type left,WallType::Type forward,WallType::Type right,bool *srchFinish)
	{
		if((this->currGrid.X ==0 && this->currGrid.Y == 0)&&(this->state.Proc == MouseProcState::Idle))
		{
			this->state.Proc = MouseProcState::ForwardSearching;
			this->state.RunTimes ++;
		}

			this->Maze->SetWall(this->currGrid,this->currHeading + Turning::Forward,forward);
			this->Maze->SetWall(this->currGrid,this->currHeading + Turning::Left,left);
			this->Maze->SetWall(this->currGrid,this->currHeading + Turning::Right,right);

		switch(this->state.Proc)
		{
		case MouseProcState::Idle:
			this->state.Proc = MouseProcState::ForwardSearching;
			break;
		case MouseProcState::ForwardSearching:
			this->Maze->Fluid(this->height,this->Target,this->currGrid,WallType::Unknown,false);
			break;
		case MouseProcState::BackwardSearching:
			this->Maze->Fluid(this->height,GridCoor(0,0),this->currGrid,WallType::Unknown,false);
			break;
		case MouseProcState::Running:
			//this->Maze->Fluid(this->height,this->Target,this->currGrid,WallType::Open,false);
			this->Maze->SFluid(this->height,this->Target,this->currGrid,WallType::Open,true);
			break;
		default:
			break;

		}

		this->SearchingTimes[this->currGrid.X + this->currGrid.Y * this->colNum] ++;

		Turning::Type bestTurn = Turning::Forward;
		Direction::Type dir;
		GridCoor adgrid;
		this->Priority = 10000;
		for(int i = 0; i < 4; i++)
		{
			dir = this->currHeading + this->turningForwardFirst[i];
			adgrid = this->currGrid + dir;
			short tempPrior = this->height[this->index(adgrid)];
			if((this->Maze->GetWall(this->currGrid , dir) == WallType::Open)
					&&(tempPrior != 0))
			{
				if(this->state.Proc != MouseProcState::Running)
				{
					tempPrior += this->PriorityCal(this->SearchingTimes[this->index(adgrid)]);
				}
				if(tempPrior < this->Priority)
				{
					this->Priority = tempPrior;
					bestTurn = turningForwardFirst[i];
				}
			}
		}


		if(bestTurn == Turning::Backward)
			this->SearchingTimes[this->currGrid.X + this->currGrid.Y * this->colNum] ++;

		this->currHeading += bestTurn;
		this->currGrid += this->currHeading;

		if(this->currGrid.X == this->Target.X && this->currGrid.Y == this->Target.Y)
		{
			this->state.Proc = MouseProcState::BackwardSearching;
			this->state.RunTimes ++;
		}
		else if(this->currGrid.X == 0 && this->currGrid.Y == 0)
		{
			this->state.Proc = MouseProcState::Running;
			*srchFinish = true;
			this->state.RunTimes ++;
		}

		return bestTurn;
	}

	void Mouse::PathGenerate(Turning::Type *TurnSeq,unsigned short *height,const GridCoor& start,const GridCoor& target,unsigned char *SeqNum)
	{

		Direction::Type thisdir;
		Direction::Type nextdir;
		GridCoor thisGrid;
		GridCoor nextGrid;

		short tempPrio = 100;
		int num = 0;

		thisGrid = start;
		thisdir = Direction::North;

		while(thisGrid != target)
		{
			for(int i = 0; i < 4; i++)
			{
				nextdir = thisdir + this->turningForwardFirst[i];
				nextGrid = thisGrid + nextdir;
				if(nextGrid.X < 0 || nextGrid.X >= this->colNum || nextGrid.Y < 0 || nextGrid.Y >= this->rowNum)
					continue;
				if(this->GetWall(thisGrid, nextdir) == WallType::Open)
				{
					if(height[this->index(nextGrid)] < tempPrio )
					{
						TurnSeq[num] = this->turningForwardFirst[i];
						tempPrio = height[this->index(nextGrid)];

					}
				}
			}
			thisdir += TurnSeq[num];
			thisGrid += thisdir;
			num++;
		}
		*SeqNum = num;
	}




}
