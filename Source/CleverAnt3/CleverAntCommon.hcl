#define actions_count 3

#define c_x_size 32
#define c_y_size 32
#define m_x_size 31
#define m_y_size 31
#define MAP_HEADER_SIZE 4

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

/*uint nextUInt( uint x )
{
    return ( rand_a*x + rand_c );
}

char16 nextChar16( char16 x )
{
    char16 a = (char16)(17);
    char16 b = (char16)(31);
    return mad_hi( x, a, b);
}*/

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
    const __global int * myMap = map + MAP_HEADER_SIZE;
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

uint getFoodNumber(const __global int* map)
{
	return map[2];
}

uint8 getInputCA3( uint x, uint y, uint direction, const __global int* map )
{
    const __global int * myMap = map + MAP_HEADER_SIZE;
    int x1 = x, y1 = y;
    x1 = moveX( x, direction );
    y1 = moveY( y, direction );
	
		uint8 out;
		out.s0 = myMap[x1 + c_x_size*y1];
		out.s1 = myMap[ moveX( x1, left( direction ) ) + c_x_size*moveY( y1, left( direction ) ) ];
		out.s2 = myMap[ moveX( x1, right( direction ) ) + c_x_size*moveY( y1, right( direction ) ) ];
		out.s3 = myMap[ moveX( x1, direction ) + c_x_size*moveY( y1, direction ) ];
		
		x1 = moveX(x, left(direction)); y1 = moveY(y, left(direction));
		out.s4 = myMap[x1 + c_x_size*y1];
		int x2 = moveX(x, right(direction)); int y2 = moveY(y, right(direction));
		out.s5 = myMap[x2 + c_x_size*y2];
		
		x1 = moveX(x1, left(direction)); y1 = moveY(y1, left(direction));
		out.s6 = myMap[x1 + c_x_size*y1];
		x2 = moveX(x2, right(direction)); y2 = moveY(y2, right(direction));
		out.s7 = myMap[x2 + c_x_size*y2];
	return out;
	//#endif
}