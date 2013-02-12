#include "stdafx.h"
#include "CMapFactory.h"
#include "CMapImpl.h"
#include <fstream>

CMapPtr CMapFactory::getMap(const char *filename)
{
    std::ifstream in( filename );
    if ( !in.is_open() )
        return getRandomMap( 32, 32, 64 );
    int n, m;
    in >> n >> m;
    CMapImpl* map = new CMapImpl( n, m, 0 );
    if ( !in.is_open() )
        return CMapPtr( map );

    char tmp[300];
    in.getline( tmp, 250 );
    for ( int i=0; i<n; ++i )
    {
        in.getline( tmp, 250 );
        for ( int j=0; j<m; ++j )
            if ( tmp[j] != '_' )
                map->map[i][j] = 1;
    }
    in.close();
    return CMapPtr( map );
}

CMapPtr CMapFactory::getRandomMap(int n, int m, int cnt)
{
    CMapPtr ptr( new CMapImpl( n, m, cnt ) );
    return ptr;
}