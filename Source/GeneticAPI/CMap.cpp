#include "CMap.h"
#include <cassert>

CMap::CMap(size_t width, size_t height, size_t foodCounter)
:x_size(width), y_size(height), map(0)
{
    map = new int*[width];
    for ( size_t i=0; i<width; ++i)
    {
        map[i] = new int[height];
        memset( map[i], 0, height*sizeof( int ) );
    }
    while ( foodCounter )
    {
        int x = rand()%width;
        int y = rand()%height;
        if ( map[x][y] > 0 )
            continue;
        foodCounter--;
        map[x][y] = 1;
    }
}

CMap::CMap(const CMap &cmap)
:x_size(cmap.x_size), y_size(cmap.y_size)
{
    map = new int*[x_size];
    for ( size_t i=0; i<x_size; ++i)
    {
        map[i] = new int[y_size];
        memcpy( map[i], cmap.map[i], y_size*sizeof( int ) );
    }
}

CMap::~CMap()
{
    for ( size_t i=0; i<x_size; ++i)
        delete[] map[i];
    delete[] map;
}

CMap& CMap::operator = ( const CMap& cmap )
{
    if ( map != 0 )
    {
        for ( size_t i=0; i<x_size; ++i)
        {
            delete[] map[i];
        }
        delete[] map;
    }
    x_size = cmap.x_size;
    y_size = cmap.y_size;
    map = new int*[x_size];
    for ( size_t i=0; i<x_size; ++i)
    {
        map[i] = new int[y_size];
        memcpy( map[i], cmap.map, y_size*sizeof( int ) );
    }
    return *this;
}

int CMap::get(int x, int y) const
{
    return map[x][y];
}

double CMap::eatFood(size_t x, size_t y)
{
    if ( map[x][y] > 0 )
    {
        map[x][y] = 0;
        return 1.0;
    }
    return 0.0;
}

size_t CMap::height() const
{
    return y_size;
}

size_t CMap::width() const
{
    return x_size;
}

void CMap::forwardOf( int& x, int& y, EDirection d) const
{
    switch (d)
    {
    case(DLeft):
        if ( x == 0)
            x = width()-1;
        else
            --x;
        break;
    case(DTop):
        if ( y == 0)
            y = height()-1;
        else
            --y;
        break;
    case(DRight):
            x = (x+1) % width();
        break;
    case(DBottom):
            y = (y+1) % height();
        break;
    }
}

void CMap::leftOf( int& x, int& y, EDirection d) const
{
    d = left( d );
    forwardOf( x, y, d );
}

void CMap::rightOf( int& x, int& y, EDirection d) const
{
    d = right( d );
    forwardOf( x, y, d );
}

std::vector< std::pair<int, int> > CMap::getVisibleCells( int x, int y, EDirection direct ) const
{
    assert( x >= 0);
    assert( y >= 0);
    std::vector< std::pair<int, int> > vec;
    int x1(x), y1(y);
    forwardOf( x1, y1, direct );
    vec.push_back( std::make_pair( x1, y1 ) );

    int x2 = x1, y2 = y1;
    leftOf( x2, y2, direct );
    vec.push_back( std::make_pair( x2, y2 ) );

    int x3 = x1, y3 = y1;
    rightOf( x3, y3, direct );
    vec.push_back( std::make_pair( x3, y3 ) );

    int x4 = x1, y4 = y1;
    forwardOf( x4, y4, direct );
    vec.push_back( std::make_pair( x4, y4 ) );

    return vec;
}

std::vector<int> CMap::getInput( size_t x, size_t y, EDirection direct ) const
{
    std::vector< std::pair<int, int> > vec( getVisibleCells( x, y, direct ) );
    std::vector<int> res;
    for ( size_t i=0; i<vec.size(); ++i )
    {
        const std::pair<int, int>& pair( vec[i] );
        res.push_back( map[pair.first][pair.second] );
    }
    return res;
}