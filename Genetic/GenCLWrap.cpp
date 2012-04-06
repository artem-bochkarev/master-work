#include "stdafx.h"
#include "GenCLWrap.h"
#include <string.h>

// rand like in MSVC
const uint rand_m = 0x80000000;
const uint rand_a = 214013;
const uint rand_c = 2531011;

//probablility in % (0..100)
const uint mutation_probability = 0;
const uint actions_count = 3;
const uint max_size = 20;

#define c_x_size 32
#define c_y_size 32
#define m_x_size 31
#define m_y_size 31

uint nextInt( uint x )
{
    return ( rand_a*x + rand_c );// % rand_m;
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
    //int res = select( 0, -1, action == 2 );
    int res = (action == 2) ? -1 : 0;
    //res = select( res, 1, action == 1 );
	res = (action == 1) ? 1 : res;
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
    //int b = select( (int)direction - 1, 0, (direction & 1) != 0 );
	int b = ( (direction & 1) != 0 ) ? 0 : (int)direction - 1;
	return ( x + b ) & m_x_size;
}

uint moveY( uint y, uint direction )
{
    //int b = select( (int)direction - 2, 0, (direction & 1) == 0 );
	int b = ( (direction & 1) == 0 ) ? 0 : (int)direction - 2;
	return ( y + b ) & m_y_size;
}

uint moveXa( uint x, uint direction, uint action )
{
    int a = (direction ) & 1;
    int c = ( (a) | action );
    //int b = select( (int)direction - 1, 0, c );
	int b = ( c ) ? 0 : (int)direction - 1;
	return ( x + b ) & m_x_size;
}

uint moveYa( uint y, uint direction, uint action )
{
    int a = (direction + 1) & 1;
    int c = ( (a) | action );
    //int b = select( (int)direction - 2, 0, c );
	int b = ( c ) ? 0 : (int)direction - 2;
	return ( y + b ) & m_y_size;
}

void getInput( uint* buf, uint x, uint y, uint direction, const int* map )
{
    const int * myMap = map + 2;
    int x1 = x, y1 = y;
    x1 = moveX( x, direction );
    y1 = moveY( y, direction );
    buf[0] = myMap[x1 + c_x_size*y1];
    buf[1] = myMap[ moveX( x1, left( direction ) ) + c_x_size*moveY( y1, left( direction ) ) ];
    buf[2] = myMap[ moveX( x1, right( direction ) ) + c_x_size*moveY( y1, right( direction ) ) ];
    buf[3] = myMap[ moveX( x1, direction ) + c_x_size*moveY( y1, direction ) ];
}

uint countIndex( uint* buf, uint stateSize )
{
    /*uint res = mad_sat( (uint)2, buf[1], buf[0] );
    res = mad_sat( (uint)4, buf[2], res );
    res = mad_sat( (uint)8, buf[3], res );*/
	uint res = buf[0] + buf[1]*2 + buf[2]*4 + buf[3]*8;
    return res;//%stateSize;
}

void getNext( uint* buf, uint index, uint curState, const uint* ind )
{
    uint diff = 8*curState;
	uint step = index/4;
	uint shift = index - step*4;
	buf[0] = ind[ diff + step ];
	buf[1] = ind[ diff + step + 4 ];
	diff = shift*8;
    buf[0] = (buf[0] >> diff)&0x000000FF;
    buf[1] = (buf[1] >> diff)&0x000000FF;
}

int run( uint* ind, const uint statesCount, const uint stateSize, int* map )
{
    int * myMap = map + 2;
    uint x = 0, y = 0;
    uint direction = 2;
    uint curState = (*ind)&0x000000FF;
    ind++;
    int cnt = 0;

    for ( uint i=0; i<100; ++i )
    {
        uint input[4];
        getInput( input, x, y, direction, map );
        uint index = countIndex( input, stateSize );
        uint nex[2];
        getNext( nex, index, curState, ind );
        uint action = nex[1];
        curState = nex[0];
        
        x = moveXa( x, direction, action );
        y = moveYa( y, direction, action );
        cnt += myMap[x + c_x_size*y];
        myMap[x + c_x_size*y] = 0;
        direction = actionTurn( direction, action );
    }
    return cnt;
}

/*uint mutateHim( uint statesCount, uint stateSize, 
               __local char* automatPtr, uint random_value )
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

void memcpy_from_local( uint* to, const uint* from, uint count )
{
    memcpy( to, from, count*4 );
}

uint crossThem( uint bufSize, uint myBuf, uint* tempBuffer, 
              uint hisBuf, uint* inBuffer, uint random_value )
{
	//difficult
    for ( uint i=0; i < bufSize; ++i )
    {
        random_value = nextInt( random_value );
        //uint res = select( myBuf, hisBuf, (random_value & 512) ); 
        uint res = (random_value & 512) ? hisBuf : myBuf;
        tempBuffer[ myBuf + i ] = inBuffer[ res + i ];
    }
    return random_value;
}

int tryHim( uint bufSize, uint myBuf, uint statesCount, uint stateSize, uint* tempBuffer, 
             int bestResult, int* myMaps, uint* outBuffer, int* maps )
{
    //make copy of a map
    uint s = 32 * 32;
    int* from = maps+2;
    int* to = myMaps+2;
    for ( uint i=0; i < s; ++i )
    {
        to[i] = from[i];
    }

    int result = run( tempBuffer + myBuf, statesCount, stateSize, myMaps );
    if ( result > bestResult )
    {
        bestResult = result;
        memcpy_from_local( outBuffer + myBuf, tempBuffer + myBuf, bufSize );
    }
    return bestResult;
}
//constSizes[0] - statesCount
//constSizes[1] - stateSize
//constSizes[2] - mapSize
//constSizes[3] - gensNum

//varValues[0] - srand
//varValues[1] - last buf
void genetic_2d( uint* stuff, uint* inBuffer, uint* outBuffer, 
                         const uint* constSizes, uint* varValues,
                         int* mapBuffer, uint* tempBuffer, 
                         int* maps, int* cache )
{
    uint N = stuff[0];
    uint M = stuff[1];

    uint x = stuff[2];
    uint y = stuff[3];

    uint statesCount = constSizes[0];
    uint stateSize = constSizes[1];
    uint mapSize = constSizes[2];

    uint sizeInBytes = 4 + 2 * statesCount * stateSize;
	uint bufSize = sizeInBytes / 4;

    uint myPos = x*M + y;
    uint myBuf = myPos * bufSize;
    uint srand = varValues[0];
    uint random_value = nextInt( srand+(srand*x - srand*y)%(N*M) );

    //float cache[ max_size*max_size ];

    int* myMapsPtr = mapBuffer + myPos*mapSize;    
    

    uint* inputBuffer = inBuffer;
    uint* outputBuffer = outBuffer;

    uint gensNum = constSizes[3];

    for ( uint counter = 0; counter < gensNum; ++counter )
    {
        int result = -1;
        //try me
        uint hisPos = x*M + y;
        uint hisBuf = hisPos * bufSize;
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        //bestResult = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, bestResult, myMapsPtr, outputBuffer, ptr+myPos*200 );    

        // with left of him
        // myMapsPtr += mapSize;
        hisPos = x*M + (y + M - 1)%M;
        hisBuf = hisPos * bufSize;
        random_value = crossThem( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_value );
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        // with up of him
        hisPos = ((x + N - 1)%N)*M + y;
        hisBuf = hisPos * bufSize;
        random_value = crossThem( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_value );
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        //with right of him
        hisPos = x*M + (y + 1)%M;
        hisBuf = hisPos * bufSize;
        random_value = crossThem( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_value );
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        //with down of him
        hisPos = ((x + 1)%N)*M + y;
        hisBuf = hisPos * bufSize;
        random_value = crossThem( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_value );
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        cache[myPos] = result;
        //swap buffers:
        uint* tmp = inputBuffer;
        inputBuffer = outputBuffer;
        outputBuffer = tmp;
    }
}
  
