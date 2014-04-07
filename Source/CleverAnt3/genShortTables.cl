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
		
		x1 = moveX(x, left(direction)); y1 = moveY(y, left(direction));
		answ[4] = myMap[x1 + c_x_size*y1];
		int x2 = moveX(x, right(direction)); int y2 = moveY(y, right(direction));
		answ[5] = myMap[x2 + c_x_size*y2];
		
		x1 = moveX(x1, left(direction)); y1 = moveY(y1, left(direction));
		answ[6] = myMap[x1 + c_x_size*y1];
		x2 = moveX(x2, right(direction)); y2 = moveY(y2, right(direction));
		answ[7] = myMap[x2 + c_x_size*y2];		
	//#endif
}

uint getMask( __global const uint* maskPtr, uint index)
{
	uint k = index/4;
	uint res = maskPtr[k];
	uint diff = index - k*4;
	return (k >> 8*diff)&0x000000FF;
}

uint countIndex( const uint* in, __global const uint* maskPtr )
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

uint2 getNext( uint index, uint curState, __global const uint* ind )
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

float run( __global const uint* ind, __global int* map )
{
	float cnt = 0.0f;
	uint myId=get_global_id(0);
	
	__global int * myMap = map + 2;
	/*printf("\n");
	if (myId==0)
	{
		for (int i=0; i<c_y_size; ++i)
		{
			for (int j=0; j<c_x_size; ++j)
			{
				if (myMap[j + i*c_x_size] > 0)
					printf("+");
				else
					printf("_");
			}
			printf("\n");
		}
	}*/
    
    uint x = 0, y = 0;
    uint direction = 2;
    uint curState = (*ind)&0x000000FF;
    ind++;
    
	uint diff = COMMON_DATA_SIZE + STATE_SIZE_UINTS*curState;
	
	
	
	if (myId==0)
		printf("\t\t%i, %i, %i, %i, %i, %i, %i\n", ind[0], ind[1], ind[2], ind[3], ind[4], ind[5], ind[6]);
	
	if (myId==0)
		printf("\t%i, %i\n", x, y);
	
    for ( uint i=0; i<100; ++i )
    {
		uint input[8];
		getInput( input, x, y, direction, map );
		if (myId==0 && i<32)
			printf("\t  %i\n\t %i%i%i\n\t%i%iA%i%i\n", input[3], input[1], input[0], input[2], input[6], input[4], input[5], input[7] );
		uint index = countIndex( input, ind + diff );
        uint2 nex = getNext( index, curState, ind );
        uint action = nex.y;
        curState = nex.x;
		
        
        x = moveXa( x, direction, action );
        y = moveYa( y, direction, action );
		if (myId==0)
			printf("\t%i, %i\n", x, y);
		uint f = x + c_x_size*y;
		if (myId==0 && myMap[f] > 0)
			printf("eated!\n");
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

//varValues[0] - srand
//varValues[1] - last buf
__kernel void genetic_1d( __global const uint* individs, __global const uint* constSizes,
                         __global int* mapBuffer, __constant int* maps, __global float* resultCache )
{
    uint myPos = get_global_id(0);

	if (myPos == 0)
		printf("begin run\n");
    uint mapSize = constSizes[4];
	uint bufSize = constSizes[2];
	uint bufOffset = constSizes[3];

	if (myPos == 0)
	{
		printf("AUTSIZE=%i, OFFSET=%i, MAPSIZE=%i\n", bufSize, bufOffset, mapSize);
		for (int i=0; i<20; ++i)
		{
			printf("%i, ", individs[i]);
		}
	}
    uint myBuf = myPos * bufSize;

    
    __global int* myMapsPtr = mapBuffer + myPos*mapSize;    
    __global const uint* myBuffer = individs + myPos*bufSize  + bufOffset;// = (1-koeff)*inBuffer + koeff*outBuffer;

	//make copy of a map
    uint s = 32 * 2;
    int16 tmp;
    __constant int* from = maps+2;
    __global int* to = myMapsPtr+2;
    for ( uint i=0; i < s; ++i )
    {
        tmp = vload16( i, from );
        vstore16( tmp, i, to );
    }
	
	/*if (myPos==0)
	{
		printf("\n");
		for (int i=0; i<c_y_size; ++i)
		{
			for (int j=0; j<c_x_size; ++j)
			{
				if (from[j + i*c_x_size] > 0)
					printf("+");
				else
					printf("_");
			}
			printf("\n");
		}
	}*/
	
    resultCache[ myPos ] = run( myBuffer, myMapsPtr );
	if (myPos == 0)
		printf("end run, result=%f\n", resultCache[myPos]);
}
  
