#include "CleverAnt3Map.h"

CCleverAnt3Map::CCleverAnt3Map(size_t width, size_t geight, size_t foodCounter)
:CMapImpl(width, geight, foodCounter)
{}

int CCleverAnt3Map::getVisibleCells(int x, int y, EDirection direct, int* buffer ) const
{
	int x1(x), y1(y);
	forwardOf(x1, y1, direct);
	pushToBuffer(x1, y1, buffer);

	int x2(x1), y2(y1);
	leftOf(x2, y2, direct);
	pushToBuffer(x2, y2, buffer);

	int x3(x1), y3(y1);
	rightOf(x3, y3, direct);
	pushToBuffer(x3, y3, buffer);

	int x4(x1), y4(y1);
	forwardOf(x4, y4, direct);
	pushToBuffer(x4, y4, buffer);

	int x5(x), y5(y);
	leftOf(x5, y5, direct);
	pushToBuffer(x5, y5, buffer);

	int x6(x), y6(y);
	rightOf(x6, y6, direct);
	pushToBuffer(x6, y6, buffer);

	int x7(x5), y7(y5);
	leftOf(x7, y7, direct);
	pushToBuffer(x7, y7, buffer);

	int x8(x6), y8(y6);
	rightOf(x8, y8, direct);
	pushToBuffer(x8, y8, buffer);

	return 8;
}

std::vector< std::pair<int, int> > CCleverAnt3Map::getVisibleCells(int x, int y, EDirection direct) const
{
	/*
	    4
	  2 1 3
	7 5 _ 6 8
	*/
	assert(x >= 0);
	assert(y >= 0);
	std::vector< std::pair<int, int> > vec;
	int x1(x), y1(y);
	forwardOf(x1, y1, direct);
	vec.push_back(std::make_pair(x1, y1));

	int x2 = x1, y2 = y1;
	leftOf(x2, y2, direct);
	vec.push_back(std::make_pair(x2, y2));

	int x3 = x1, y3 = y1;
	rightOf(x3, y3, direct);
	vec.push_back(std::make_pair(x3, y3));

	int x4 = x1, y4 = y1;
	forwardOf(x4, y4, direct);
	vec.push_back(std::make_pair(x4, y4));

	int x5(x), y5(y);
	leftOf( x5, y5, direct );
	vec.push_back( std::make_pair( x5, y5 ) );

	int x6(x), y6(y);
	rightOf( x6, y6, direct );
	vec.push_back( std::make_pair( x6, y6 ) );

	int x7(x5), y7(y5);
	leftOf( x7, y7, direct );
	vec.push_back( std::make_pair( x7, y7 ) );

	int x8(x6), y8(y6);
	rightOf( x8, y8, direct );
	vec.push_back( std::make_pair( x8, y8 ) );

	return vec;
}

std::vector<int> CCleverAnt3Map::getInput(size_t x, size_t y, EDirection direct) const
{

	/*std::vector< std::pair<int, int> > vec( getVisibleCells( x, y, direct ) );
	std::vector<int> res;
	for ( size_t i=0; i<vec.size(); ++i )
	{
	const std::pair<int, int>& pair( vec[i] );
	res.push_back( map[pair.second][pair.first] );
	}
	return res;*/

	int buffer[2 * 8];
	int size = getVisibleCells(x, y, direct, buffer);
	std::vector<int> res;
	for (int i = 0; i<size; ++i)
	{
		res.push_back(map[buffer[2 * i + 1]][buffer[2 * i]]);
	}
	return res;
}

void CCleverAnt3Map::getInput(size_t x, size_t y, EDirection direct, int* output) const
{
	int buffer[2 * 8];
	int size = getVisibleCells(x, y, direct, buffer);
	for (int i = 0; i<size; ++i)
	{
		output[i] = map[buffer[2 * i + 1]][buffer[2 * i]];
	}
}