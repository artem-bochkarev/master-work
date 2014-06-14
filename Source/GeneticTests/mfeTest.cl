#define GLOBAL_SIZE 1024

#define MAX_LOCAL_SIZE 256

#define CHECK_COUNT 32

#define DATA_TYPE float
#define MFE_COUNTERS_SIZE 8

#define DEBUG_ME (get_global_id(0)==0)

uint equals(const DATA_TYPE* a, const DATA_TYPE* b)
{
    return (*a == *b);
}

uint equalsDiff(__local const DATA_TYPE* a, __global const DATA_TYPE* b)
{
    return (*a == *b);
}

#include "GeneticTests/mfe.clh"

__kernel void genetic_1d( __global const float* inputFloats, __global float* mostCommon )
{
	uint myLocalId = get_local_id(0);
    uint myGlobalId = get_global_id(0);
	uint size = get_global_size(0);
    
    __local mfeGetter getters[MAX_LOCAL_SIZE];
    
    for (uint i=0; i<MFE_COUNTERS_SIZE; ++i)
    {
        getters[myLocalId].counters[i] = 0;
    }
    
    for (uint i=0; i<CHECK_COUNT; ++i)
    {
        uint pos = (myGlobalId + i) % size;
        incrementLocal( &getters[myLocalId], &inputFloats[pos] );
    }
    
    uint2 res = checkLocal( &getters[myLocalId] );
    mostCommon[myGlobalId] = getters[myLocalId].data[res.x];
}