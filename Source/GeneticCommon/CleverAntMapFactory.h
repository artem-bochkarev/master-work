#include "CleverAntMap.h"

template<typename MAP_TYPE>
class CMapFactory
{
public:
    static CMapPtr getMap( const char* filename );
    static CMapPtr getRandomMap( int n, int m, int cnt );
};