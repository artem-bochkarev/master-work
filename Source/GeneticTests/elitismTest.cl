#define GLOBAL_SIZE 1024
#define GO_VALUE 16

__kernel void genetic_1d( __global float* inputFloats, __global uint* takeOrNot )
{
	uint myId = get_global_id(0);
	uint size = get_global_size(0);
    uint greater = 0;

	for (uint i=0; i<size; ++i)
	{
		uint isGreater = select(inputFloats[myId] < inputFloats[i], myId >= i,inputFloats[myId] == inputFloats[i]);
		greater = select(greater, greater + 1, isGreater);
	}
	takeOrNot[myId] = (greater <= GO_VALUE);
}