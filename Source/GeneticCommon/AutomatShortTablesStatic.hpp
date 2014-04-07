#pragma once
#include "AutomatShortTablesStatic.h"
#include "Tools\binFunc.hpp"
#include <boost\assert.hpp>

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::CAutomatShortTablesStatic(CAntCommon<COUNTERS_TYPE>* pAntCommon)
:pAntCommon(pAntCommon)
{
	startState = 0;
	memset(buffer, 0, sizeof(COUNTERS_TYPE)*(bufferSize));
	buffer[0] = 666;
	buffer[1] = 666;
	buffer[2] = 666;
	buffer[3] = 666;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::CAutomatShortTablesStatic()
:startState(0)
{
	memset(buffer, 0, sizeof(COUNTERS_TYPE)*(bufferSize));
	buffer[0] = 666;
	buffer[1] = 666;
	buffer[2] = 666;
	buffer[3] = 666;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::~CAutomatShortTablesStatic()
{
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::CAutomatShortTablesStatic(const CAutomatShortTablesStatic& automat)
:pAntCommon(automat.pAntCommon), startState(automat.startState)
{
	memcpy(buffer, automat.buffer, bufferSize*sizeof(COUNTERS_TYPE));
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
size_t CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::getBufferOffset() const
{
	int* a = buffer;
	int* b = (int*)this;
	int c = a - b;
	void* a1 = (void*)buffer;
	void* b1 = (void*)this;
	//int c1 = a1 - b1;
	return c;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
const CAntCommon<COUNTERS_TYPE>* CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::getAntCommon() const
{
	return pAntCommon;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>& 
CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::operator = 
	(const CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>& automat)
{
	pAntCommon = automat.pAntCommon;
	startState = automat.startState;
	memcpy(buffer, automat.buffer, bufferSize*sizeof(COUNTERS_TYPE));
	return *this;
}

template<typename CT, typename IT, size_t S, size_t IPC, size_t SC>
CAutomatShortTablesStaticPtr<CT, IT, S, IPC, SC> CAutomatShortTablesStatic<CT, IT, S, IPC, SC>::createFromBuffer(
	CAntCommon<CT>* pAntCommon, CT* buf)
{
	CAutomatShortTablesStaticPtr<CT, IT, S, IPC, SC> aut(
		new CAutomatShortTablesStatic<CT, IT, S, IPC, SC>(pAntCommon));
	aut->startState = buf[0];
	memcpy(aut->buffer, buf, bufferSize*sizeof(CT));
	return aut;
}


template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::generateRandom(CRandom* rand)
{
	startState = 0;// pAntCommon->randomState(rand);
	buffer[0] = startState;
	buffer[1] = 666;
	buffer[2] = 666;
	buffer[3] = 666;
	for (size_t currentState = 0; currentState < pAntCommon->statesCount(); ++currentState)
	{
		COUNTERS_TYPE* maskPtr = buffer + commonDataSize + currentState*stateSize;
		CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::randomMask(maskPtr, rand);
		COUNTERS_TYPE* tablePtr = maskPtr + maskSize;
		CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::randomTable(tablePtr, rand, pAntCommon);
	}
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::fillRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, char* buff, CRandom* rand)
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

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
size_t CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::countIndex(const std::vector<INPUT_TYPE>& mas, size_t currentState) const
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

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
COUNTERS_TYPE CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::getStartState() const
{
	BOOST_ASSERT(buffer[0] == startState);
	return startState;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
COUNTERS_TYPE CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const
{
	size_t index = countIndex(input, currentState);
	COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * stateSize + maskSize;
	COUNTERS_TYPE state = *(currentTable + recordSize*index + stateShift);
	return state;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
COUNTERS_TYPE CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const
{
	size_t index = countIndex(input, currentState);
	COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * (stateSize)+maskSize;
	COUNTERS_TYPE action = *(currentTable + recordSize*index + actionShift);

	COUNTERS_TYPE* currentMask = buffer + commonDataSize + currentState * (stateSize);
	/*printf("\tINDEX=%i    MASK=%i, %i, %i, %i, %i, %i, %i, %i\n", index, currentMask[0],
		currentMask[1], currentMask[2], currentMask[3], currentMask[4],
		currentMask[5], currentMask[6], currentMask[7]);*/

	return action;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::mutate(CRandom* rand)
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
			CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::mutateMask(currentMask, rand);
		COUNTERS_TYPE * currentTable = buffer + commonDataSize + currentState * (stateSize)+maskSize;
		a = rand->nextUINT() & 255;
		if ((a > 31) && (a < 90))
			CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::mutateTable(currentTable, rand, pAntCommon);
	}

	//if ((rand->nextUINT() & 255) < 30)
	//	startState = rand->nextUINT()%pAntCommon->statesCount();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
void CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>::crossover(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* mother,
	const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* father, CRandom* rand)
{
	const CAutomatShortTablesStatic* motherPtr = static_cast<const CAutomatShortTablesStatic*>(mother);
	const CAutomatShortTablesStatic* fatherPtr = static_cast<const CAutomatShortTablesStatic*>(father);
	pAntCommon = motherPtr->pAntCommon;
	size_t size = commonDataSize + pAntCommon->statesCount()*stateSize;
	
	memset(buffer, 0, size*sizeof(COUNTERS_TYPE));

	for (size_t currentState = 0; currentState < motherPtr->pAntCommon->statesCount(); ++currentState)
	{
		COUNTERS_TYPE* motherMask = motherPtr->buffer + commonDataSize + currentState * (stateSize);
		COUNTERS_TYPE* fatherMask = fatherPtr->buffer + commonDataSize + currentState * (stateSize);
		COUNTERS_TYPE* childMask = buffer + commonDataSize + currentState * (stateSize);

		CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::crossMasks(childMask, motherMask, fatherMask, rand);
		CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::crossTables(childMask, motherMask, fatherMask, rand);
	}
}