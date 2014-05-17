#pragma once
#include "Test.h"
#include "GeneticCommon/CleverAntMapImpl.h"
#include "GeneticCommon/AutomatImpl.h"

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::CTest(CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat, CMap* map, size_t moves, int x, int y, EDirection dir)
    :moves(moves), automat(automat), map(map), movesCnt(0), m_foodEaten(0), 
        currentState( automat->getStartState() ), currentDirection( dir ), x(x), y(y)
{
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
bool CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::makeMove()
{
	if (moves < 1 + movesCnt)
        return false;
    //int input[4];
    //CMapImpl* mapImpl = static_cast<CMapImpl*>(map);
    //CAutomatImpl* autImpl = static_cast<CAutomatImpl*>(automat);
    //mapImpl->getInput( x, y, currentDirection, input );
	std::pair<COUNTERS_TYPE, COUNTERS_TYPE> next = automat->getNextStateAction(currentState, map->getInput(x, y, currentDirection));
    //char action = automat->getAction( currentState, map->getInput(x, y, currentDirection) );
	//currentState = automat->getNextState(currentState, map->getInput(x, y, currentDirection));
	int action = next.second;
	currentState = next.first;
    switch (action)
    {
        case 0:
        {
            switch (currentDirection)
            {
            case(DLeft):
                x = ( x + map->width() - 1 ) % map->width();
                break;
            case(DUp):
                y = ( y + map->height() - 1 ) % map->height();
                break;
            case(DRight):
                    x = (x+1) % map->width();
                break;
            case(DDown):
                    y = (y+1) % map->height();
                break;
			default:
				BOOST_ASSERT(0);
            }
            m_foodEaten += map->eatFood( x, y );
        }break;
        case 1:
        {
            currentDirection = right( currentDirection );
        }break;
        case 2:
        {
            currentDirection = left( currentDirection );
        }break;
		default:
			BOOST_ASSERT(0);
    }
    movesCnt++;
    return true;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
size_t CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::movesMaked() const
{
    return movesCnt;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
size_t CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::foodEaten() const
{
    return m_foodEaten;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
std::vector< std::pair<int, int> > CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getVisibleCells(const CMap *map, int x, int y, EDirection direct)
{
    return map->getVisibleCells( x, y, direct );
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
FITNES_TYPE CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::run(const CAutomat<COUNTERS_TYPE, INPUT_TYPE> *automat, CMap *map, size_t moves)
{
    int x = 0, y = 0;
    EDirection direct( DRight );
	FITNES_TYPE cnt = 0;
    CMapImpl* mapImpl = static_cast<CMapImpl*>(map);
    //const CAutomatImpl* autImpl = static_cast<const CAutomatImpl*>(automat);
    char curState = automat->getStartState();
    for ( size_t i=0; i<moves; ++i )
    {
        //int input[4];
		std::vector<INPUT_TYPE> input(8);
        mapImpl->getInput( x, y, direct, input.data() );
		/*if (i < 10)
		{
			printf("STEP = %i\n\tINPUT: ", i);
			for (int j = 0; j<8; ++j)
				printf("%i ", input[j]);
			printf("\n");
		}*/
        //char action = automat->getAction( curState, input );
        //curState = automat->getNextState( curState, input );
		std::pair<COUNTERS_TYPE, COUNTERS_TYPE> next = automat->getNextStateAction(curState, input);
		int action = next.second;
		curState = next.first;
		/*if (i < 10)
		{
			printf("\tCOORDS: %i, %i\n", x, y);
			printf("\tACTION = %i, NEXT_STATE = %i\n", action, curState);
		}*/
		
        switch (action)
        {
            case 0:
                {
                    switch (direct)
                    {
                    case(DLeft):
                        x = ( x + map->width() - 1 ) % map->width();
                        break;
                    case(DUp):
                        y = ( y + map->height() - 1 ) % map->height();
                        break;
                    case(DRight):
                        x = (x+1) % map->width();
                        break;
                    case(DDown):
                        y = (y+1) % map->height();
                        break;
                    }
                double tmp = map->eatFood( x, y );
                cnt += tmp;
            }break;
            case 1:
            {
                direct = right( direct );
            }break;
            case 2:
            {
                direct = left( direct );
            }break;
        }
    }
    //return (cnt - lem*1.0/moves);
    return cnt;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
const CMap* CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getArena() const
{
    return map;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
int CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getX() const
{
    return x;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
int CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getY() const
{
    return y;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
EDirection CTest<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getDir() const
{
    return currentDirection;
}