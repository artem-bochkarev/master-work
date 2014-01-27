#pragma once
#include "GeneticParams.h"
#include "GeneticAPI/CAutomat.h"
#include "GeneticCommon/CleverAntMap.h"

class CTest
{
public:
	CTest(CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat, CMap* map, size_t moves, int x = 0, int y = 0,
           EDirection start = DRight );
    bool makeMove();
	static double run(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat, CMap* map, size_t moves = 100);
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
	CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat;
    CMap* map;
    char currentState;
    size_t m_foodEaten;
    EDirection currentDirection;
};