#pragma once
#include <boost/interprocess/detail/atomic.hpp>

typedef boost::uint32_t uint;

struct CLWrapSettings
{
    size_t N, M, step;
    size_t flowsCnt;
   // CRandomPtr rand;
    uint* inBuffer;
    uint* outBuffer;
    const uint* constSizes;
    uint* varValues;
    int* mapsBuffer;
    uint* tempBuffer;
    int* map;
    int* cache;
};

void genetic_2d( uint* stuff, uint* inBuffer, uint* outBuffer, 
                         const uint* constSizes, uint* varValues,
                         int* mapBuffer, uint* tempBuffer, 
                         int* maps, int* cache );