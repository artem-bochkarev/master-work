#include "stdafx.h"
#include "CAutomatImpl.h"
#include <cmath>
#include <boost/assert.hpp>

CAutomatImpl::CAutomatImpl( AntCommon* pAntCommon )
//:CAutomat( states, actions )
:startState(0), pAntCommon(pAntCommon)
{
	stateSize = 1 << pAntCommon->statesCount();
	buffer = (char*)malloc(2 * pAntCommon->statesCount()*stateSize);
}

CAutomatImpl::CAutomatImpl(const CAutomatImpl &automat)
:startState(automat.startState), stateSize(automat.stateSize), pAntCommon(automat.pAntCommon)
{
	buffer = (char*)malloc(2 * pAntCommon->statesCount() * stateSize);
	memcpy(buffer, automat.buffer, 2 * pAntCommon->statesCount() * stateSize);
}

CAutomatImpl& CAutomatImpl::operator =(const CAutomatImpl &automat)
{
    stateSize = automat.stateSize;
	pAntCommon = automat.pAntCommon;
	buffer = (char*)malloc(2 * pAntCommon->statesCount() * stateSize);
	memcpy(buffer, automat.buffer, 2 * pAntCommon->statesCount() * stateSize);
    startState = automat.startState;
    return *this;
}

void CAutomatImpl::generateRandom( CRandom* rand )
{
	for (size_t i = 0; i < pAntCommon->statesCount(); ++i)
    {
        char * ptrStates = buffer + i*2*stateSize;
        char * ptrActions = ptrStates + stateSize;
        for ( size_t j=0; j<stateSize; ++j )
        {
			*(ptrStates + j) = pAntCommon->randomState(rand);
			*(ptrActions + j) = pAntCommon->randomAction(rand);
        }
    }
	startState = rand->nextUINT()%pAntCommon->statesCount(); //rand;
}

void CAutomatImpl::fillRandom( AntCommon* pAntCommon, 
	COUNTERS_TYPE* buffer, size_t stSize, CRandom* rand)
{
	size_t statesCount = pAntCommon->statesCount();
    size_t stateSize = stSize;
    *buffer = (char)( rand->nextUINT()%statesCount );
    ++buffer;
    *buffer = (char)( rand->nextUINT()%statesCount );
    ++buffer;
    *buffer = (char)( rand->nextUINT()%statesCount );
    ++buffer;
    *buffer = (char)( rand->nextUINT()%statesCount );
    ++buffer;
    for ( size_t i=0; i<statesCount; ++i)
    {
        char * ptrStates = buffer + i*2*stateSize;
        char * ptrActions = ptrStates + stateSize;
        for ( size_t j=0; j<stateSize; ++j )
        {
			*(ptrStates + j) = pAntCommon->randomState(rand);
			char action = pAntCommon->randomAction(rand);
            *(ptrActions + j) = action;
        }
    }
}

size_t CAutomatImpl::countIndex(INPUT_TYPE* mas) const
{
    size_t index = 0;//, power = 0;
    for ( int i=0; i<4; ++i)
    {
        index += (mas[i] << i);
    }
    return index;// % stateSize;
}

char CAutomatImpl::getNextState(char currentState, INPUT_TYPE* input) const
{
    size_t index = countIndex( input );
    char * ptrStates = buffer + currentState*2*stateSize;
    char state = *(ptrStates+index);
    return state;
}

char CAutomatImpl::getAction(char currentState, INPUT_TYPE* input) const
{
    size_t index = countIndex( input );
    char * ptrStates = buffer + currentState*2*stateSize;
    char * ptrActions = ptrStates + stateSize;
    char action = *(ptrActions+index);
    return action;
}

size_t CAutomatImpl::countIndex(const std::vector<INPUT_TYPE>& input) const
{
	size_t index = 0;//, power = 0;
	BOOST_ASSERT(input.size() == 4);
	for (int i = 0; i < 4; ++i)
	{
		index += (input[i] << i);
	}
	return index;// % stateSize;
}

char CAutomatImpl::getNextState(char currentState, const std::vector<INPUT_TYPE>& input) const
{
	size_t index = countIndex(input);
	char * ptrStates = buffer + currentState * 2 * stateSize;
	char state = *(ptrStates + index);
	return state;
}

char CAutomatImpl::getAction(char currentState, const std::vector<INPUT_TYPE>& input) const
{
	size_t index = countIndex(input);
	char * ptrStates = buffer + currentState * 2 * stateSize;
	char * ptrActions = ptrStates + stateSize;
	char action = *(ptrActions + index);
	return action;
    return 0;
}

CAutomatImpl::~CAutomatImpl()
{
    free( buffer );
}

void CAutomatImpl::mutate( CRandom* rand )
{
	size_t k = rand->nextUINT();
	int i = rand->nextUINT()%pAntCommon->statesCount();
	char * ptrStates = buffer + i * 2 * stateSize;
	char * ptrActions = ptrStates + stateSize;
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
			char * ptrStates = buffer + i * 2 * stateSize;
			char * ptrActions = ptrStates + stateSize;
			size_t j = rand->nextUINT()%stateSize;
			*(ptrStates + j) = pAntCommon->randomState(rand);
			*(ptrActions + j) = pAntCommon->randomAction(rand);
		}
	}
	//if ( rand->nextUINT()%100 > 15 )
	//	startState = rand->nextUINT()%pAntCommon->statesCount();
}

void CAutomatImpl::crossover( const CAutomat* mother, const CAutomat* father, CRandom* rand )
{
    const CAutomatImpl* moth = static_cast<const CAutomatImpl*>( mother );
    const CAutomatImpl* fath = static_cast<const CAutomatImpl*>( father );
    /*for ( size_t i=0; i<statesCount; ++i)
    {
        char * ptrStates = buffer + i*2*stateSize;
        char * ptrActions = ptrStates + stateSize;
        char * ptrStAnc; 
        char * ptrActAnc;
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

std::vector<CAutomatImplPtr> CAutomatImpl::cross( const CAutomat* mother, 
                                                 const CAutomat* father, CRandom* rand )
{
    const CAutomatImpl* moth = static_cast<const CAutomatImpl*>( mother );
    const CAutomatImpl* fath = static_cast<const CAutomatImpl*>( father );

    CAutomatImplPtr a1( new CAutomatImpl() );
    CAutomatImplPtr a2( new CAutomatImpl() );
 
    size_t stateSize = fath->stateSize;

	for (size_t i = 0; i < moth->pAntCommon->statesCount(); ++i)
    {
        char * ptrStates1 = a1->buffer + i*2*stateSize;
        char * ptrActions1 = ptrStates1 + stateSize;
        char * ptrStates2 = a2->buffer + i*2*stateSize;
        char * ptrActions2 = ptrStates2 + stateSize;

        char * ptrStAnc1, *ptrStAnc2; 
        char * ptrActAnc1, *ptrActAnc2;
   
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

char CAutomatImpl::getStartState() const
{
    return (char)startState;
}

CAutomatImpl CAutomatImpl::createFromBuffer(AntCommon* pAntCommon, char* buf)
{
	CAutomatImpl impl(pAntCommon);
    impl.startState = *buf;
    buf += 4;
	memcpy(impl.buffer, buf, 2 * pAntCommon->statesCount()*impl.stateSize);
    return impl;
}
