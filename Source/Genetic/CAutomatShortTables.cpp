#include "stdafx.h"
#include "CAutomatShortTables.h"
#include "Tools\binFunc.hpp"
#include <boost\assert.hpp>

CStateContainer<COUNTERS_TYPE>* CAutomatShortTables::states = nullptr;
CActionContainer<COUNTERS_TYPE>* CAutomatShortTables::actions = nullptr;

size_t CAutomatShortTables::commonDataSize = 4;
size_t CAutomatShortTables::statesCount = 5;
size_t CAutomatShortTables::maskSize = INPUT_PARAMS_COUNT;
size_t CAutomatShortTables::tableSize = 2*(1 << SHORT_TABLE_COLUMNS);
size_t CAutomatShortTables::stateSize = tableSize + maskSize;

CAutomatShortTables::CAutomatShortTables(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions)
{
	startState = 0;
	buffer = new COUNTERS_TYPE[commonDataSize + statesCount*stateSize];
	this->states = states;
	this->actions = actions;
}

CAutomatShortTables::~CAutomatShortTables()
{
	delete[] buffer;
}

CAutomatShortTables::CAutomatShortTables(const CAutomatShortTables& automat)
{
	startState = automat.startState;
	size_t size = commonDataSize + statesCount*stateSize;
	buffer = new COUNTERS_TYPE[size];
	memcpy(buffer, automat.buffer, size*sizeof(COUNTERS_TYPE));
}

CAutomatShortTables& CAutomatShortTables::operator = (const CAutomatShortTables& automat)
{
	startState = automat.startState;
	size_t size = commonDataSize + statesCount*stateSize;
	memcpy(buffer, automat.buffer, size*sizeof(COUNTERS_TYPE));
	return *this;
}

void CAutomatShortTables::generateRandom(CRandom* rand)
{
	startState = states->randomState(rand);
	buffer[0] = startState;
	for (size_t currentState = 0; currentState < statesCount; ++currentState)
	{
		COUNTERS_TYPE* maskPtr = buffer + commonDataSize + currentState*stateSize;
		randomMask(maskPtr, rand);
		COUNTERS_TYPE* tablePtr = maskPtr + maskSize;
		randomTable(tablePtr, rand);
	}
}

void CAutomatShortTables::randomMask(COUNTERS_TYPE* mask, CRandom* rand)
{
	size_t alreadyEnabled = 0;
	BOOST_ASSERT(maskSize == INPUT_PARAMS_COUNT);
	for (size_t i = 0; i < maskSize; ++i)
	{
		mask[i] = 0;
		double prob = (MAX_PARAMETERS*2.0) / (3.0*maskSize);
		if ((rand->nextUINT() & 255) < 256 * prob)
		{
			if (alreadyEnabled < MAX_PARAMETERS)
			{
				++alreadyEnabled;
				mask[i] = 1;
			}
		}
	}
}

void CAutomatShortTables::randomTable(COUNTERS_TYPE* table, CRandom* rand)
{
	size_t maxIndex = Tools::twoPow(MAX_PARAMETERS);
	BOOST_ASSERT(maxIndex*recordSize <= tableSize);
	for (size_t index = 0; index < maxIndex; ++index)
	{
		table[index*recordSize + stateShift] = states->randomState(rand);
		table[index*recordSize + actionShift] = actions->randomAction(rand);
	}
}

size_t CAutomatShortTables::countIndex(std::vector<INPUT_TYPE>* mas, size_t currentState ) const
{
	COUNTERS_TYPE* currentMask = buffer + commonDataSize + currentState * (tableSize + maskSize);
	int index = 0, k = 1, j = 0;
	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (currentMask[i])
		{
			index += k*mas->at(j++);
			k *= 2;
		}
	}
	return index;
}

COUNTERS_TYPE CAutomatShortTables::getStartState() const
{
	BOOST_ASSERT(buffer[0] == startState);
	return startState;
}

COUNTERS_TYPE CAutomatShortTables::getNextState(COUNTERS_TYPE currentState, std::vector<INPUT_TYPE>* input) const
{
	size_t index = countIndex(input, currentState);
	COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * (stateSize)+maskSize;
	COUNTERS_TYPE state = *(currentTable + recordSize*index + stateShift);
	return state;
}

COUNTERS_TYPE CAutomatShortTables::getAction(COUNTERS_TYPE currentState, std::vector<INPUT_TYPE>* input) const
{
	size_t index = countIndex(input, currentState);
	COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * (stateSize)+maskSize;
	COUNTERS_TYPE action = *(currentTable + recordSize*index + actionShift);
	return action;
}

void CAutomatShortTables::mutate(CRandom* rand)
{
	for (size_t currentState = 0; currentState < statesCount; ++currentState)
	{
		COUNTERS_TYPE* currentMask = buffer + commonDataSize + currentState * (stateSize);
		mutateMask(currentMask, rand);
		COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * (stateSize) + maskSize;
		mutateTable(currentTable, rand);
	}

	if ((rand->nextUINT() & 255) < 30)
		startState = rand->nextUINT()%statesCount;
}

void CAutomatShortTables::mutateTable(COUNTERS_TYPE* currentTable, CRandom* rand)
{
	for (size_t i = 0; i < tableSize; i += recordSize)
	{
		uint a = rand->nextUINT() & 255;
		if (a == 100)
			currentTable[i + stateShift] = states->randomState(rand);
		if ((a >= 100) && (a <= 104))
			currentTable[i + actionShift] = actions->randomAction(rand);
	}
}

void CAutomatShortTables::mutateMask(COUNTERS_TYPE* currentMask, CRandom* rand)
{
	uint a = rand->nextUINT() & 255;
	if ((a > 31) && (a < 38))
		//small probaility to change mask
	{
		uint alreadyEnabledCnt = 0;
		for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
		if (currentMask[i])
			++alreadyEnabledCnt;
		uint toDel = rand->nextUINT()%INPUT_PARAMS_COUNT;
		uint toAdd = rand->nextUINT()%INPUT_PARAMS_COUNT;
		if (alreadyEnabledCnt == MAX_PARAMETERS)
		{
			if (currentMask[toDel])
			{
				currentMask[toDel] = 0;
				currentMask[toAdd] = 1;
			}
		}
		else
		{
			currentMask[toDel] = 0;
			currentMask[toAdd] = 1;
		}
	}
}

void CAutomatShortTables::crossover(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* mother, const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* father, CRandom* rand)
{
	const CAutomatShortTables* motherPtr = static_cast<const CAutomatShortTables*>(mother);
	const CAutomatShortTables* fatherPtr = static_cast<const CAutomatShortTables*>(father);

	for (size_t currentState = 0; currentState < statesCount; ++currentState)
	{
		COUNTERS_TYPE* motherMask = motherPtr->buffer + commonDataSize + currentState * (stateSize);
		COUNTERS_TYPE* fatherMask = fatherPtr->buffer + commonDataSize + currentState * (stateSize);
		COUNTERS_TYPE* childMask = buffer + commonDataSize + currentState * (stateSize);

		crossMasks(childMask, motherMask, fatherMask, rand);
		crossTables(childMask, motherMask, fatherMask, rand);
	}
}

void CAutomatShortTables::crossMasks(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand)
{
	size_t bothEnabled = 0, oneEnabled = 0, enabled = 0;
	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (motherMask[i] && fatherMask[i])
		{
			childMask[i] = 1;
			bothEnabled++;
			enabled++;
		}
		else if (motherMask[i] || fatherMask[i])
			oneEnabled++;
	}

	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (childMask[i] == 0)
		{
			if (motherMask[i] || fatherMask[i])
			{
				size_t res = ((rand->nextUINT() & 255) < (255 / (MAX_PARAMETERS - bothEnabled + 1))) ? 1 : 0;
				if (res && (enabled < MAX_PARAMETERS))
				{
					++enabled;
					childMask[i] = 1;
				}
			}
		}
	}
}

void CAutomatShortTables::crossTables(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand)
{
	size_t myMas[MAX_PARAMETERS], j = 0;
	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (childMask[i])
			myMas[j++] = i;
	}

	size_t toMother[MAX_PARAMETERS], toFather[MAX_PARAMETERS];
	toParent(toMother, myMas, motherMask);
	toParent(toFather, myMas, fatherMask);

	size_t maxIndex = Tools::twoPow(MAX_PARAMETERS);
	for (size_t index = 0; index < maxIndex; ++index)
	{
		size_t motherIndex = createParentIndex(toMother, myMas, index, rand);
		size_t fatherIndex = createParentIndex(toFather, myMas, index, rand);
		COUNTERS_TYPE * childTable = childMask + maskSize;
		const COUNTERS_TYPE * motherTable = motherMask + maskSize;
		const COUNTERS_TYPE * fatherTable = fatherMask + maskSize;
		if ((rand->nextUINT() & 255) > 127)
			memcpy(childTable + index*recordSize, motherTable + motherIndex*recordSize, recordSize*sizeof(COUNTERS_TYPE));
		else
			memcpy(childTable + index*recordSize, fatherTable + fatherIndex*recordSize, recordSize*sizeof(COUNTERS_TYPE));
	}
}

size_t CAutomatShortTables::createParentIndex(const size_t* toParent, const size_t* myArray, size_t index, CRandom* rand)
{
	size_t maxIndex = Tools::twoPow(MAX_PARAMETERS);
	size_t parentIndex = rand->nextUINT()&(maxIndex - 1);
	for (size_t i = 0; i < MAX_PARAMETERS; ++i)
	{
		//change toGetBit
		size_t myBit = Tools::checkBit(index, myArray[i]);
		if (toParent[i])
		{
			//change to SteBit
			if (myBit)
				Tools::setBit(parentIndex, toParent[i] - 1);
			else
				Tools::clearBit(parentIndex, toParent[i] - 1);
		}
	}
	return parentIndex;
}

void CAutomatShortTables::toParent(size_t* toMother, const size_t* myMas, const COUNTERS_TYPE* motherMask)
{
	for (size_t i = 0; i < MAX_PARAMETERS; ++i)
		toMother[i] = 0;
	size_t j = 1, k = 0;
	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (i > myMas[k] && k < MAX_PARAMETERS)
			++k;
		if (motherMask[i])
		{
			if (myMas[k] == i)
				toMother[k] = j;
			++j;
		}
	}
}