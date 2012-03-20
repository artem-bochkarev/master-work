#pragma OPENCL EXTENSION cl_khr_byte_addressable_store : enable

// rand like in MSVC
__constant uint rand_m = 0x80000000;
__constant uint rand_a = 214013;
__constant uint rand_c = 2531011;

//probablility in % (0..100)
__constant uint mutation_probability = 0;
#define actions_count 3;
__constant uint max_size = 20;
/*__constant uint nextDir[12] = { 
                           0, 1, 2, 3,  //step forward
                           3, 0, 1, 2,  //turn left
                           1, 2, 3, 0,  //turn right
                          };
                          */
__constant int nextValX[12] = { 
                           -1, 0, 1, 0,  //step forward
                           0, 0, 0, 0,  //turn left
                           0, 0, 0, 0,  //turn right
                          };
__constant int nextValY[12] = { 
                           0, -1, 0, 1,  //step forward
                           0, 0, 0, 0,  //turn left
                           0, 0, 0, 0,  //turn right
                          };

#define c_x_size 32
#define c_y_size 32
#define m_x_size 31
#define m_y_size 31

uint nextInt( uint x )
{
    return ( rand_a*x + rand_c );// % rand_m;
}

char16 nextUChar16( char16 x )
{
    char16 a = (char16)(17);
    char16 b = (char16)(31);
    return mad_hi( x, a, b);//( x*33 + 17 );
    //return (x*33 + 17);//( x*33 + 17 );
}

uint left( uint direction )
{
    return ( direction - 1 ) & 3;
    //return ( 4 + direction - 1 ) % 4;
}
uint right( uint direction )
{
    return ( 1 + direction ) & 3;
    //return ( 1 + direction ) % 4;
}

uint actionTurn( uint direction, uint action )
{
    int res = select( 0, -1, action == 2 );
    res = select( res, 1, action == 1 );
    return ( direction + res )&3;
    //return nextDir[ direction + action*4 ];
    //return ( 4 + direction + res ) % 4;
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
uint moveX( uint x, uint direction )//, uint x_size )
{
    /*switch ( direction )
    {
    case 0:
        return (x_size + x - 1)%x_size;
    case 1:
        return x;
    case 2:
        return (x + 1)%x_size;
    case 3:
        return x;
    }
    return x;*/
    //int a = (direction + 1) % 2;
    //int b = a*( (int)direction - 1 );
    int b = select( (int)direction - 1, 0, (direction & 1) != 0 );
	//return ( x_size + x + b )%x_size;
    return ( x + b ) & m_x_size;
}

uint moveY( uint y, uint direction )//, uint y_size )
{
    /*switch ( direction )
    {
    case 0:
        return y;
    case 1:
        return (y_size + y - 1)%y_size;
    case 2:
        return y;
    case 3:
        return (y + 1)%y_size;
    }
    return y;*/
    //int a = direction % 2;
    //int b = a*( (int)direction - 2 );
    int b = select( (int)direction - 2, 0, (direction & 1) == 0 );
	//return ( y_size + y + b )%y_size;
    return ( y + b ) & m_y_size;
}

uint moveXa( uint x, uint direction, uint action )
{
    int a = (direction ) & 1;
    //int b = a*( (int)direction - 1 )*(1 - step( 1, action ));
    //uint c = ( a == 0 )||( action != 0 );
    int c = ( (a) | action );
    int b = select( (int)direction - 1, 0, c );
	return ( x + b ) & m_x_size;
    //return ( x + nextValX[ direction + action*direction] ) & m_x_size;
    //return ( x_size + x + b )%x_size;
}

uint moveYa( uint y, uint direction, uint action )
{
    int a = (direction + 1) & 1;
    //int b = a*( (int)direction - 2 )*( 1 - step( 1, action ));
    //uint c = ( a == 0 )||( action != 0 );
    int c = ( (a) | action );
    int b = select( (int)direction - 2, 0, c );
	return ( y + b ) & m_y_size;
    //return ( y + nextValY[ direction + action*direction] ) & m_y_size;
    //return ( y_size + y + b )%y_size;
}

uint4 getInput( uint x, uint y, uint direction, const __global char* map )
{
    //int x_size = 32;//(0x00000020);// = (uint)c_x_size;//(uint)map[0];
    //uint x_size = (uint)map[0];
    //uint y_size = c_y_size;//(uint)map[1];

    const __global char * myMap = map + 2;
    int x1 = x, y1 = y;
    x1 = moveX( x, direction );//, x_size );
    y1 = moveY( y, direction );//, y_size );
    uint4 out;
    out.x = myMap[ x1 + c_x_size*y1 ];
    out.y = myMap[ moveX( x1, left( direction ) ) + c_x_size*moveY( y1, left( direction ) ) ];
    out.z = myMap[ moveX( x1, right( direction ) ) + c_x_size*moveY( y1, right( direction ) ) ];
    out.w = myMap[ moveX( x1, direction ) + c_x_size*moveY( y1, direction ) ];
    return out;
}

uint countIndex( uint4 in, uint stateSize )
{
    //uint res = 0;
    //for ( int i=0; i<4; ++i )
    //{
        //if ( in[i] != 0 )
      //      res += ( in[i] << i );
        //res <<= 1;
    //}
    uint res = mad_sat( (uint)2, in.y, in.x );
    res = mad_sat( (uint)4, in.z, res );
    res = mad_sat( (uint)8, in.w, res );
    return res;//%stateSize;
}
char getNextState( uint index, char curState, const __global char* ind, uint stateSize )
{
    uint diff = 2*stateSize*(uint)curState;
    return *(ind + diff + index);
}

char getNextAction( uint index, char curState, const __global char* ind, uint stateSize )
{
    uint diff = 2*stateSize*(uint)curState;
    return *(ind + diff + index + stateSize);
}

char2 getNext( uint index, char curState, const __global char* ind )
{
    char2 next;
    uint diff = 32*(uint)curState;
    next.x = *(ind + diff + index);
    next.y = *(ind + diff + index + 16);
    return next;
}

float run( __global char* ind, const uint statesCount, const uint stateSize, __global char* map )
{
    //uint x_size = (uint)map[0];
    //uint y_size = (uint)map[1];
    __global char * myMap = map + 2;
    uint x = 0, y = 0;
    uint direction = 2;
    char curState = *ind;
    ind++;
    float cnt = 0.0f;

    for ( uint i=0; i<100; ++i )
    {
        uint4 input = getInput( x, y, direction, map );
        uint index = countIndex( input, stateSize );
        //char action = getNextAction( index, curState, ind, stateSize );
        
        //curState = getNextState( index, curState, ind, stateSize );
        char2 nex = getNext( index, curState, ind );
        char action = nex.y;
        curState = nex.x;
        /*switch (action)
        {
            case 0:
                {
                    x = moveX( x, direction, x_size );
                    y = moveY( y, direction, y_size );
                    if ( myMap[x + x_size*y] != 0 )
                    {
                        ++cnt;
                        myMap[x + x_size*y] = 0;
                    }
                    //cnt += myMap[x + x_size*y];
                    //myMap[x + x_size*y] = 0;
            }break;
            case 1:
            {
                direction = right( direction );
            }break;
            case 2:
            {
                direction = left( direction );
            }break;
        }*/
        /*if ( action == 0 )
        {
            x = moveX( x, direction, x_size );
            y = moveY( y, direction, y_size );
            if ( myMap[x + x_size*y] != 0 )
            {
                ++cnt;
                myMap[x + x_size*y] = 0;
            }
        }else
            direction = actionTurn( direction, action );*/
        /*int mas[6] = { 0, -1, 0, 1, 0, 0 };
        uint res = direction + action*4;
        res = select( res, (uint)4, (uint)(res > 4) );
        x = ( x + mas[res+1] )& m_x_size;
        y = ( y + mas[res] )& m_y_size;*/
        x = moveXa( x, direction, action );
        y = moveYa( y, direction, action );
        cnt += myMap[x + c_x_size*y];
        myMap[x + c_x_size*y] = 0;
        direction = actionTurn( direction, action );
    }
    return cnt;
}

uint mutateHim( uint statesCount, uint stateSize, 
               __global char* automatPtr, uint random_value )
{
    random_value = nextInt( random_value );

    if ( random_value%100 < mutation_probability )
    {
        random_value = nextInt( random_value );
        uint pos = random_value%statesCount;
        __global char* ptrStates = automatPtr + 1 + 2*stateSize*pos;
        __global char* ptrActions = ptrStates + stateSize;
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
}

uint crossThem( uint bufSize, uint myBuf, __global char* tempBuffer, 
              uint hisBuf, __global char* inBuffer, uint random_value )
{
    for ( uint i=0; i < bufSize; ++i )
    {
        random_value = nextInt( random_value );
        /*uint x = step( 50, random_value % 100 );
        uint res = ( 1 - x )*myBuf + x*hisBuf;*/
        //uint res = select( myBuf, hisBuf, (random_value % 100) < 50 ); 
        uint res = select( myBuf, hisBuf, (random_value & 512) );
        tempBuffer[ myBuf + i ] = inBuffer[ res + i ];
        /*if ( ( random_value % 100) >= 50 )
            tempBuffer[ myBuf + i ] = inBuffer[ myBuf + i ];
        else
            tempBuffer[ myBuf + i ] = inBuffer[ hisBuf + i ];*/
    }
    return random_value;
}

uchar16 newCross( uint bufSize, uint myBuf, __global char* tempBuffer, 
              uint hisBuf, __global char* inBuffer, uchar16 random_vec )
{
    uint s = bufSize / 16;
    uchar16 a = (uchar16)(65);
    uchar16 b = (uchar16)(17);
    for ( uint i=0; i < s; ++i )
    {
        //random_vec = nextUChar16( random_vec );
        //random_vec = nextUChar16( random_vec );
        //random_vec = mad_hi( random_vec, a, b);
        random_vec *= a;
        random_vec += b;
        char16 mother = vload16( i, inBuffer + myBuf );
        char16 father = vload16( i, inBuffer + hisBuf );
      
        char16 child = select( mother, father, random_vec );
        vstore16( child, i, tempBuffer + myBuf );
    }
    return random_vec;
}

void memcpy_from_local( __global char* to, const __local char* from, uint count )
{
    char16 tmp16;
    uint cnt = count / 16;
    for ( uint i=0; i < cnt; ++i )
    {
        tmp16 = vload16( i, from );
        vstore16( tmp16, i, to );
    }
    count -= cnt*16;
    __global char* ptrTo = to + cnt*16;
    __local char* ptrFrom = from + cnt*16;
    for ( uint i=0; i<count; ++i )
        ptrTo[i] = ptrFrom[i];
}

void memcpy_from_global( __global char* to, const __global char* from, uint count )
{
    char16 tmp16;
    uint cnt = count / 16;
    for ( uint i=0; i < cnt; ++i )
    {
        tmp16 = vload16( i, from );
        vstore16( tmp16, i, to );
    }
    count -= cnt*16;
    __global char* ptrTo = to + cnt*16;
    __global char* ptrFrom = from + cnt*16;
    for ( uint i=0; i<count; ++i )
        ptrTo[i] = ptrFrom[i];
}

float tryHim( uint bufSize, uint myBuf, uint statesCount, uint stateSize, __global char* tempBuffer, 
             float bestResult, __global char* myMaps, __global char* outBuffer, __constant char* maps )
{
    //make copy of a map
    uint s = 32 * 2;
    char16 tmp;
    __constant char* from = maps+2;
    __global char* to = myMaps+2;
    for ( uint i=0; i < s; ++i )
    {
        tmp = vload16( i, from );
        vstore16( tmp, i, to );
    }

    float result = run( tempBuffer + myBuf, statesCount, stateSize, myMaps );
    if ( result > bestResult )
    {
        bestResult = result;
        memcpy_from_global( outBuffer + myBuf, tempBuffer + myBuf, bufSize );
    }
    return bestResult;
}
//constSizes[0] - statesCount
//constSizes[1] - stateSize
//constSizes[2] - mapSize
//constSizes[3] - gensNum

//varValues[0] - srand
//varValues[1] - last buf
__kernel void genetic_2d( __global char* inBuffer, __global char* outBuffer, 
                         __global const uint* constSizes, __global uint* varValues,
                         __global char* mapBuffer, __global char* tempBuffer, 
                         __constant char* maps, __global float* bestResult,
                         __global float* sumResult, __global char* bestIndivid )
{
    uint N = get_global_size(0);
    uint M = get_global_size(1);

    uint x = get_global_id(0);
    uint y = get_global_id(1);

    uint statesCount = constSizes[0];
    uint stateSize = constSizes[1];
    uint mapSize = constSizes[2];

    uint bufSize = 1 + 2 * statesCount * stateSize;

    uint myPos = x*M + y;
    uint myBuf = myPos * bufSize;
    uint srand = varValues[0];
    char16 tmp_vec = vload16( srand & 127, inBuffer );
    uchar16 random_vec = convert_uchar16( tmp_vec );
    //random_vec = nextUChar16( random_vec );
    uint random_value = nextInt( srand+(srand*N - srand*M)%(N*M) );

    __local float cache[ max_size*max_size ];
    __local uint bestResults[ max_size ];
    __local float sumResults[ max_size ];

    __global char* myMapsPtr = mapBuffer + myPos*mapSize;    
    

    //todo: in/out due to the varValues[1]
    __global char* inputBuffer = inBuffer;// = (1-koeff)*inBuffer + koeff*outBuffer;
    __global char* outputBuffer = outBuffer;// = (1-koeff)*outBuffer + koeff*inBuffer;
    
    uint koeff = varValues[1];
    if ( koeff%2 != 0 )
    {
        inputBuffer = outBuffer;
        outputBuffer = inBuffer;
    }

    uint gensNum = constSizes[3];
    //for ( uint i=0; i<bufSize; ++i )
      //  tempBuffer[myBuf + i] = inputBuffer[myBuf + i];

    for ( uint counter = 0; counter < gensNum; ++counter )
    {
        float result = -1.0f;
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
        //random_vec = newCross( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_vec );
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        // with up of him
        hisPos = ((x + N - 1)%N)*M + y;
        hisBuf = hisPos * bufSize;
        random_value = crossThem( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_value );
        //random_vec = newCross( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_vec );
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        //with right of him
        hisPos = x*M + (y + 1)%M;
        hisBuf = hisPos * bufSize;
        random_value = crossThem( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_value );
        //random_vec = newCross( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_vec );
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    

        //with down of him
        hisPos = ((x + 1)%N)*M + y;
        hisBuf = hisPos * bufSize;
        random_value = crossThem( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_value );
        //random_vec = newCross( bufSize, myBuf, tempBuffer, hisBuf, inputBuffer, random_vec );
        //random_value = mutateHim( statesCount, stateSize, tempBuffer + myBuf, random_value );
        result = tryHim( bufSize, myBuf, statesCount, stateSize, tempBuffer, result, myMapsPtr, outputBuffer, maps );    


        cache[myPos] = result;
        barrier( CLK_LOCAL_MEM_FENCE );

        if ( y == M - 1 )
        {
            uint max = 0;
            float sum = 0;
            for ( uint i=0; i<M; ++i )
            {
                sum += cache[ x*M + i ];
                if ( cache[ x*M + i ] > cache[ x*M + max ] )
                    max = i;
            }
            bestResults[x] = max;
            sumResults[x] = sum;
        }

        barrier( CLK_LOCAL_MEM_FENCE );
        if (( y == M - 1 ) && (x == N - 1) )
        {
            uint max = 0;
            float sum = 0;
            for ( uint i=0; i<N; ++i )
            {
                sum += sumResults[i];
                if ( cache[ i*M + bestResults[i] ] > cache[ max*M + bestResults[max] ] )
                    max = i;
            }
            bestResult[counter] = cache[ max*M + bestResults[max] ];
            sumResult[counter] = sum / (N*M);
            hisPos = max*M + bestResults[max];
            hisBuf = hisPos * bufSize;
            for ( uint i=0; i < bufSize; ++i )
                bestIndivid[ counter*bufSize + i ] = outputBuffer[ hisBuf + i ];

            //set new values for next turn
            //srand:
            varValues[0] = nextInt( random_value );
            varValues[1] = ( varValues[1] + 1 )%2;
        }
        //swap buffers:
        __global char* tmp = inputBuffer;
        inputBuffer = outputBuffer;
        outputBuffer = tmp;
        
        barrier( CLK_GLOBAL_MEM_FENCE );
    }
}
  