//
//  mmmaze.cpp
//  mmmaze
//
//  Created by Loy Kyle Wong on 11/09/2013.
//  Copyright (c) 2013 Loy Kyle Wong. All rights reserved.
//

#include "mmaze.h"
#include <math.h>
#include <stdlib.h>

namespace Micromouse
{

    void operator++(Direction::Type& l)
    {
        signed char i = (signed char)l + 1;
        if(i > (signed char)Direction::West)
            i -= 4;
        l = (Direction::Type)i;
    }
    
    Direction::Type operator+(const Direction::Type & l, const Turning::Type & r)
    {
        signed char rtn = (signed char)l + (signed char)r;
        if(rtn > 1) return (Direction::Type)(rtn - 4);
        else if(rtn < -2) return (Direction::Type)(rtn + 4);
        else return (Direction::Type)rtn;
    }
    
    const Direction::Type &operator+=(Direction::Type & l, const Turning::Type & r)
    {
        signed char rtn = (signed char)l + (signed char)r;
        if(rtn > 1) l = (Direction::Type)(rtn - 4);
        else if(rtn < -2) l = (Direction::Type)(rtn + 4);
        else l = (Direction::Type)rtn;
        return l;
    }
    
    inline Turning::Type operator-(const Direction::Type & l, const Direction::Type & r)
    {
        signed char rtn = (signed char)l - (signed char)r;
        if(rtn > 1) return (Turning::Type)(rtn - 4);
        if(rtn < -2) return (Turning::Type)(rtn + 4);
        else return (Turning::Type)rtn;
    }
    
    // N N N
    // W N E
    // S S S
    inline Direction::Type operator-(const GridCoor & l, const GridCoor & r)
    {
        short x = l.X - r.X;
        short y = l.Y - r.Y;
        if(abs(x) > abs(y)) // east or west
        {
            if (x < 0)
                return Direction::West;
            else
                return Direction::East;
        }
        else // north or south
        {
            if(y < 0)
                return Direction::South;
            else
                return Direction::North;
        }
    }
    
    bool operator!=(const GridCoor & l, const GridCoor & r)
    {
        if((l.X == r.X)&&(l.Y == r.Y))
        	return false;
        else
        	return true;
    }


    GridCoor operator+(const GridCoor & l, const Direction::Type & r)
    {
        switch (r) {
            case Direction::East:
                return GridCoor(l.X + 1, l.Y);
                break;
            case Direction::West:
                return GridCoor(l.X - 1, l.Y);
                break;
            case Direction::North:
                return GridCoor(l.X, l.Y + 1);
                break;
            case Direction::South:
                return GridCoor(l.X, l.Y - 1);
                break;
            default:
                break;
        }
    	return GridCoor(l.X, l.Y);
    }
    
    const GridCoor &operator+=(GridCoor & l, const Direction::Type & r)
    {
        switch (r) {
            case Direction::East:
                l.X++;
                break;
            case Direction::West:
                l.X--;
                break;
            case Direction::North:
                l.Y++;
                break;
            case Direction::South:
                l.Y--;
                break;
            default:
                break;
        }
        return l;
    }
    
    template <typename T>
    Queue<T>::Queue(int capacity)
    {
        this->array = new T[capacity + 1];
        this->head = 0;
        this->tail = 0;
        this->capacity = capacity;
    }
    
    template <typename T>
    Queue<T>::~Queue()
    {
        delete[] this->array;
    }
    
    template <typename T>
    void Queue<T>::Enqueue(const T & data)
    {
#ifdef SAFE
        if (Length() < capacity)
        {
#endif
            this->array[tail++] = data;
            if (tail > capacity) tail = 0;
#ifdef SAFE
        }
        else
            throw "Error: Enqueue when queue full.";
#endif
    }
    
    template <typename T>
    T Queue<T>::Dequeue()
    {
#ifdef SAFE
        if (Length() > 0)
        {
#endif
            T& rtn = this->array[head++];
            if(head > capacity) head = 0;
            return rtn;
#ifdef SAFE
        }
        else
            throw "Error: Dequeue when queue empty.";
#endif
    }
    
    template <typename T>
    T Queue<T>::Peek()
    {
#ifdef SAFE
        if (Length() > 0)
        {
#endif
            return this->array[head++];
#ifdef SAFE
        }
        else
            throw "Error: Peek when queue empty";
#endif
    }
    
    template <typename T>
    void Queue<T>::Reset()
    {
        this->head = 0;
        this->tail = 0;
    }
    
    template <typename T>
    int Queue<T>::Length()
    {
        int rtn = this->tail - this->head;
        if(rtn < 0)
            return rtn + capacity + 1;
        else
            return rtn;
    }
    
    template <typename T>
    int Queue<T>::Capacity()
    {
        return capacity;
    }

    MMaze::MMaze()
    {
       this->colNum = 16;
       this->rowNum = 16;
       this->target.X = 8;
       this->target.Y =7;

        // this->colNum = 32;
        // this->rowNum = 32;
        // this->target.X = 25;
        // this->target.Y =27;

        this->grids = new GridInfo[(colNum + 1) * (rowNum + 1)];

        for (int y = 0; y <= rowNum; y++)
        {
            for (int x = 0; x <= colNum; x++)
            {
					this->grids[index(x, y)].South = WallType::Unknown;
					this->grids[index(x, y)].West = WallType::Unknown;
            }
        }
    }
    
    MMaze::MMaze(short col , short row , const GridCoor &target1)
    {
        this->colNum = col;
        this->rowNum = row;
        this->target.X = target1.X;
        this->target.Y =target1.Y;

        this->grids = new GridInfo[(colNum + 1) * (rowNum + 1)];

        for (int y = 0; y <= rowNum; y++)
        {
            for (int x = 0; x <= colNum; x++)
            {
					this->grids[index(x, y)].South = WallType::Unknown;
					this->grids[index(x, y)].West = WallType::Unknown;
            }
        }
    }
      
    MMaze::~MMaze()
    {
        delete[] this->grids;
    }
    
    void MMaze::PutTheGridInfo( GridInfo *gridsaddr,int *size)
    {
        for (int y = 0; y <= rowNum; y++)
        {
            for (int x = 0; x <= colNum; x++)
            {
            	gridsaddr[index(x, y)].South = this->grids[index(x, y)].South;
            	gridsaddr[index(x, y)].West  = this->grids[index(x, y)].West;
            }
        }
    	*size = sizeof(GridInfo) * (colNum + 1) * (rowNum + 1);
    }

    void MMaze::GetTheGridInfo( GridInfo *gridsaddr)
    {
        for (int y = 0; y <= rowNum; y++)
        {
            for (int x = 0; x <= colNum; x++)
            {
            	this->grids[index(x, y)].South = gridsaddr[index(x, y)].South ;
            	this->grids[index(x, y)].West  = gridsaddr[index(x, y)].West ;
            }
        }
    }



    void MMaze::setTarget(const GridCoor &l)
    {
    	this->target = l;
    }

    inline int MMaze::index(const GridCoor &coor)
    {
        return coor.X + coor.Y * (this->colNum + 1);
    }
    
    inline int MMaze::index(short x, short y)
    {
        return x + y * (this->colNum + 1);
    }

    short MMaze::ColNum()
    {
        return this->colNum;
    }
    
    short MMaze::RowNum()
    {
        return this->rowNum;
    }
    
    GridCoor MMaze::Target()
    {
        return this->target;
    }
    
    bool MMaze::SetWall(const GridCoor& coor, Direction::Type dir, WallType::Type wall)
    {
#ifdef SAFE
        if(coor.X >= colNum || coor.X < 0 || coor.Y >= rowNum || coor.Y < 0)
            throw "Error: Coordinates exceed limit.";
#endif
        if(wall == WallType::Undefined) return true;
        switch (dir)
        {
        case Direction::South:
            this->grids[index(coor)].South = wall;
            break;
        case Direction::West:
            this->grids[index(coor)].West = wall;
            break;
        case Direction::North:
            this->grids[index(coor + Direction::North)].South = wall;
            break;
        case Direction::East:
            this->grids[index(coor + Direction::East)].West = wall;
            break;
        default:
            break;
        }
        return true;
    }
    
    bool MMaze::SetWall(const GridCoor& coor, WallType::Type north, WallType::Type west, WallType::Type south, WallType::Type east)
    {
#ifdef SAFE
        if(coor.X >= colNum || coor.X < 0 || coor.Y >= rowNum || coor.Y < 0)
            throw "Error: Coordinates exceed limit.";
#endif
        if(north != WallType::Undefined)
        {
            this->grids[index(coor + Direction::North)].South = north;
        }
        if(east != WallType::Undefined)
        {
            this->grids[index(coor + Direction::East)].West = east;
        }
        if(south != WallType::Undefined)
        {
            this->grids[index(coor)].South = south;
        }
        if(west != WallType::Undefined)
        {
            this->grids[index(coor)].West = west;
        }
        return true;
    }
    
    WallType::Type MMaze::GetWall(const GridCoor& coor, Direction::Type dir)
    {
//#ifdef SAFE
//        if(coor.X >= colNum || coor.X < 0 || coor.Y >= rowNum || coor.Y < 0)
//            throw "Error: Coordinates exceed limit.";
//#else
//        if(coor.X >= colNum || coor.X < 0 || coor.Y >= rowNum || coor.Y < 0)
//            return WallType::Undefined;
//#endif
        switch (dir) {
            case Direction::South:
                return this->grids[index(coor)].South;
                break;
            case Direction::West:
                return this->grids[index(coor)].West;
                break;
            case Direction::North:
                return this->grids[index(coor + Direction::North)].South;
                break;
            case Direction::East:
                return this->grids[index(coor + Direction::East)].West;
                break;
            default:
                return WallType::Undefined;
                break;
        }
        return WallType::Undefined;
    }

    void MMaze::Fluid(unsigned short *height, const GridCoor& start, const GridCoor& target, WallType::Type wallCanGo, bool fullFluid)
    {
        static Queue<GridCoor> q(this->colNum * this->rowNum / 2);
        q.Reset();
        
        short h;
        GridCoor grid(0, 0);
        GridCoor adjGrid(0, 0);
        for(int y = 0; y < this->rowNum; y++)
            for(int x = 0; x < this->colNum; x++) {
                height[x + y * this->colNum] = 0;
            }
        q.Enqueue(start);
        height[start.X + start.Y * this->colNum] = 1;
        while (q.Length() > 0)
        {
            grid = q.Dequeue();
            h = height[grid.X + grid.Y * this->colNum];
            for(signed char i = -2; i <= 1; i++)
            {
                adjGrid = grid + (Direction::Type)i;
                if(adjGrid.X < 0 || adjGrid.X >= this->colNum || adjGrid.Y < 0 || adjGrid.Y >= this->rowNum)
                    continue;
                if(this->GetWall(grid, (Direction::Type)i) >= wallCanGo && height[adjGrid.X + adjGrid.Y * this->colNum] == 0)
                {
                    if(fullFluid || adjGrid.X != target.X || adjGrid.Y != target.Y) {
                        q.Enqueue(adjGrid);
                        height[adjGrid.X + adjGrid.Y * this->colNum] = h + 1;
                    }
                    else {
                        return;
                    }
                }
            }
        }
    }

    void MMaze::SFluid(unsigned short *height, const GridCoor& start, const GridCoor& target, WallType::Type wallCanGo, bool fullFluid)
    {
        static Queue<GridCoor> q(this->colNum * this->rowNum);
        q.Reset();

        short h;
        bool FormerPoint = false;
        GridCoor grid(0, 0);
        GridCoor adjGrid(0, 0);
        GridCoor formerGrid(0, 0);
        Direction::Type formerDir;
        for(int y = 0; y < this->rowNum; y++)
            for(int x = 0; x < this->colNum; x++) {
                height[x + y * this->colNum] = 0;
            }
        q.Enqueue(start);
        height[start.X + start.Y * this->colNum] = 1;
        while (q.Length() > 0)
        {

            grid = q.Dequeue();

        	if(FormerPoint)
        	{
                formerGrid = q.Dequeue();
                formerDir = grid - formerGrid;
        	}

            h = height[grid.X + grid.Y * this->colNum];
            for(signed char i = -2; i <= 1; i++)
            {
                adjGrid = grid + (Direction::Type)i;
                if(adjGrid.X < 0 || adjGrid.X >= this->colNum || adjGrid.Y < 0 || adjGrid.Y >= this->rowNum)
                    continue;
                if(this->GetWall(grid, (Direction::Type)i) >= wallCanGo && height[adjGrid.X + adjGrid.Y * this->colNum] == 0)
                {
                    if(fullFluid || adjGrid.X != target.X || adjGrid.Y != target.Y) {
                        q.Enqueue(adjGrid);
                        q.Enqueue(grid);
                        if(FormerPoint && (i == (signed char)formerDir))
                        {
                        	height[adjGrid.X + adjGrid.Y * this->colNum] = h + 1;
                        }
                        else
                        {
                        	height[adjGrid.X + adjGrid.Y * this->colNum] = h + 2;
                        	FormerPoint = true;
                        }

                    }
                    else {
                        return;
                    }
                }
            }
        }
    }
}
