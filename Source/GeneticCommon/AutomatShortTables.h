#pragma once
#include "AntCommon.h"

//class CAutomatShortTables;
static const size_t recordSize = 2;
static const size_t stateShift = 0;
static const size_t actionShift = 1;


template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
class CAutomatShortTables;


template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT,
	typename C = CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT> >
	using CAutomatShortTablesPtr = boost::shared_ptr< C >;
//typedef boost::shared_ptr<CAutomatShortTables> CAutomatShortTablesPtr;

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT>
class CAutomatShortTables final : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatShortTables();
	CAutomatShortTables(CAntCommon<COUNTERS_TYPE>* pAntCommon);// size_t stateSize);
    CAutomatShortTables( const CAutomatShortTables& automat );
    CAutomatShortTables& operator = ( const CAutomatShortTables& automat );
    virtual void generateRandom( CRandom* rand ) override;
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual std::pair<COUNTERS_TYPE, COUNTERS_TYPE> getNextStateAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getStartState() const override;

	virtual void mutate(CRandom* rand) override;
	virtual void crossover(const CAutomat* mother, const CAutomat* father, CRandom* rand) override;

	static void fillRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, char* buff, CRandom* rand);
	static char generateRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, CRandom* rand);
    //static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatShortTablesPtr<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT> 
		createFromBuffer(CAntCommon<COUNTERS_TYPE>* pAntCommon, COUNTERS_TYPE* buf);

    virtual ~CAutomatShortTables();

	static void toParent(size_t* toMother, const size_t* myMas, const COUNTERS_TYPE* motherMask);
	static void crossMasks(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand);
	static void crossTables(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand);
	static void mutateMask(COUNTERS_TYPE* currentMask, CRandom* rand);
	static void randomMask(COUNTERS_TYPE* mask, CRandom* rand);
	static void mutateTable(COUNTERS_TYPE* currentTable, CRandom* rand, CAntCommon<COUNTERS_TYPE>* );
	static void randomTable(COUNTERS_TYPE* table, CRandom* rand, CAntCommon<COUNTERS_TYPE>* );
	static size_t createParentIndex(const size_t* toParent, size_t index, CRandom* rand);
protected:
	size_t countIndex(const std::vector<INPUT_TYPE>& input, size_t currentsState) const;
private:
	static const size_t commonDataSize = 4;
	static const size_t maskSize = INPUT_PARAMS_COUNT;
	static const size_t tableSize = 2 * (1 << SHORT_TABLE_COLUMNS);
	static const size_t stateSize = tableSize + maskSize;

	COUNTERS_TYPE startState;
	COUNTERS_TYPE * buffer;
	CAntCommon<COUNTERS_TYPE>* pAntCommon;
};

#include "AutomatShortTables.hpp"