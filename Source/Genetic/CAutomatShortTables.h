#pragma once
#include "GeneticParams.h"

class CAutomatShortTables;
typedef boost::shared_ptr<CAutomatShortTables> CAutomatShortTablesPtr;
static const size_t recordSize = 2;
static const size_t stateShift = 0;
static const size_t actionShift = 1;
static const size_t MAX_PARAMETERS = 3;


class CAutomatShortTables : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatShortTables(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, size_t stateSize);
    CAutomatShortTables( const CAutomatShortTables& automat );
    CAutomatShortTables& operator = ( const CAutomatShortTables& automat );
    virtual void generateRandom( CRandom* rand );
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, std::vector<INPUT_TYPE>* input) const;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, std::vector<INPUT_TYPE>* input) const;
	virtual COUNTERS_TYPE getStartState() const;

	//virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, INPUT_TYPE* input);
	//virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, INPUT_TYPE* input);	
	//virtual size_t countIndex(INPUT_TYPE* input) const;
    
	virtual void mutate( CRandom* rand );
    virtual void crossover( const CAutomat* mother, const CAutomat* father, CRandom* rand );

	static void fillRandom(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, char* buff,
        size_t stSize, CRandom* rand );
	static char generateRandom(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, CRandom* rand);
    //static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatShortTablesPtr createFromBuffer(CStateContainer<COUNTERS_TYPE>* states,
		CActionContainer<COUNTERS_TYPE>* actions, char* buf, size_t stateSize = 16);

    virtual ~CAutomatShortTables();
protected:
	virtual size_t countIndex(std::vector<INPUT_TYPE>* input) const;
	static void toParent(size_t* toMother, const size_t* myMas, const COUNTERS_TYPE* motherMask);
	static void crossMasks(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand);
	static void crossTables(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand);
	static void mutateMask(COUNTERS_TYPE* currentMask, CRandom* rand);
	void mutateTable(COUNTERS_TYPE* currentTable, CRandom* rand);
	static size_t createParentIndex(const size_t* toParent, const size_t* myArray, size_t index, CRandom* rand);
private:
    CAutomatShortTables() {};
	CStateContainer<COUNTERS_TYPE>* states;
	CActionContainer<COUNTERS_TYPE>* actions;
	static size_t statesCount, stateSize, maskSize, tableSize;
	size_t startState, currentState;
	COUNTERS_TYPE * buffer;
};