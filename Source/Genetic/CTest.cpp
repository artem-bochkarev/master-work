#include "stdafx.h"
#include "CTest.h"
#include "CMapImpl.h"
#include "CAutomatImpl.h"

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
    int input[4];
    CMapImpl* mapImpl = static_cast<CMapImpl*>(map);
    CAutomatImpl* autImpl = static_cast<CAutomatImpl*>(automat);
    mapImpl->getInput( x, y, currentDirection, input );
    char action = autImpl->getAction( currentState, input );
    currentState = autImpl->getNextState( currentState, input );
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

double CTest::run( const CAutomat *automat, CMap *map, size_t moves )
{
    int x = 0, y = 0;
    EDirection direct( DRight );
    double cnt = 0;
    CMapImpl* mapImpl = static_cast<CMapImpl*>(map);
    const CAutomatImpl* autImpl = static_cast<const CAutomatImpl*>(automat);
    char curState = automat->getStartState();
    for ( size_t i=0; i<moves; ++i )
    {
        int input[4];
        mapImpl->getInput( x, y, direct, input );
        char action = autImpl->getAction( curState, input );
        curState = autImpl->getNextState( curState, input );
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