//#version=2
//#type=shortTables
//version 2.0

#define SHORT_TABLE_COLUMNS 4
#define INPUT_PARAMS_COUNT 4
#define MASK_SIZE INPUT_PARAMS_COUNT
#define STATES_COUNT 4
#define STATE_SIZE_BYTES 20
#define STATE_SIZE_UINTS 5
#define COMMON_DATA_SIZE 1


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

void setBit(uint* value, uint pos)
{
	*value |= (1 << pos);
}

void clearBit(uint* value, uint pos)
{
	*value &= ~(1 << pos);
}

void toogleBit(uint* value, uint pos)
{
	*value ^= (1 << pos);
}

uint checkBit(uint value, uint pos)
{
	return value & (1 << pos);
}

uint nextUInt( uint x )
{
    return ( rand_a*x + rand_c );
}

char16 nextChar16( char16 x )
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
    int a = (direction ) & 1;
    int c = ( (a) | action );
    int b = select( (int)direction - 1, 0, c );
	return ( x + b ) & m_x_size;
}

uint moveYa( uint y, uint direction, uint action )
{
    int a = (direction + 1) & 1;
    int c = ( (a) | action );
    int b = select( (int)direction - 2, 0, c );
	return ( y + b ) & m_y_size;
}

//#ifndef CPU_VERSION
//uint4 getInput( uint x, uint y, uint direction, const __global int* map )
//#else
void getInput( uint* answ, uint x, uint y, uint direction, const __global int* map )
//#endif
{
    const __global int * myMap = map + 2;
    int x1 = x, y1 = y;
    x1 = moveX( x, direction );
    y1 = moveY( y, direction );
	/*#ifndef CPU_VERSION
		uint4 out;
		out.x = myMap[x1 + c_x_size*y1];
		out.y = myMap[ moveX( x1, left( direction ) ) + c_x_size*moveY( y1, left( direction ) ) ];
		out.z = myMap[ moveX( x1, right( direction ) ) + c_x_size*moveY( y1, right( direction ) ) ];
		out.w = myMap[ moveX( x1, direction ) + c_x_size*moveY( y1, direction ) ];
		return out;
	#else*/
		answ[0] = myMap[x1 + c_x_size*y1];
		answ[1] = myMap[ moveX( x1, left( direction ) ) + c_x_size*moveY( y1, left( direction ) ) ];
		answ[2] = myMap[ moveX( x1, right( direction ) ) + c_x_size*moveY( y1, right( direction ) ) ];
		answ[3] = myMap[ moveX( x1, direction ) + c_x_size*moveY( y1, direction ) ];
	//#endif
}

uint getMask( const __check_space uint* maskPtr, uint index)
{
	uint k = index/4;
	uint res = maskPtr[k];
	uint diff = index - k*4;
	return (k >> 8*diff)&0x000000FF;
}

uint countIndex( const uint* in, const __check_space uint* maskPtr )
{
	uint k = 1;
	uint index = 0;
	for (uint i=0; i<INPUT_PARAMS_COUNT; ++i)
	{
		uint maskVal = getMask(maskPtr, i);
		uint a = select( (uint)0, k, maskVal );
		a = select((uint)0, k, in[i]);
		index += k;
		k = select(k, 2*k, getMask(maskPtr, i));
	}
    return index;
}

uint2 getNext( uint index, uint curState, const __check_space uint* ind )
{
    uint2 next;
    uint diff = COMMON_DATA_SIZE + STATE_SIZE_UINTS*curState + MASK_SIZE;
	uint step = index/2;
	uint shift = index - step*2;
	next.x = ind[ diff + step ];
	next.y = ind[ diff + step ];
	diff = shift*16;
    next.x = (next.x >> diff)&0x000000FF;
	diff += 8;
	next.y = (next.y >> diff)&0x000000FF;
    return next;
}

float run( __check_space uint* ind, __global int* map )
{
    __global int * myMap = map + 2;
    uint x = 0, y = 0;
    uint direction = 2;
    uint curState = (*ind)&0x000000FF;
    ind++;
    float cnt = 0.0f;
	uint diff = COMMON_DATA_SIZE + STATE_SIZE_UINTS*curState;

    for ( uint i=0; i<100; ++i )
    {
		uint input[4];
		getInput( input, x, y, direction, map );
		uint index = countIndex( input, ind + diff );
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

/*#ifndef CPU_VERSION
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
#else*/

uint createParentIndex(const uint* toParent, const uint* myArray, uint index)
{
	uint maxIndex = 1 << SHORT_TABLE_COLUMNS;
	uint parentIndex = index;// rand->nextUINT()&(maxIndex - 1);
	for (uint i = 0; i < SHORT_TABLE_COLUMNS; ++i)
	{
		uint myBit = checkBit(index, myArray[i]);
		if (toParent[i])
		{
			if (myBit)
				setBit(&parentIndex, toParent[i] - 1);
			else
				clearBit(&parentIndex, toParent[i] - 1);
		}
	}
	return parentIndex;
}

void toParent(uint* toParent, const uint* myMas, const __check_space uint* parentMask)
{
	for (uint i = 0; i < SHORT_TABLE_COLUMNS; ++i)
		toParent[i] = 0;
	uint j = 1, k = 0;
	for (uint i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		k = select(k, k+1, (i > myMas[k] && k < SHORT_TABLE_COLUMNS) );
		toParent[k] = select( toParent[k], j, (getMask(parentMask, i) && (myMas[k]==i)) );
		j = select( j, j+1, getMask(parentMask, i) );
	}
}

uint crossMasks(__check_space uint* childMask, const __check_space uint* motherMask, const __check_space uint* fatherMask, uint random_value)
{
	uint oneEnabled = 0, enabled = 0;
	for (uint i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		childMask[i] = select( 0, 1, (motherMask[i] && fatherMask[i]) );
		enabled = select( enabled, enabled + 1, (motherMask[i] && fatherMask[i]) );
		//oneEnabled = ( oneEnabled, oneEnabled + 1, (motherMask[i] || fatherMask[i]) );
	}
	uint bothEnabled = enabled;

	for (uint i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		uint res = ((random_value & 255) < (255 / (SHORT_TABLE_COLUMNS - bothEnabled + 1))) ? 1 : 0;
		random_value = nextUInt(random_value);
		uint b = ((childMask[i] == 0) && (motherMask[i] || fatherMask[i]) && (enabled < SHORT_TABLE_COLUMNS) && res);
		enabled = select( enabled, enabled + 1, b );
		childMask[i] = select( 0, 1, b );
	}
	return random_value;
}

void setRecord( __check_space uint* childTable, const __check_space uint* parentTable, uint childIndex, uint parentIndex )
{
	uint ci = childIndex / 2;
	uint pi = parentIndex / 2;
	uint newVal = select( 0x0000FFFF & parentTable[pi], 0x0000FFFF & (parentTable[pi] >> 16), parentIndex & 2 );
	uint tmp = select( (childTable[ci] & 0x0000FFFF) | (newVal << 16), (childTable[ci] & 0xFFFF0000) | newVal, childIndex & 2 );
	childTable[ci] = tmp;
}

uint crossTables(__check_space uint* childMask, const __check_space uint* motherMask, const __check_space uint* fatherMask, uint random_value)
{
	uint myMas[SHORT_TABLE_COLUMNS], j = 0;
	for (uint i = 0; i < SHORT_TABLE_COLUMNS; ++i)
		myMas[i] = 0;
	
	for (uint i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (childMask[i])
			myMas[j++] = i;
	}

	uint toMother[SHORT_TABLE_COLUMNS], toFather[SHORT_TABLE_COLUMNS];
	toParent(toMother, myMas, motherMask);
	toParent(toFather, myMas, fatherMask);

	uint maxIndex = 1 << SHORT_TABLE_COLUMNS;
	for (uint index = 0; index < maxIndex; ++index)
	{
		uint motherIndex = createParentIndex(toMother, myMas, index);
		uint fatherIndex = createParentIndex(toFather, myMas, index);
		__check_space uint * childTable = childMask + MASK_SIZE;
		const __check_space uint * motherTable = motherMask + MASK_SIZE;
		const __check_space uint * fatherTable = fatherMask + MASK_SIZE;
		const __check_space uint* parentTable = (const __check_space uint*)select((uint)motherTable, (uint)fatherTable, (random_value&255)>127);
		uint parentIndex = select((uint)motherIndex, (uint)fatherIndex, (random_value&255)>127);
		random_value = nextUInt(random_value);
		setRecord(childTable, parentTable, index,  parentIndex );
	}
	return random_value;
}

uint crossover( __check_space uint* buffer, const __check_space uint* motherPtr, const __check_space uint* fatherPtr, uint random_value)
{
	for (size_t currentState = 0; currentState < STATES_COUNT; ++currentState)
	{
		const __check_space uint* motherMask = motherPtr + COMMON_DATA_SIZE + currentState * (STATE_SIZE_UINTS);
		const __check_space uint* fatherMask = fatherPtr + COMMON_DATA_SIZE + currentState * (STATE_SIZE_UINTS);
		__check_space uint* childMask  = buffer + COMMON_DATA_SIZE + currentState * (STATE_SIZE_UINTS);

		random_value = crossMasks(childMask, motherMask, fatherMask, random_value);
		random_value = crossTables(childMask, motherMask, fatherMask, random_value);
	}
	return random_value;
}

uint crossThem( uint bufSize, uint myBuf, uint myBufLocal, __check_space uint* tempBuffer, 
              uint hisBuf, __global uint* inBuffer, uint random_value )
{
	/*for ( uint i=0; i < bufSize; ++i )
    {
        random_value = nextInt( random_value );
        uint res = select( myBuf, hisBuf, (random_value & 512) ); 
        tempBuffer[ myBuf + i ] = inBuffer[ res + i ];
    }*/
	//crossover( tempBuffer + myBuf, inBuffer + hisBuf, inBuffer + myBuf, random_value);
	return random_value;
}
//#endif

float tryHim( uint bufSize, uint myBuf, uint myBufLocal, __check_space uint* tempBuffer, 
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

    float result = run( tempBuffer + myBufLocal, myMaps );
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

    uint mapSize = constSizes[2];
	uint bufSize = COMMON_DATA_SIZE + STATE_SIZE_UINTS * STATES_COUNT;

    uint myPos = coord_global.x*size_global.y + coord_global.y;
	uint myPosLocal = coord_local.x*size_local.y + coord_local.y;
    uint myBuf = myPos * bufSize;
	uint myBufLocal = myPosLocal * bufSize;
    uint srand = varValues[0];
    uint random_value = nextUInt( srand+(srand*coord_global.x - srand*coord_global.y)%(size_global.x*size_global.y) );
	
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
		#ifdef CPU_VERSION 
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value );
		#else
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value, random16 );
		#endif
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, myBufLocal, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        // with up of him
        hisPos = (( coord_global.x + size_global.x - 1)%size_global.x)*size_global.y + coord_global.y;
        hisBuf = hisPos * bufSize;
        #ifdef CPU_VERSION 
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value );
		#else
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value, random16 );
		#endif
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, myBufLocal, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        //with right of him
        hisPos = coord_global.x*size_global.y + ( coord_global.y + 1)%size_global.y;
        hisBuf = hisPos * bufSize;
        #ifdef CPU_VERSION 
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value );
		#else
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value, random16 );
		#endif
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, myBufLocal, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        //with down of him
        hisPos = (( coord_global.x + 1)%size_global.x)*size_global.y + coord_global.y;
        hisBuf = hisPos * bufSize;
        #ifdef CPU_VERSION 
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value );
		#else
			random_value = crossThem( bufSize, myBuf, myBufLocal, tempBuffer, hisBuf, inputBuffer, random_value, random16 );
		#endif
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, myBufLocal, tempBuffer, result, myMapsPtr, outputBuffer, maps );    


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
  
