#pragma once
#include "GeneticParams.h"
#include "GeneticAPI/CAutomat.h"

class CAutomatImpl;
typedef boost::shared_ptr<CAutomatImpl> CAutomatImplPtr;

class CAutomatImpl : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatImpl(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, size_t stateSize = 16);
    CAutomatImpl( const CAutomatImpl& automat );
    CAutomatImpl& operator = ( const CAutomatImpl& automat );
    virtual void generateRandom( CRandom* rand );
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, std::vector<int>* input) const;
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, int* input) const;
	virtual COUNTERS_TYPE getStartState() const;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, std::vector<int>* input) const;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, int* input) const;
    virtual size_t countIndex( std::vector<int>* input ) const;
    virtual size_t countIndex( int* input ) const;
    virtual void mutate( CRandom* rand );
    virtual void crossover( const CAutomat* mother, const CAutomat* father, CRandom* rand );

	static void fillRandom(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, char* buff,
        size_t stSize, CRandom* rand );
    static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatImpl createFromBuffer(CStateContainer<COUNTERS_TYPE>* states,
		CActionContainer<COUNTERS_TYPE>* actions, char* buf, size_t stateSize = 16);

    virtual ~CAutomatImpl();
private:
    CAutomatImpl() {};
	CStateContainer<COUNTERS_TYPE>* states;
	CActionContainer<COUNTERS_TYPE>* actions;
    size_t statesCount, stateSize, startState;
    char * buffer;
};