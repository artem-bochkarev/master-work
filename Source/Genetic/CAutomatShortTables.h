#pragma once
#include "GeneticParams.h"

class CAutomatShortTables;
typedef boost::shared_ptr<CAutomatShortTables> CAutomatShortTablesPtr;

class CAutomatShortTables : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatShortTables(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, size_t stateSize = 16);
    CAutomatShortTables( const CAutomatShortTables& automat );
    CAutomatShortTables& operator = ( const CAutomatShortTables& automat );
    virtual void generateRandom( CRandom* rand );
    virtual char getNextState( char currentState, std::vector<int>* input );
    virtual char getNextState( char currentState, int* input );
    virtual char getStartState() const;
    virtual char getAction( char currentState, std::vector<int>* input );
    virtual char getAction( char currentState, int* input );
    virtual size_t countIndex( std::vector<int>* input ) const;
    virtual size_t countIndex( int* input ) const;
    virtual void mutate( CRandom* rand );
    virtual void crossover( const CAutomat* mother, const CAutomat* father, CRandom* rand );

	static void fillRandom(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, char* buff,
        size_t stSize, CRandom* rand );
	static char generateRandom(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, CRandom* rand);
    //static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatShortTablesPtr createFromBuffer(CStateContainer<COUNTERS_TYPE>* states,
		CActionContainer<COUNTERS_TYPE>* actions, char* buf, size_t stateSize = 16);

    virtual ~CAutomatShortTables();
private:
    CAutomatShortTables() {};
	CStateContainer<COUNTERS_TYPE>* states;
	CActionContainer<COUNTERS_TYPE>* actions;
    size_t statesCount, stateSize, startState, currentState;
	COUNTERS_TYPE * buffer;
};