#include "AutomatShortTables.h"
#include "Tools\binFunc.hpp"
#include <boost\assert.hpp>

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::CAutomatShortTables(CAntCommon<COUNTERS_TYPE>* pAntCommon)
:pAntCommon(pAntCommon)
{
	startState = 0;
	buffer = new COUNTERS_TYPE[commonDataSize + pAntCommon->statesCount()*stateSize];
	memset(buffer, 0, sizeof(COUNTERS_TYPE)*(commonDataSize + pAntCommon->statesCount()*stateSize));
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::CAutomatShortTables()
:buffer(0), startState(0)
{
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::~CAutomatShortTables()
{
	delete[] buffer;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::CAutomatShortTables(const CAutomatShortTables& automat)
:pAntCommon(automat.pAntCommon), startState(automat.startState)
{
	size_t size = commonDataSize + pAntCommon->statesCount()*stateSize;
	buffer = new COUNTERS_TYPE[size];
	memcpy(buffer, automat.buffer, size*sizeof(COUNTERS_TYPE));
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>& CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::operator = (const CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>& automat)
{
	pAntCommon = automat.pAntCommon;
	startState = automat.startState;
	size_t size = commonDataSize + pAntCommon->statesCount()*stateSize;
	if (buffer == 0)
		buffer = new COUNTERS_TYPE[size];
	memcpy(buffer, automat.buffer, size*sizeof(COUNTERS_TYPE));
	return *this;
}

template<typename CT, typename IT, size_t S, size_t IPC>
CAutomatShortTablesPtr<CT, IT, S, IPC> CAutomatShortTables<CT, IT, S, IPC>::createFromBuffer(
		CAntCommon<CT>* pAntCommon, CT* buf)
{
		CAutomatShortTablesPtr<CT, IT, S, IPC> aut(
			new CAutomatShortTables<CT, IT, S, IPC>(pAntCommon));
	size_t size = commonDataSize + pAntCommon->statesCount()*stateSize;
	aut->startState = buf[0];
	memcpy(aut->buffer, buf, size*sizeof(CT));
	return aut;
}


template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::generateRandom(CRandom* rand)
{
	startState = 0;// pAntCommon->randomState(rand);
	buffer[0] = startState;
	for (size_t currentState = 0; currentState < pAntCommon->statesCount(); ++currentState)
	{
		COUNTERS_TYPE* maskPtr = buffer + commonDataSize + currentState*stateSize;
		randomMask(maskPtr, rand);
		COUNTERS_TYPE* tablePtr = maskPtr + maskSize;
		randomTable(tablePtr, rand, pAntCommon);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::fillRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, char* buff, CRandom* rand)
{
	COUNTERS_TYPE startState = 0;// pAntCommon->randomState(rand);
	buff[0] = startState;
	for (size_t currentState = 0; currentState < pAntCommon->statesCount(); ++currentState)
	{
		COUNTERS_TYPE* maskPtr = buff + commonDataSize + currentState*stateSize;
		randomMask(maskPtr, rand);
		COUNTERS_TYPE* tablePtr = maskPtr + maskSize;
		randomTable(tablePtr, rand, pAntCommon);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::randomMask(COUNTERS_TYPE* mask, CRandom* rand)
{
	size_t alreadyEnabled = 0;
	BOOST_ASSERT(maskSize == INPUT_PARAMS_COUNT);
	for (size_t i = 0; i < maskSize; ++i)
	{
		mask[i] = 0;
		double prob = (SHORT_TABLE_COLUMNS*2.0) / (3.0*maskSize);
		if ((rand->nextUINT() & 255) < 256 * prob)
		{
			if (alreadyEnabled < SHORT_TABLE_COLUMNS)
			{
				++alreadyEnabled;
				mask[i] = 1;
			}
		}
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::randomTable(COUNTERS_TYPE* table, CRandom* rand, CAntCommon<COUNTERS_TYPE>* antCommon)
{
	size_t maxIndex = Tools::twoPow(SHORT_TABLE_COLUMNS);
	BOOST_ASSERT(maxIndex*recordSize <= tableSize);
	for (size_t index = 0; index < maxIndex; ++index)
	{
		table[index*recordSize + stateShift] = antCommon->randomState(rand);
		table[index*recordSize + actionShift] = antCommon->randomAction(rand);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
size_t CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::countIndex(const std::vector<INPUT_TYPE>& mas, size_t currentState) const
{
	COUNTERS_TYPE* currentMask = buffer + commonDataSize + currentState * stateSize;
	int index = 0, k = 1;
	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (currentMask[i])
		{
			index += k*mas[i];
			k *= 2;
		}
	}
	return index;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
COUNTERS_TYPE CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::getStartState() const
{
	BOOST_ASSERT(buffer[0] == startState);
	return startState;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
COUNTERS_TYPE CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const
{
	size_t index = countIndex(input, currentState);
	COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * stateSize + maskSize;
	COUNTERS_TYPE state = *(currentTable + recordSize*index + stateShift);
	return state;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
COUNTERS_TYPE CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const
{
	size_t index = countIndex(input, currentState);
	COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * (stateSize)+maskSize;
	COUNTERS_TYPE action = *(currentTable + recordSize*index + actionShift);
	return action;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::mutate(CRandom* rand)
{
	for (size_t currentState = 0; currentState < pAntCommon->statesCount(); ++currentState)
	{
		uint a = rand->nextUINT() & 255;
		if (a < 16 || a > 64)
			continue;
		COUNTERS_TYPE* currentMask = buffer + commonDataSize + currentState * (stateSize);
		a = rand->nextUINT() & 255;
		if ((a > 31) && (a < 50))
			//small probaility to change mask
			mutateMask(currentMask, rand);
		COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * (stateSize) + maskSize;
		a = rand->nextUINT() & 255;
		if ((a > 31) && (a < 90))
			mutateTable(currentTable, rand, pAntCommon);
	}

	//if ((rand->nextUINT() & 255) < 30)
	//	startState = rand->nextUINT()%pAntCommon->statesCount();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::mutateTable(COUNTERS_TYPE* currentTable, CRandom* rand, 
	CAntCommon<COUNTERS_TYPE>* pAntCommon )
{
	for (size_t i = 0; i < tableSize; i += recordSize)
	{
		uint a = rand->nextUINT() & 255;
		if (a < 32)
			currentTable[i + stateShift] = pAntCommon->randomState(rand);
		a = rand->nextUINT() & 255;
		if ((a >= 100) && (a <= 140))
			currentTable[i + actionShift] = pAntCommon->randomAction(rand);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::mutateMask(COUNTERS_TYPE* currentMask, CRandom* rand)
{
	uint alreadyEnabledCnt = 0;
	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	if (currentMask[i])
		++alreadyEnabledCnt;
	uint toDel = rand->nextUINT()%INPUT_PARAMS_COUNT;
	uint toAdd = rand->nextUINT()%INPUT_PARAMS_COUNT;
	if (alreadyEnabledCnt == SHORT_TABLE_COLUMNS)
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

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::crossover(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* mother, 
	const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* father, CRandom* rand)
{
	const CAutomatShortTables* motherPtr = static_cast<const CAutomatShortTables*>(mother);
	const CAutomatShortTables* fatherPtr = static_cast<const CAutomatShortTables*>(father);
	pAntCommon = motherPtr->pAntCommon;
	size_t size = commonDataSize + pAntCommon->statesCount()*stateSize;
	if (buffer == 0)
	{
		buffer = new COUNTERS_TYPE[size];
		memset(buffer, 0, size*sizeof(COUNTERS_TYPE));
	}

	for (size_t currentState = 0; currentState < motherPtr->pAntCommon->statesCount(); ++currentState)
	{
		COUNTERS_TYPE* motherMask = motherPtr->buffer + commonDataSize + currentState * (stateSize);
		COUNTERS_TYPE* fatherMask = fatherPtr->buffer + commonDataSize + currentState * (stateSize);
		COUNTERS_TYPE* childMask = buffer + commonDataSize + currentState * (stateSize);

		crossMasks(childMask, motherMask, fatherMask, rand);
		crossTables(childMask, motherMask, fatherMask, rand);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::crossMasks(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand)
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
				size_t res = ((rand->nextUINT() & 255) < (255 / (SHORT_TABLE_COLUMNS - bothEnabled + 1))) ? 1 : 0;
				if (res && (enabled < SHORT_TABLE_COLUMNS))
				{
					++enabled;
					childMask[i] = 1;
				}
			}
		}
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::crossTables(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand)
{
	size_t myMas[SHORT_TABLE_COLUMNS], j = 0;
	memset(myMas, 0, SHORT_TABLE_COLUMNS*sizeof(size_t));
	
	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (childMask[i])
			myMas[j++] = i;
	}

	size_t toMother[SHORT_TABLE_COLUMNS], toFather[SHORT_TABLE_COLUMNS];
	toParent(toMother, myMas, motherMask);
	toParent(toFather, myMas, fatherMask);

	size_t maxIndex = Tools::twoPow(SHORT_TABLE_COLUMNS);
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

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
size_t CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::createParentIndex(const size_t* toParent, const size_t* myArray, size_t index, CRandom* rand)
{
	size_t maxIndex = Tools::twoPow(SHORT_TABLE_COLUMNS);
	size_t parentIndex = index;// rand->nextUINT()&(maxIndex - 1);
	for (size_t i = 0; i < SHORT_TABLE_COLUMNS; ++i)
	{
		size_t myBit = Tools::checkBit(index, myArray[i]);
		if (toParent[i])
		{
			if (myBit)
				Tools::setBit(parentIndex, toParent[i] - 1);
			else
				Tools::clearBit(parentIndex, toParent[i] - 1);
		}
	}
	return parentIndex;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
void CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::toParent(size_t* toMother, const size_t* myMas, const COUNTERS_TYPE* motherMask)
{
	for (size_t i = 0; i < SHORT_TABLE_COLUMNS; ++i)
		toMother[i] = 0;
	size_t j = 1, k = 0;
	for (size_t i = 0; i < INPUT_PARAMS_COUNT; ++i)
	{
		if (i > myMas[k] && k < SHORT_TABLE_COLUMNS)
			++k;
		if (motherMask[i])
		{
			if (myMas[k] == i)
				toMother[k] = j;
			++j;
		}
	}
}