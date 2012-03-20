#include "GeneticAPI/CMap.h"

class CMapImpl : public CMap
{
public:
    friend class CMapFactory;
    CMapImpl( size_t width, size_t geight, size_t foodCounter );
    CMapImpl( const CMapImpl& map );
    CMapImpl( const CMap* map );
    CMapImpl& operator = ( const CMapImpl& map );
    virtual std::vector<int> getInput( size_t x, size_t y, EDirection direct ) const;
    virtual double eatFood( size_t x, size_t y );
    virtual int get( int x, int y ) const;
    virtual size_t width() const;
    virtual void getLine( int * buf, size_t n ) const;
    virtual size_t height() const;
    virtual std::vector< std::pair<int, int> > getVisibleCells( int x, int y, EDirection direct ) const;
    virtual int getVisibleCells( int x, int y, EDirection direct, int* buffer, int size ) const;
    virtual size_t toCharBuffer( char* buffer, size_t max_size=0 ) const;
    virtual size_t toIntBuffer( int* buffer, size_t max_size=0 ) const;
    virtual ~CMapImpl();
private:
    void forwardOf( int& x, int& y, EDirection d) const;
    void leftOf( int& x, int& y, EDirection d) const;
    void rightOf( int& x, int& y, EDirection d) const;
    void pushToBuffer( int x, int y, int*& buffer ) const;
    size_t x_size, y_size;
    int** map;
};