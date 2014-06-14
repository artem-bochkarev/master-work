#define GLOBAL_SIZE 1024
#define GO_VALUE 1
#define CHECK_COUNT 64

#define SRAND 0xAB123E0D

#include "GeneticTests/random.clh"

__kernel void genetic_1d( __global float* inputFloats, __global uint* takeOrNot )
{
	uint myId = get_global_id(0);
	uint size = get_global_size(0);
    uint greater = 0;
    
    uint rand = nextUINT( get_global_id(0)+(SRAND*get_global_id(0)) );

	for (uint t=0; t<CHECK_COUNT; ++t)
	{
        uint i = getValue1024(rand, GLOBAL_SIZE);
        rand = nextUINT(rand);
		uint isGreater = select(inputFloats[myId] < inputFloats[i], myId >= i,inputFloats[myId] == inputFloats[i]);
		greater = select(greater, greater + 1, isGreater);
	}
	takeOrNot[myId] = (greater < GO_VALUE);
}