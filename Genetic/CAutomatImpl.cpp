#include "stdafx.h"
#include "CAutomatImpl.h"
#include <cmath>
#include <boost/assert.hpp>

CAutomatImpl::CAutomatImpl( CStateContainer* states, CActionContainer* actions, size_t stateSize )
//:CAutomat( states, actions )
:startState(0), stateSize( stateSize )
{
    this->states = states;
    this->actions = actions;
    statesCount = states->size();
    //stateSize = 1 << statesCount;
    buffer = (char*)malloc( 2*statesCount*stateSize );
}

CAutomatImpl::CAutomatImpl(const CAutomatImpl &automat)
:startState(automat.startState), stateSize( automat.stateSize ), statesCount( automat.statesCount )
{
    this->states = automat.states;
    this->actions = automat.actions;
    buffer = (char*)malloc( 2*statesCount*stateSize );
    memcpy( buffer, automat.buffer, 2*statesCount*stateSize );
}

CAutomatImpl& CAutomatImpl::operator =(const CAutomatImpl &automat)
{
    this->states = automat.states;
    this->actions = automat.actions;
    statesCount = automat.statesCount;
    stateSize = automat.stateSize;
    buffer = (char*)malloc( 2*statesCount*stateSize );
    memcpy( buffer, automat.buffer, 2*statesCount*stateSize );
    startState = automat.startState;
    return *this;
}

void CAutomatImpl::generateRandom( CRandom* rand )
{
    for ( size_t i=0; i<statesCount; ++i)
    {
        char * ptrStates = buffer + i*2*stateSize;
        char * ptrActions = ptrStates + stateSize;
        for ( size_t j=0; j<stateSize; ++j )
        {
            *(ptrStates + j) = states->randomState( rand );
            *(ptrActions + j) = actions->randomAction( rand );
        }
    }
    startState = rand->nextUINT()%statesCount; //rand;
}

void CAutomatImpl::fillRandom( CStateContainer* states, CActionContainer* actions, 
                              char* buffer, size_t stSize, CRandom* rand )
{
    size_t statesCount = states->size();
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
            *(ptrStates + j) = states->randomState( rand );
            char action = actions->randomAction( rand );
            if ( action > 2 )
            {
                int k =0;
                ++k;
            }
            *(ptrActions + j) = action;
        }
    }
}

size_t CAutomatImpl::countIndex( int* mas ) const
{
    size_t index = 0;//, power = 0;
    for ( int i=0; i<4; ++i)
    {
        index += (mas[i] << i);
    }
    return index;// % stateSize;
}

char CAutomatImpl::getNextState( char currentState, int* input )
{
    size_t index = countIndex( input );
    char * ptrStates = buffer + currentState*2*stateSize;
    char state = *(ptrStates+index);
    return state;
}

char CAutomatImpl::getAction( char currentState, int* input )
{
    size_t index = countIndex( input );
    char * ptrStates = buffer + currentState*2*stateSize;
    char * ptrActions = ptrStates + stateSize;
    char state = *(ptrStates+index);
    char action = *(ptrActions+index);
    return action;
}

size_t CAutomatImpl::countIndex( std::vector<int>* input ) const
{
    BOOST_ASSERT(0);
    return 0;// % stateSize;
}

char CAutomatImpl::getNextState( char currentState, std::vector<int>* input )
{
    BOOST_ASSERT(0);
    return 0;
}

char CAutomatImpl::getAction( char currentState, std::vector<int>* input )
{
    BOOST_ASSERT(0);
    return 0;
}

CAutomatImpl::~CAutomatImpl()
{
    free( buffer );
}

void CAutomatImpl::mutate( CRandom* rand )
{
    int i = rand->nextUINT()%statesCount;
    char * ptrStates = buffer + i*2*stateSize;
    char * ptrActions = ptrStates + stateSize;
    for ( size_t j=0; j<stateSize; ++j )
    {
        *(ptrStates + j) = states->randomState( rand );
        *(ptrActions + j) = actions->randomAction( rand );
    }
    if ( rand->nextUINT()%100 > 15 )
        startState = rand->nextUINT()%statesCount;
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
    for ( size_t i=0; i < statesCount*2*stateSize; ++i)
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

    CAutomatImplPtr a1( new CAutomatImpl( moth->states, moth->actions ) );
    CAutomatImplPtr a2( new CAutomatImpl( moth->states, moth->actions ) );
 
    size_t statesCount = fath->statesCount;
    size_t stateSize = fath->stateSize;

    for ( size_t i=0; i<fath->statesCount; ++i)
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
    return startState;
}

char CAutomatImpl::generateRandom( CStateContainer* states, CActionContainer* actions, CRandom* rand )
{
    return rand->nextUINT()%states->size();
}

CAutomatImpl CAutomatImpl::createFromBuffer( CStateContainer* states, 
        CActionContainer* actions, char* buf, size_t stateSize )
{
    CAutomatImpl impl( states, actions, stateSize );
    impl.startState = *buf;
    buf += 4;
    memcpy( impl.buffer, buf, 2*impl.statesCount*impl.stateSize );
    return impl;
}
