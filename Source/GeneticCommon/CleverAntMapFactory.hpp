#pragma once
#include "CleverAntMapFactory.h"
#include "CleverAntMapImpl.h"
#include <fstream>
#include "Tools/errorMsg.hpp"
template<typename MAP_TYPE>
CMapPtr CMapFactory<MAP_TYPE>::getMap(const char *filename)
{
    std::ifstream in( filename );
    if ( !in.is_open() )
		Tools::throwDetailedFailed("[FAILED] to open MAP file: ", filename, 0);
        //return getRandomMap( 32, 32, 64 );
    int n, m;
    in >> n >> m;
	CMapImpl* map = new MAP_TYPE(n, m, 0);
    if ( !in.is_open() )
        return CMapPtr( map );

    char tmp[300];
    in.getline( tmp, 250 );
	size_t foodCnt = 0;
    for ( int i=0; i<n; ++i )
    {
		in.getline(tmp, 250);
		for (int j = 0; j < m; ++j)
		{
			if (tmp[j] == '+')
			{
				map->map[i][j] = 1;
				++foodCnt;
			}
		}
    }
	map->m_foodCnt = foodCnt;
    in.close();
    return CMapPtr( map );
}

template<typename MAP_TYPE>
CMapPtr CMapFactory<MAP_TYPE>::getRandomMap(int n, int m, int cnt)
{
    CMapPtr ptr( new MAP_TYPE( n, m, cnt ) );
    return ptr;
}