#pragma once
#include "AutomatImpl.h"
#include <cmath>
#include <boost/assert.hpp>

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::CAutomatImpl( CAntCommon<COUNTERS_TYPE>* pAntCommon )
//:CAutomat( states, actions )
:startState(0), pAntCommon(pAntCommon)
{
	stateSize = 1 << pAntCommon->statesCount();
	buffer = (COUNTERS_TYPE*)malloc(2 * pAntCommon->statesCount()*stateSize);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::CAutomatImpl(const CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE> &automat)
:startState(automat.startState), stateSize(automat.stateSize), pAntCommon(automat.pAntCommon)
{
	buffer = (COUNTERS_TYPE*)malloc(2 * pAntCommon->statesCount() * stateSize);
	memcpy(buffer, automat.buffer, 2 * pAntCommon->statesCount() * stateSize);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>& CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::operator =(const CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE> &automat)
{
    stateSize = automat.stateSize;
	pAntCommon = automat.pAntCommon;
	buffer = (COUNTERS_TYPE*)malloc(2 * pAntCommon->statesCount() * stateSize);
	memcpy(buffer, automat.buffer, 2 * pAntCommon->statesCount() * stateSize);
    startState = automat.startState;
    return *this;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
void CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::generateRandom(CRandom* rand)
{
	for (size_t i = 0; i < pAntCommon->statesCount(); ++i)
    {
		COUNTERS_TYPE * ptrStates = buffer + i * 2 * stateSize;
		COUNTERS_TYPE * ptrActions = ptrStates + stateSize;
        for ( size_t j=0; j<stateSize; ++j )
        {
			*(ptrStates + j) = pAntCommon->randomState(rand);
			*(ptrActions + j) = pAntCommon->randomAction(rand);
        }
    }
	startState = rand->nextUINT()%pAntCommon->statesCount(); //rand;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
void CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::fillRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon,
	COUNTERS_TYPE* buffer, CRandom* rand)
{
	size_t statesCount = pAntCommon->statesCount();
    size_t stateSize = 1 << statesCount;
	*buffer = (COUNTERS_TYPE)(rand->nextUINT() % statesCount);
    ++buffer;
	*buffer = (COUNTERS_TYPE)(rand->nextUINT() % statesCount);
    ++buffer;
	*buffer = (COUNTERS_TYPE)(rand->nextUINT() % statesCount);
    ++buffer;
	*buffer = (COUNTERS_TYPE)(rand->nextUINT() % statesCount);
    ++buffer;
    for ( size_t i=0; i<statesCount; ++i)
    {
		COUNTERS_TYPE * ptrStates = buffer + i * 2 * stateSize;
		COUNTERS_TYPE * ptrActions = ptrStates + stateSize;
        for ( size_t j=0; j<stateSize; ++j )
        {
			*(ptrStates + j) = pAntCommon->randomState(rand);
			COUNTERS_TYPE action = pAntCommon->randomAction(rand);
            *(ptrActions + j) = action;
        }
    }
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
size_t CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::countIndex(INPUT_TYPE* mas) const
{
    size_t index = 0;//, power = 0;
    for ( int i=0; i<4; ++i)
    {
        index += (mas[i] << i);
    }
    return index;// % stateSize;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
COUNTERS_TYPE CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::getNextState(COUNTERS_TYPE currentState, INPUT_TYPE* input) const
{
    size_t index = countIndex( input );
	COUNTERS_TYPE * ptrStates = buffer + currentState * 2 * stateSize;
	COUNTERS_TYPE state = *(ptrStates + index);
    return state;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
COUNTERS_TYPE CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::getAction(COUNTERS_TYPE currentState, INPUT_TYPE* input) const
{
    size_t index = countIndex( input );
	COUNTERS_TYPE * ptrStates = buffer + currentState * 2 * stateSize;
	COUNTERS_TYPE * ptrActions = ptrStates + stateSize;
	COUNTERS_TYPE action = *(ptrActions + index);
    return action;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
size_t CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::countIndex(const std::vector<INPUT_TYPE>& input) const
{
	size_t index = 0;//, power = 0;
	BOOST_ASSERT(input.size() == 4);
	for (int i = 0; i < 4; ++i)
	{
		index += (input[i] << i);
	}
	return index;// % stateSize;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
COUNTERS_TYPE CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const
{
	size_t index = countIndex(input);
	COUNTERS_TYPE * ptrStates = buffer + currentState * 2 * stateSize;
	COUNTERS_TYPE state = *(ptrStates + index);
	return state;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
COUNTERS_TYPE CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const
{
	size_t index = countIndex(input);
	COUNTERS_TYPE * ptrStates = buffer + currentState * 2 * stateSize;
	COUNTERS_TYPE * ptrActions = ptrStates + stateSize;
	COUNTERS_TYPE action = *(ptrActions + index);
	return action;
    return 0;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::~CAutomatImpl()
{
    free( buffer );
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
void CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::mutate(CRandom* rand)
{
	size_t k = rand->nextUINT();
	int i = rand->nextUINT()%pAntCommon->statesCount();
	COUNTERS_TYPE * ptrStates = buffer + i * 2 * stateSize;
	COUNTERS_TYPE * ptrActions = ptrStates + stateSize;
	if (k < 32)
	{
		for (size_t j = 0; j < stateSize; ++j)
		{
			*(ptrStates + j) = pAntCommon->randomState(rand);
			*(ptrActions + j) = pAntCommon->randomAction(rand);
		}
	}
	else if (k < 128)
	{
		size_t j = rand->nextUINT()%stateSize;
		*(ptrStates + j) = pAntCommon->randomState(rand);
		*(ptrActions + j) = pAntCommon->randomAction(rand);
	}
	else
	{
		for (size_t i = 0; i < pAntCommon->statesCount(); ++i)
		{
			COUNTERS_TYPE * ptrStates = buffer + i * 2 * stateSize;
			COUNTERS_TYPE * ptrActions = ptrStates + stateSize;
			size_t j = rand->nextUINT()%stateSize;
			*(ptrStates + j) = pAntCommon->randomState(rand);
			*(ptrActions + j) = pAntCommon->randomAction(rand);
		}
	}
	//if ( rand->nextUINT()%100 > 15 )
	//	startState = rand->nextUINT()%pAntCommon->statesCount();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
void CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::crossover(const CAutomat* mother, const CAutomat* father, CRandom* rand)
{
    const CAutomatImpl* moth = static_cast<const CAutomatImpl*>( mother );
    const CAutomatImpl* fath = static_cast<const CAutomatImpl*>( father );
    /*for ( size_t i=0; i<statesCount; ++i)
    {
        COUNTERS_TYPE * ptrStates = buffer + i*2*stateSize;
        COUNTERS_TYPE * ptrActions = ptrStates + stateSize;
        COUNTERS_TYPE * ptrStAnc; 
        COUNTERS_TYPE * ptrActAnc;
        int k = rand()%2;
        if ( k == 0)
            ptrStAnc = moth->buffer + i*2*stateSize;
        else
            ptrStAnc = fath->buffer + i*2*stateSize;
        ptrActAnc = ptrStAnc + stateSize;
        for ( size_t j=0; j<stateSize; ++j )
        {
            *(ptrStates + j) = *(ptrStAnc + j);
            *(ptrActions + j) = *(ptrActAnc + j);
        }
    }*/
    int k;
	for (size_t i = 0; i < pAntCommon->statesCount() * 2 * stateSize; ++i)
    {
        k = rand->nextUINT()%100;

        if ( k >= 50 )
            *(buffer+i) = *(moth->buffer+i);
        else
            *(buffer+i) = *(fath->buffer+i);
    }

    k = rand->nextUINT()%100;
    if ( k >= 50 )
        startState = moth->getStartState();
    else
        startState = fath->getStartState();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
std::vector< CAutomatImplPtr<COUNTERS_TYPE, INPUT_TYPE> > CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::cross(const CAutomat* mother,
                                                 const CAutomat* father, CRandom* rand )
{
    const CAutomatImpl* moth = static_cast<const CAutomatImpl*>( mother );
    const CAutomatImpl* fath = static_cast<const CAutomatImpl*>( father );

    CAutomatImplPtr a1( new CAutomatImpl() );
    CAutomatImplPtr a2( new CAutomatImpl() );
 
    size_t stateSize = fath->stateSize;

	for (size_t i = 0; i < moth->pAntCommon->statesCount(); ++i)
    {
		COUNTERS_TYPE * ptrStates1 = a1->buffer + i * 2 * stateSize;
		COUNTERS_TYPE * ptrActions1 = ptrStates1 + stateSize;
		COUNTERS_TYPE * ptrStates2 = a2->buffer + i * 2 * stateSize;
		COUNTERS_TYPE * ptrActions2 = ptrStates2 + stateSize;

		COUNTERS_TYPE * ptrStAnc1, *ptrStAnc2;
		COUNTERS_TYPE * ptrActAnc1, *ptrActAnc2;
   
        int k = rand->nextUINT()%100;
        if ( k >= 0)
        {
            ptrStAnc1 = moth->buffer + i*2*stateSize;
            ptrStAnc2 = fath->buffer + i*2*stateSize;
        }
        else
        {
            ptrStAnc1 = fath->buffer + i*2*stateSize;
            ptrStAnc2 = moth->buffer + i*2*stateSize;
        }
        ptrActAnc1 = ptrStAnc1 + stateSize;
        ptrActAnc2 = ptrStAnc2 + stateSize;
        for ( size_t j=0; j<stateSize; ++j )
        {
            *(ptrStates1 + j) = *(ptrStAnc1 + j);
            *(ptrActions1 + j) = *(ptrActAnc1 + j);

            *(ptrStates2 + j) = *(ptrStAnc2 + j);
            *(ptrActions2 + j) = *(ptrActAnc2 + j);
        }
    }
    if ( rand->nextUINT()%100 >= 50 )
        a1->startState = moth->getStartState();
    else
        a1->startState = fath->getStartState();

    if ( rand->nextUINT()%100 >= 50 )
        a2->startState = moth->getStartState();
    else
        a2->startState = fath->getStartState();
    std::vector<CAutomatImplPtr> vec;
    vec.push_back( a1 );
    vec.push_back( a2 );
    return vec;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
COUNTERS_TYPE CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::getStartState() const
{
	return (COUNTERS_TYPE)startState;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
CAutomatImplPtr<COUNTERS_TYPE, INPUT_TYPE> CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::createFromBuffer(CAntCommon<COUNTERS_TYPE>* pAntCommon, COUNTERS_TYPE* buf)
{
	CAutomatImplPtr<COUNTERS_TYPE, INPUT_TYPE> impl(new CAutomatImpl(pAntCommon));
    impl->startState = *buf;
    buf += 4;
	memcpy(impl->buffer, buf, 2 * pAntCommon->statesCount()*impl->stateSize);
    return impl;
}
