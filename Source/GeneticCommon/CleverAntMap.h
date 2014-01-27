#pragma once
#include <vector>
#include "boost/smart_ptr/shared_ptr.hpp"

enum EMove
{
    Left,
    Right,
    Move
};

enum EDirection
{
    DLeft,
	DUp,
    DRight,
    DDown
};

EDirection left( EDirection );
EDirection right( EDirection );

class CMap
{
public:
    virtual std::vector<int> getInput( size_t x, size_t y, EDirection direct ) const = 0;
    virtual int eatFood( size_t x, size_t y ) = 0;
    virtual int get( int x, int y ) const = 0;
    virtual size_t width() const = 0;
    virtual size_t height() const = 0;
    virtual void getLine( int * buf, size_t n ) const = 0;
    virtual std::vector< std::pair<int, int> > getVisibleCells( int x, int y, EDirection direct ) const = 0;
    virtual int getVisibleCells( int x, int y, EDirection direct, int* buffer, int size ) const = 0;
    virtual size_t toCharBuffer( char* buffer, size_t max_size=0 ) const = 0;
    virtual size_t toIntBuffer( int* buffer, size_t max_size=0 ) const = 0;
    virtual ~CMap() {};
    CMap() {};
private:
    CMap( size_t width, size_t geight, size_t foodCounter );
    CMap( const CMap& map );
    CMap& operator = ( const CMap& map );
};

typedef boost::shared_ptr<CMap> CMapPtr;