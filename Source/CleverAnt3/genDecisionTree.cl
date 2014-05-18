//#version=2
//#type=shortTables
//version 2.0

#include "CleverAnt3/CleverAntCommon.hcl"

#define INPUT_PARAMS_COUNT 8
#define STATES_COUNT 4
#define COMMON_DATA_SIZE 4
#define NODE_SIZE 3
#define MAX_DEPTH 4
#define N (1 << MAX_DEPTH)
#define TREE_SIZE  (NODE_SIZE * (N * (N - 1) / 2))
#define STEPS_COUNT 100

uint2 getNext( const uint* input, __check_space const uint* treePtr )
{
    uint2 next;
	uint position = 0;
	uint tmp = 0;
	uint4 preget = 0;
	#pragma unroll
	for (uint depth = 0; depth < MAX_DEPTH; ++depth)
	{
		tmp = position*NODE_SIZE;
		preget = vload4(0, treePtr + tmp);
		uint currentVal = preget.x;
		position = select( position, preget.y, (currentVal)&&(input[currentVal-1]) );
		position = select( position, preget.z, (currentVal)&&(!input[currentVal-1]) );
	}
	next.x = preget.y;
	next.y = preget.z;
	return next;
}

float run( __check_space const uint* ind, __global int* map )
{
	float cnt = 0.0f;
	
	__global int * myMap = map + 2;
    
    uint x = 0, y = 0;
    uint direction = 2;
    uint curState = (*ind);
	
    for ( uint i=0; i<STEPS_COUNT; ++i )
    {
		uint input[8];
		getInput( input, x, y, direction, map );
		__check_space const uint* treePtr = ind + COMMON_DATA_SIZE + TREE_SIZE*curState;
		
        uint2 nex = getNext( input, treePtr );
        uint action = nex.y;
        curState = nex.x;
        
        x = moveXa( x, direction, action );
        y = moveYa( y, direction, action );
		
		uint f = x + c_x_size*y;
        cnt += myMap[f];
        myMap[f] = 0;
        direction = actionTurn( direction, action );
    }
    return cnt;
}

//constSizes[0] - statesCount
//constSizes[1] - stateSize
//constSizes[2] - mapSize
//constSizes[3] - gensNum

__kernel void genetic_1d( __check_space const uint* individs, __constant const uint* constSizes,
                         __global int* mapBuffer, __constant int* maps, __global float* resultCache )
{
    uint myPos = get_global_id(0);

	//if (myPos == 0)
	//	printf("begin run\n");
    uint mapSize = constSizes[4];
	uint bufSize = constSizes[2];
	uint bufOffset = constSizes[3];

	/*if (myPos == 0)
	{
		printf("AUTSIZE=%i, OFFSET=%i, MAPSIZE=%i\n", bufSize, bufOffset, mapSize);
		for (int i=0; i<20; ++i)
		{
			printf("%i, ", individs[i]);
		}
	}*/
    
    __global int* myMapsPtr = mapBuffer + myPos*mapSize;    
    __global const uint* myBuffer = individs + myPos*bufSize  + bufOffset;// = (1-koeff)*inBuffer + koeff*outBuffer;

	//make copy of a map
    /*uint s = 32 * 2;
    int16 tmp;
    __constant int* from = maps+2;
    __global int* to = myMapsPtr+2;
    for ( uint i=0; i < s; ++i )
    {
        tmp = vload16( i, from );
        vstore16( tmp, i, to );
    }*/

	//if (myPos == 0)
	//	printf("MY_BUFFER: %i, %i, %i, %i\n", myBuffer[0], myBuffer[1], myBuffer[2], myBuffer[3] );
    resultCache[ myPos ] = run( myBuffer, myMapsPtr );
	//if (myPos == 0)
	//	printf("end run, result=%f\n", resultCache[myPos]);
}
  
