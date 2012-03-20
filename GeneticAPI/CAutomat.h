#pragma once
#include "CAction.h"
#include "CState.h"

class CRandom;

class CAutomat
{
public:
    //CAutomat( CStateContainer* states, CActionContainer* actions );
    virtual void generateRandom( CRandom* rand ) = 0;
    virtual char getNextState( char currentState, std::vector<int>* input ) = 0;
    virtual char getStartState() const  = 0;
    virtual char getAction( char currentState, std::vector<int>* input )  = 0;
    virtual size_t countIndex( std::vector<int>* input ) const  = 0;
    virtual void mutate( CRandom* rand )  = 0;
    virtual void crossover( const CAutomat* mother, const CAutomat* father, CRandom* rand ) = 0;
    virtual ~CAutomat() {};
private:
};

typedef boost::shared_ptr<CAutomat> CAutomatPtr;