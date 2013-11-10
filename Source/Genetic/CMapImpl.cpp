#include "stdafx.h"
#include "CMapImpl.h"
#include <cassert>

const int VISIBLE_CELLS = 4;

CMapImpl::CMapImpl(size_t width, size_t height, size_t foodCounter)
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
        if ( map[y][x] > 0 )
            continue;
        foodCounter--;
        map[y][x] = 1;
    }
}

CMapImpl::CMapImpl(const CMapImpl &cmap)
:x_size(cmap.x_size), y_size(cmap.y_size)
{
    map = new int*[x_size];
    for ( size_t i=0; i<x_size; ++i)
    {
        map[i] = new int[y_size];
        memcpy( map[i], cmap.map[i], y_size*sizeof( int ) );
    }
}

CMapImpl::CMapImpl( const CMap* cmap )
:x_size(cmap->width()), y_size(cmap->height())
{
    map = new int*[x_size];
    for ( size_t i=0; i<x_size; ++i)
    {
        map[i] = new int[y_size];
        cmap->getLine( map[i], i );
    }
}

void CMapImpl::getLine( int * buf, size_t n ) const
{
    memcpy( buf, map[n], y_size*sizeof( int ) );
}

CMapImpl::~CMapImpl()
{
    for ( size_t i=0; i<x_size; ++i)
        delete[] map[i];
    delete[] map;
}

CMapImpl& CMapImpl::operator = ( const CMapImpl& cmap )
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

int CMapImpl::get(int x, int y) const
{
    return map[y][x];
}

int CMapImpl::eatFood(size_t x, size_t y)
{
    if ( map[y][x] > 0 )
    {
        map[y][x] = 0;
        return 1;
    }
    return 0;
}

size_t CMapImpl::height() const
{
    return y_size;
}

size_t CMapImpl::width() const
{
    return x_size;
}

void CMapImpl::forwardOf( int& x, int& y, EDirection d) const
{
    switch (d)
    {
    case(DLeft):
        if ( x == 0)
            x = width()-1;
        else
            --x;
        break;
    case(DUp):
        if ( y == 0)
            y = height()-1;
        else
            --y;
        break;
    case(DRight):
            x = (x+1) % width();
        break;
    case(DDown):
            y = (y+1) % height();
        break;
    }
}

void CMapImpl::leftOf( int& x, int& y, EDirection d) const
{
    d = left( d );
    forwardOf( x, y, d );
}

void CMapImpl::rightOf( int& x, int& y, EDirection d) const
{
    d = right( d );
    forwardOf( x, y, d );
}

void CMapImpl::pushToBuffer( int x, int y, int*& buffer ) const
{
    buffer[0] = x;
    buffer[1] = y;
    buffer += 2;
}

int CMapImpl::getVisibleCells( int x, int y, EDirection direct, int* buffer, int size ) const
{
    int x1(x), y1(y);
    forwardOf( x1, y1, direct );
    pushToBuffer( x1, y1, buffer );

    int x2(x1), y2(y1);
    leftOf( x2, y2, direct );
    pushToBuffer( x2, y2, buffer );

    int x3(x1), y3(y1);
    rightOf( x3, y3, direct );
    pushToBuffer( x3, y3, buffer );

    int x4(x1), y4(y1);
    forwardOf( x4, y4, direct );
    pushToBuffer( x4, y4, buffer );

    if ( size <= 4 )
        return 4;

    int x5(x), y5(y);
    leftOf( x5, y5, direct );
    pushToBuffer( x5, y5, buffer );

    int x6(x), y6(y);
    rightOf( x6, y6, direct );
    pushToBuffer( x6, y6, buffer );

    int x7(x5), y7(y5);
    leftOf( x7, y7, direct );
    pushToBuffer( x7, y7, buffer );

    int x8(x6), y8(y6);
    rightOf( x8, y8, direct );
    pushToBuffer( x8, y8, buffer );

    return 8;
}

std::vector< std::pair<int, int> > CMapImpl::getVisibleCells( int x, int y, EDirection direct ) const
{
    /*
         4
       2 1 3
     7 5 _ 6 8
    */
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


    /*
    int x5(x), y5(y);
    leftOf( x5, y5, direct );
    vec.push_back( std::make_pair( x5, y5 ) );

    int x6(x), y6(y);
    rightOf( x6, y6, direct );
    vec.push_back( std::make_pair( x6, y6 ) );

    int x7(x5), y7(y5);
    leftOf( x7, y7, direct );
    vec.push_back( std::make_pair( x7, y7 ) );

    int x8(x6), y8(y6);
    rightOf( x8, y8, direct );
    vec.push_back( std::make_pair( x8, y8 ) );*/

    return vec;
}

std::vector<int> CMapImpl::getInput( size_t x, size_t y, EDirection direct ) const
{
    
    /*std::vector< std::pair<int, int> > vec( getVisibleCells( x, y, direct ) );
    std::vector<int> res;
    for ( size_t i=0; i<vec.size(); ++i )
    {
        const std::pair<int, int>& pair( vec[i] );
        res.push_back( map[pair.second][pair.first] );
    }
    return res;*/
    
    int buffer[2 * VISIBLE_CELLS];
    int size = getVisibleCells( x, y, direct, buffer, VISIBLE_CELLS );
    std::vector<int> res;
    for ( int i=0; i<size; ++i )
    {
        res.push_back( map[ buffer[2*i + 1] ][ buffer[2*i] ] );
    }
    return res;
}

void CMapImpl::getInput( size_t x, size_t y, EDirection direct, int* output ) const
{   
    int buffer[2 * VISIBLE_CELLS];
    int size = getVisibleCells( x, y, direct, buffer, VISIBLE_CELLS );
    for ( int i=0; i<size; ++i )
    {
        output[i] = map[ buffer[2*i + 1] ][ buffer[2*i] ];
    }
}

size_t CMapImpl::toCharBuffer( char* buffer, size_t max_size ) const
{
   size_t realSize = x_size * y_size + 2;
   if ((max_size > 0)&&( realSize > max_size ))
       return 0;
   buffer[0] = (char)x_size;
   buffer[1] = (char)y_size;
   buffer += 2;
   for ( size_t i=0; i<y_size; ++i )
       for ( size_t j=0; j<x_size; ++j )
       {
           *buffer = (char)map[i][j];
           ++buffer;
       }
   return realSize;
}

size_t CMapImpl::toIntBuffer( int* buffer, size_t max_size ) const
{
   size_t realSize = x_size * y_size + 2;
   if ((max_size > 0)&&( realSize > max_size ))
       return 0;
   buffer[0] = (int)x_size;
   buffer[1] = (int)y_size;
   buffer += 2;
   for ( size_t i=0; i<y_size; ++i )
       for ( size_t j=0; j<x_size; ++j )
       {
           *buffer = map[i][j];
           ++buffer;
       }
   return realSize;
}