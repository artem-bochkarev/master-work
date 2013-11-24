#pragma once
#include "CAction.h"
#include "CState.h"

class CRandom;

template< typename NUMBERS_TYPE, typename INPUT_TYPE > class CAutomat
{
public:
    virtual void generateRandom( CRandom* rand ) = 0;
	virtual NUMBERS_TYPE getNextState(NUMBERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const = 0;
	virtual NUMBERS_TYPE getAction(NUMBERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const = 0;
	virtual NUMBERS_TYPE getStartState() const = 0;
    virtual void mutate( CRandom* rand )  = 0;
    virtual void crossover( const CAutomat* mother, const CAutomat* father, CRandom* rand ) = 0;
    virtual ~CAutomat() {};
private:
};

//typedef boost::shared_ptr<CAutomat> CAutomatPtr;