#pragma once
#include "GeneticParams.h"

//class CAutomatShortTables;
static const size_t recordSize = 2;
static const size_t stateShift = 0;
static const size_t actionShift = 1;
static const size_t MAX_PARAMETERS = SHORT_TABLE_COLUMNS;


class CAutomatShortTables final : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatShortTables(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions);// size_t stateSize);
    CAutomatShortTables( const CAutomatShortTables& automat );
    CAutomatShortTables& operator = ( const CAutomatShortTables& automat );
    virtual void generateRandom( CRandom* rand ) override;
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, std::vector<INPUT_TYPE>* input) const override;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, std::vector<INPUT_TYPE>* input) const override;
	virtual COUNTERS_TYPE getStartState() const override;

	virtual void mutate(CRandom* rand) override;
	virtual void crossover(const CAutomat* mother, const CAutomat* father, CRandom* rand) override;

	static void fillRandom(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, char* buff,
        size_t stSize, CRandom* rand );
	static char generateRandom(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, CRandom* rand);
    //static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	//static CAutomatShortTablesPtr createFromBuffer(CStateContainer<COUNTERS_TYPE>* states,
		//CActionContainer<COUNTERS_TYPE>* actions, char* buf, size_t stateSize = 16);

    virtual ~CAutomatShortTables();
protected:
	size_t countIndex(std::vector<INPUT_TYPE>* input, size_t currentsState) const;
	static void toParent(size_t* toMother, const size_t* myMas, const COUNTERS_TYPE* motherMask);
	static void crossMasks(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand);
	static void crossTables(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand);
	static void mutateMask(COUNTERS_TYPE* currentMask, CRandom* rand);
	static void randomMask(COUNTERS_TYPE* mask, CRandom* rand);
	static void mutateTable(COUNTERS_TYPE* currentTable, CRandom* rand);
	static void randomTable(COUNTERS_TYPE* table, CRandom* rand);
	static size_t createParentIndex(const size_t* toParent, const size_t* myArray, size_t index, CRandom* rand);
private:
    CAutomatShortTables() {};
	static CStateContainer<COUNTERS_TYPE>* states;
	static CActionContainer<COUNTERS_TYPE>* actions;
	static size_t commonDataSize, statesCount, stateSize, maskSize, tableSize;
	COUNTERS_TYPE startState;
	COUNTERS_TYPE * buffer;
};

typedef boost::shared_ptr<CAutomatShortTables> CAutomatShortTablesPtr;