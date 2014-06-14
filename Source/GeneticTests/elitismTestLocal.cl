#define GLOBAL_SIZE 1024
#define GO_VALUE 16

#define MAX_LOCAL_SIZE 256
#define MAX_LOCAL_COPY_COUNT 16

__kernel void genetic_1d( __global const float* inputFloats, __global uint* takeOrNot )
{
	uint myId = get_local_id(0);
	uint size = get_global_size(0);
    const uint localSize = get_local_size(0);
    uint greater = 0;
    uint goValue = GO_VALUE / get_num_groups(0);
    
    __local float tmpBuffer[MAX_LOCAL_SIZE];

    float16 tmp16;
    for ( uint i=0; i < MAX_LOCAL_COPY_COUNT; ++i )
    {
        tmp16 = vload16( i, inputFloats );
        vstore16( tmp16, i, tmpBuffer );
    }

	for (uint i=0; i<localSize; ++i)
	{
		uint isGreater = select(inputFloats[myId] < inputFloats[i], myId >= i,inputFloats[myId] == inputFloats[i]);
		greater = select(greater, greater + 1, isGreater);
	}
	takeOrNot[get_global_id(0)] = (greater <= goValue);
}