#include "stdafx.h"
#include "CTest.h"

EDirection left( EDirection d )
{
    switch (d)
    {
        case (DLeft):
            {
                return DBottom;
            }break;
        case (DTop):
            {
                return DLeft;
            }break;
        case (DRight):
            {
                return DTop;
            }break;
        case (DBottom):
            {
                return DRight;
            }break;
    }
    return DTop;
}

EDirection right( EDirection d )
{
    switch (d)
    {
        case (DLeft):
            {
                return DTop;
            }break;
        case (DTop):
            {
                return DRight;
            }break;
        case (DRight):
            {
                return DBottom;
            }break;
        case (DBottom):
            {
                return DLeft;
            }break;
    }
    return DTop;
}

CTest::CTest( CAutomat* automat, CMap* map, size_t moves, int x, int y, EDirection dir )
    :moves(moves), automat(automat), map(map), movesCnt(0), m_foodEaten(0), 
        currentState( automat->getStartState() ), currentDirection( dir ), x(x), y(y)
{
}

bool CTest::makeMove()
{
    if ( movesCnt >= moves-1 )
        return false;
    std::vector<int> input = map->getInput( x, y, currentDirection );
    char action = automat->getAction( currentState, &input );
    currentState = automat->getNextState( currentState, &input );
    switch (action)
    {
        case 0:
        {
            switch (currentDirection)
            {
            case(DLeft):
                x = ( x + map->width() - 1 ) % map->width();
                break;
            case(DTop):
                y = ( y + map->height() - 1 ) % map->height();
                break;
            case(DRight):
                    x = (x+1) % map->width();
                break;
            case(DBottom):
                    y = (y+1) % map->height();
                break;
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
        {
            int k = 0;
            ++k;
        }
    }
    movesCnt++;
    return true;
}

size_t CTest::movesMaked() const
{
    return movesCnt;
}

size_t CTest::foodEaten() const
{
    return m_foodEaten;
}

std::vector< std::pair<int, int> > CTest::getVisibleCells( const CMap *map, int x, int y, EDirection direct )
{
    return map->getVisibleCells( x, y, direct );
}

double CTest::run( CAutomat *automat, CMap *map, size_t moves, char * ptr )
{
    int x = 0, y = 0;
    int lem = 0;
    EDirection direct( DRight );
    double cnt = 0;
    char curState = automat->getStartState();
    for ( size_t i=0; i<moves; ++i )
    {
        std::vector<int> input = map->getInput( x, y, direct );
        char action = automat->getAction( curState, &input );
        if ( ptr != 0 )
        {
            ptr[100+i] = curState;
            if ( i == 26 )
            {
                ptr[100] = (char)input[0];
                ptr[101] = (char)input[1];
                ptr[102] = (char)input[2];
                ptr[103] = (char)input[3];
            }
            ptr[i] = action;
        }
        curState = automat->getNextState( curState, &input );
        switch (action)
        {
            case 0:
                {
                    switch (direct)
                    {
                    case(DLeft):
                        x = ( x + map->width() - 1 ) % map->width();
                        break;
                    case(DTop):
                        y = ( y + map->height() - 1 ) % map->height();
                        break;
                    case(DRight):
                        x = (x+1) % map->width();
                        break;
                    case(DBottom):
                        y = (y+1) % map->height();
                        break;
                    }
                double tmp = map->eatFood( x, y );
                if ( tmp > 0.5 )
                {
                    cnt += tmp;
                    lem = i;
                }
            }break;
            case 1:
            {
                direct = right( direct );
            }break;
            case 2:
            {
                direct = left( direct );
            }break;
            default:
            {
                int k = 0;
                ++k;
            }
        }
    }
    //return (cnt - lem*1.0/moves);
    return cnt;
}

const CMap* CTest::getArena() const
{
    return map;
}

int CTest::getX() const
{
    return x;
}

int CTest::getY() const
{
    return y;
}

EDirection CTest::getDir() const
{
    return currentDirection;
}