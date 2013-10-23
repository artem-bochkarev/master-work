#pragma once
#include "../GeneticAPI/CAutomat.h"
#include "CMap.h"

class CTest
{
public:
    CTest( CAutomat* automat, CMap* map, size_t moves = 100, int x = 0, int y = 0, 
           EDirection start = DRight );
    bool makeMove();
    static double run( CAutomat* automat, CMap* map, size_t moves = 100 );
    const CMap* getArena() const;
    size_t movesMaked() const;
    size_t foodEaten() const;
    int getX() const;
    int getY() const;
    EDirection getDir() const;

    static std::vector< std::pair<int, int> > getVisibleCells( const CMap *map, 
        int x, int y, EDirection direct );
private:
    size_t moves, movesCnt;
    int x, y;
    CAutomat* automat;
    CMap* map;
    char currentState;
    size_t m_foodEaten;
    EDirection currentDirection;
};