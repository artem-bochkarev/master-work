#include "stdafx.h"
#include "CAutomatShortTables.h"

size_t CAutomatShortTables::countIndex(int* mas) const
{
	int* mask = buffer + currentState * stateSize;
	int index = 0, k = 1, j = 0;
	for (int i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (mask[i])
		{
			index += k*mas[j++];
			k *= 2;
		}
	}
	return index;
}

char CAutomatShortTables::getNextState(char currentState, int* input) const
{
	size_t index = countIndex(input);
	char * ptrStates = buffer + currentState * 2 * stateSize;
	char state = *(ptrStates + index);
	return state;
}

char CAutomatShortTables::getAction(char currentState, int* input) const
{
	size_t index = countIndex(input);
	char * ptrStates = buffer + currentState * 2 * stateSize;
	char * ptrActions = ptrStates + stateSize;
	char action = *(ptrActions + index);
	return action;
}