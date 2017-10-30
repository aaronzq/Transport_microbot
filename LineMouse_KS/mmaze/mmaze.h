//
//  mmmaze.h
//  mmmaze
//
//  Created by Loy Kyle Wong on 11/09/2013.
//  Copyright (c) 2013 Loy Kyle Wong. All rights reserved.
//

#ifndef MMAZE_H_
#define MMAZE_H_

namespace Micromouse
{
    class WallType
    {
    public: enum Type
        {
            Undefined = 0,
            Blocked = 1,
            Unknown = 2,
            Open = 3
        };
    };

    class Direction
    {
    public : enum Type
        {
            North = 0,
            West = 1,
            South = -2,
            East = -1
        };
    };
    
    class Turning
    {
    public : enum Type
        {
            Forward = 0,
            Left = 1,
            Backward = -2,
            Right = -1
        };
    };

#pragma pack(push)
#pragma pack(2)
    struct GridCoor
    {
        // east x, north y
        unsigned short X : 5;
        unsigned short Y : 5;
        GridCoor() {
            X = 0; Y = 0;
        }
        GridCoor(unsigned short x, unsigned short y) {
            X = x; Y = y;
        }
        void Set(unsigned short x, unsigned short y) {
            X = x; Y = y;
        }
    };
#pragma pack(pop)
    
    // direction turning
    Direction::Type operator+(const Direction::Type &, const Turning::Type &);
    const Direction::Type &operator+=(Direction::Type &, const Turning::Type &);
    Turning::Type operator-(const Direction::Type &, const Direction::Type &);
    // grid direction
    bool operator!=(const GridCoor & l, const GridCoor & r);
    GridCoor operator+(const GridCoor &, const Direction::Type &);
    const GridCoor &operator+=(GridCoor &, const Direction::Type &);
    Direction::Type operator-(const GridCoor &, const GridCoor &);
    
#pragma pack(push)
#pragma pack(1)
    struct GridInfo
    {
        WallType::Type South : 2;
        WallType::Type West : 2;
        GridInfo()
        {
            South = WallType::Undefined;
            West = WallType::Undefined;
        }
    };
#pragma pack(pop)
    

    template <typename T>
    class Queue
    {
    private:
        T *array;
        int head;
        int tail;
        int capacity;
    public:
        Queue(int capacity);
        ~Queue();
        void Enqueue(const T & data);
        T Dequeue();
        T Peek();
        void Reset();
        int Length();
        int Capacity();
    };
    
    class MMaze
    {
    private:
        GridInfo *grids;
        GridInfo *hwGrids;
        short colNum;
        short rowNum;
        GridCoor target;
        int index(const GridCoor &coor);
        int index(short x, short y);
        //void hwSetWall(int idx, Direction::Type dir, WallType::Type wt);
    public:
        MMaze();
        MMaze(short col , short row , const GridCoor &target1);
        ~MMaze();
        void PutTheGridInfo(GridInfo *gridsaddr,int *size);
        void GetTheGridInfo(GridInfo *gridsaddr);
        void setTarget(const GridCoor &l);
        bool SetWall(const GridCoor& coor, Direction::Type dir, WallType::Type wall);
        bool SetWall(const GridCoor& coor, WallType::Type north, WallType::Type west, WallType::Type south, WallType::Type east);

        WallType::Type GetWall(const GridCoor& coor, Direction::Type dir);

        void Fluid(unsigned short * height, const GridCoor &start, const GridCoor &target, WallType::Type wallCanGo, bool fullFluid);
        void SFluid(unsigned short * height, const GridCoor &start, const GridCoor &target, WallType::Type wallCanGo, bool fullFluid);
        short ColNum();
        short RowNum();
        GridCoor Target();
    };
}
#endif /* defined(__mmmaze__mmmaze__) */
