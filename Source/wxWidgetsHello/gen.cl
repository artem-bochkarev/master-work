//#version=2
//version 2.0


// rand like in MSVC
__constant uint rand_m = 0x80000000;
__constant uint rand_a = 214013;
__constant uint rand_c = 2531011;

//probablility in % (0..100)
__constant uint mutation_probability = 0;
__constant uint actions_count = 3;

#define c_x_size 32
#define c_y_size 32 
#define m_x_size 31
#define m_y_size 31

uint nextInt( uint x )
{
    return ( rand_a*x + rand_c );
}

char16 nextUChar16( char16 x )
{
    char16 a = (char16)(17);
    char16 b = (char16)(31);
    return mad_hi( x, a, b);
}

uint left( uint direction )
{
    return ( direction - 1 )&3;
}
uint right( uint direction )
{
    return ( 1 + direction )&3;
}

uint actionTurn( uint direction, uint action )
{
    int res = select( 0, -1, action == 2 );
    res = select( res, 1, action == 1 );
	return ( direction + res )&3;
}

//0 - left
//1 - up
//2 - right
//3 - down

/*

y 

^
|
|
|
|
|
L------------------------------>  x
*/
uint moveX( uint x, uint direction )
{
    int b = select( (int)direction - 1, 0, (direction & 1) != 0 );
	return ( x + b ) & m_x_size;
}

uint moveY( uint y, uint direction )
{
    int b = select( (int)direction - 2, 0, (direction & 1) == 0 );
	return ( y + b ) & m_y_size;
}

uint moveXa( uint x, uint direction, uint action )
{
    uint x1 = moveX(x, direction);
	uint res = select( x, x1, (action == 0) );
	return res;
}

uint moveYa( uint y, uint direction, uint action )
{
	uint y1 = moveY(y, direction);
	uint res = select( y, y1, (action == 0) );
	return res;
}

#ifndef CPU_VERSION
uint4 getInput( uint x, uint y, uint direction, const __global int* map )
#else
void getInput( uint* answ, uint x, uint y, uint direction, const __global int* map )
#endif
{
    const __global int * myMap = map + 2;
    int x1 = x, y1 = y;
    x1 = moveX( x, direction );
    y1 = moveY( y, direction );
	#ifndef CPU_VERSION
		uint4 out;
		out.x = myMap[x1 + c_x_size*y1];
		out.y = myMap[ moveX( x1, left( direction ) ) + c_x_size*moveY( y1, left( direction ) ) ];
		out.z = myMap[ moveX( x1, right( direction ) ) + c_x_size*moveY( y1, right( direction ) ) ];
		out.w = myMap[ moveX( x1, direction ) + c_x_size*moveY( y1, direction ) ];
		return out;
	#else
		answ[0] = myMap[x1 + c_x_size*y1];
		answ[1] = myMap[ moveX( x1, left( direction ) ) + c_x_size*moveY( y1, left( direction ) ) ];
		answ[2] = myMap[ moveX( x1, right( direction ) ) + c_x_size*moveY( y1, right( direction ) ) ];
		answ[3] = myMap[ moveX( x1, direction ) + c_x_size*moveY( y1, direction ) ];
	#endif
}

#ifndef CPU_VERSION
uint countIndex( uint4 in, uint stateSize )
#else
uint countIndex( uint* in, uint stateSize )
#endif
{
	#ifndef CPU_VERSION
		uint res = mad_sat( (uint)2, in.y, in.x );
		res = mad_sat( (uint)4, in.z, res );
		res = mad_sat( (uint)8, in.w, res );
	#else
		uint res = mad_sat( (uint)2, in[1], in[0] );
		res = mad_sat( (uint)4, in[2], res );
		res = mad_sat( (uint)8, in[3], res );
	#endif
    return res;//%stateSize;
}

uint2 getNext( uint index, uint curState, const __check_space uint* ind )
{
    uint2 next;
    uint diff = 8*curState;
	uint step = index/4;
	uint shift = index - step*4;
	next.x = ind[ diff + step ];
	next.y = ind[ diff + step + 4 ];
	diff = shift*8;
    next = (next >> diff)&0x000000FF;
    return next;
}

float run( __check_space uint* ind, const uint statesCount, const uint stateSize, __global int* map )
{
    __global int * myMap = map + 2;
    uint x = 0, y = 0;
    uint direction = 2;
    uint curState = (*ind)&0x000000FF;
    ind++;
    float cnt = 0.0f;

    for ( uint i=0; i<100; ++i )
    {
		#ifndef CPU_VERSION
			uint4 input = getInput( x, y, direction, map );
		#else
			uint input[4];
			getInput( input, x, y, direction, map );
		#endif
		uint index = countIndex( input, stateSize );
        uint2 nex = getNext( index, curState, ind );
        uint action = nex.y;
        curState = nex.x;
        
        x = moveXa( x, direction, action );
        y = moveYa( y, direction, action );
        cnt += myMap[x + c_x_size*y];
        myMap[x + c_x_size*y] = 0;
        direction = actionTurn( direction, action );
    }
    return cnt;
}

/*uint mutateHim( uint statesCount, uint stateSize, 
               __local uint* automatPtr, uint random_value )
{
    random_value = nextInt( random_value );

    if ( random_value%100 < mutation_probability )
    {
        random_value = nextInt( random_value );
        uint pos = random_value%statesCount;
        __local char* ptrStates = automatPtr + 1 + 2*stateSize*pos;
        __local char* ptrActions = ptrStates + stateSize;
        for ( size_t j=0; j<stateSize; ++j )
        {
            random_value = nextInt( random_value );
            *(ptrStates + j) = random_value % statesCount;

            random_value = nextInt( random_value );
            *(ptrActions + j) = random_value % actions_count;
        }

        random_value = nextInt( random_value );
        if ( random_value % 100 > 15 )
            *(automatPtr) = (char)(random_value % statesCount);
    }

    return random_value;
}*/

void memcpy_from_local( __global uint* to, const __check_space uint* from, uint count )
{
    uint16 tmp16;
    uint cnt = count / 16;
    for ( uint i=0; i < cnt; ++i )
    {
        tmp16 = vload16( i, from );
        vstore16( tmp16, i, to );
    }
    count -= cnt*16;
    __global uint* ptrTo = to + cnt*16;
    const __check_space uint* ptrFrom = from + cnt*16;
    for ( uint i=0; i<count; ++i )
        ptrTo[i] = ptrFrom[i];
}

void memcpy_to_local( __check_space uint* to, const __global uint* from, uint count )
{
    uint16 tmp16;
    uint cnt = count / 16;
    for ( uint i=0; i < cnt; ++i )
    {
        tmp16 = vload16( i, from );
        vstore16( tmp16, i, to );
    }
    count -= cnt*16;
    __check_space uint* ptrTo = to + cnt*16;
    const __global uint* ptrFrom = from + cnt*16;
    for ( uint i=0; i<count; ++i )
        ptrTo[i] = ptrFrom[i];
}

#ifndef CPU_VERSION
uint crossThem( uint bufSize, uint myBuf, uint myBufLocal, __check_space uint* tempBuffer, 
              uint hisBuf, __global uint* inBuffer, uint random_value, char16 random16 )
			  {
	uint cnt = bufSize/4;
    for ( uint i=0; i < cnt; ++i )
    {
        char16 father = as_char16(vload4( i, inBuffer + myBuf));
		char16 mother = as_char16(vload4( i, inBuffer + hisBuf));
        char16 res = select( father, mother, (random16 << (char)3) );
		random16 = nextUChar16( random16 );
		vstore4( as_uint4(res), i, tempBuffer + myBufLocal );
    }
	for ( uint i=cnt*4; i<bufSize; ++i )
	{
		random_value = nextInt( random_value );
        uint res = select( myBuf, hisBuf, (random_value & 512) ); 
        tempBuffer[ myBufLocal + i ] = inBuffer[ res + i ];
	}
    return random_value;
}
#else
uint crossThem( uint bufSize, uint myBuf, uint myBufLocal, __check_space uint* tempBuffer, 
              uint hisBuf, __global uint* inBuffer, uint random_value )
{
	for ( uint i=0; i < bufSize; ++i )
    {
        random_value = nextInt( random_value );
        uint res = select( myBuf, hisBuf, (random_value & 512) ); 
        tempBuffer[ myBuf + i ] = inBuffer[ res + i ];
    }
	return random_value;
}
#endif

float tryHim( uint bufSize, uint myBuf, uint myBufLocal, uint statesCount, uint stateSize, __check_space uint* tempBuffer, 
             float bestResult, __global int* myMaps, __global uint* outBuffer, __constant int* maps )
{
    //make copy of a map
    uint s = 32 * 2;
    int16 tmp;
    __constant int* from = maps+2;
    __global int* to = myMaps+2;
    for ( uint i=0; i < s; ++i )
    {
        tmp = vload16( i, from );
        vstore16( tmp, i, to );
    }

    float result = run( tempBuffer + myBufLocal, statesCount, stateSize, myMaps );
    if ( result > bestResult )
    {
        bestResult = result;
        memcpy_from_local( outBuffer + myBuf, tempBuffer + myBufLocal, bufSize );
    }
    return bestResult;
}
//constSizes[0] - statesCount
//constSizes[1] - stateSize
//constSizes[2] - mapSize
//constSizes[3] - gensNum

//varValues[0] - srand
//varValues[1] - last buf
__kernel void genetic_2d( __global uint* inBuffer, __global uint* outBuffer, 
                         __global const uint* constSizes, __global uint* varValues,
                         __global int* mapBuffer, __constant int* maps, 
						 __global float* resultCache, __global uint* globalTempBuffer )
{
    uint2 size_global, size_local;
	size_global.x = get_global_size(0);
	size_global.y = get_global_size(1);
    size_local.x = get_local_size(0);
	size_local.y = get_local_size(1);

	uint2 coord_global, coord_local;
    coord_global.x = get_global_id(0);
    coord_global.y = get_global_id(1);
	coord_local.x = get_local_id(0);
    coord_local.y = get_local_id(1);

    uint statesCount = constSizes[0];
    uint stateSize = constSizes[1];
    uint mapSize = constSizes[2];

    uint sizeInBytes = 4 + 2 * statesCount * stateSize;
	uint bufSize = sizeInBytes / 4;

    uint myPos = coord_global.x*size_global.y + coord_global.y;
	uint myPosLocal = coord_local.x*size_local.y + coord_local.y;
    uint myBuf = myPos * bufSize;
	uint myBufLocal = myPosLocal * bufSize;
    uint srand = varValues[0];
    uint random_value = nextInt( srand+(srand*coord_global.x - srand*coord_global.y)%(size_global.x*size_global.y) );
	
	#ifndef CPU_VERSION
		uint4 rand;
		random_value = nextInt( random_value );
		rand.x = random_value;
		random_value = nextInt( random_value );
		rand.y = random_value;
		random_value = nextInt( random_value );
		rand.z = random_value;
		random_value = nextInt( random_value );
		rand.w = random_value;
		char16 random16 = as_char16(rand);
		//ToDo: calls!!
	#endif

    __local float cache[ WORK_GROUP_SIZE ];
    //__local uint bestResults[ 16 ];
    //__local float sumResults[ 16 ];
	#ifdef ENOUGH_LOCAL_MEMORY
		__local uint tempBuffer[ BUFFER_SIZE ];
	#else
		__global uint* tempBuffer = globalTempBuffer;
	#endif

    __global int* myMapsPtr = mapBuffer + myPos*mapSize;    
    
    __global uint* inputBuffer = inBuffer;// = (1-koeff)*inBuffer + koeff*outBuffer;
    __global uint* outputBuffer = outBuffer;// = (1-koeff)*outBuffer + koeff*inBuffer;

    //uint gensNum = constSizes[3];

    for ( uint counter = 0; counter < 2; ++counter )
    {
        float result = -1.0f;
        //try me
        uint hisPos = coord_global.x*size_global.y + coord_global.y;
        uint hisBuf = hisPos * bufSize;
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        //bestResult = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, bestResult, myMapsPtr, outputBuffer, ptr+myPos*200 );    

        // with left of him
        // myMapsPtr += mapSize;
        hisPos = coord_global.x*size_global.y + ( coord_global.y + size_global.y - 1)%size_global.y;
        hisBuf = hisPos * bufSize;
		#ifndef CPU_VERSION 
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value, random16 );
		#else
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value );
		#endif
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, myBufLocal, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        // with up of him
        hisPos = (( coord_global.x + size_global.x - 1)%size_global.x)*size_global.y + coord_global.y;
        hisBuf = hisPos * bufSize;
        #ifndef CPU_VERSION 
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value, random16 );
		#else
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value );
		#endif
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, myBufLocal, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        //with right of him
        hisPos = coord_global.x*size_global.y + ( coord_global.y + 1)%size_global.y;
        hisBuf = hisPos * bufSize;
        #ifndef CPU_VERSION 
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value, random16 );
		#else
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value );
		#endif
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, myBufLocal, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        //with down of him
        hisPos = (( coord_global.x + 1)%size_global.x)*size_global.y + coord_global.y;
        hisBuf = hisPos * bufSize;
        #ifndef CPU_VERSION 
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value, random16 );
		#else
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value );
		#endif
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, myBufLocal, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    


        cache[myPosLocal] = result;
		resultCache[counter*size_global.x*size_global.y + myPos] = result;
		//ToDo next!
        barrier( CLK_GLOBAL_MEM_FENCE );
		//swap buffers:
        __global uint* tmp = inputBuffer;
        inputBuffer = outputBuffer;
        outputBuffer = tmp;
    }
}
  
