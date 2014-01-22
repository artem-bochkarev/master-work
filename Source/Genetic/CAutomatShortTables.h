#pragma once
#include "GeneticParams.h"

//class CAutomatShortTables;
static const size_t recordSize = 2;
static const size_t stateShift = 0;
static const size_t actionShift = 1;

class CAutomatShortTables;
typedef boost::shared_ptr<CAutomatShortTables> CAutomatShortTablesPtr;

class CAutomatShortTables final : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatShortTables(AntCommon* pAntCommon);// size_t stateSize);
    CAutomatShortTables( const CAutomatShortTables& automat );
    CAutomatShortTables& operator = ( const CAutomatShortTables& automat );
    virtual void generateRandom( CRandom* rand ) override;
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getStartState() const override;

	virtual void mutate(CRandom* rand) override;
	virtual void crossover(const CAutomat* mother, const CAutomat* father, CRandom* rand) override;

	static void fillRandom(AntCommon* pAntCommon, char* buff, CRandom* rand );
	static char generateRandom(AntCommon* pAntCommon, CRandom* rand);
    //static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatShortTablesPtr createFromBuffer(AntCommon* pAntCommon, COUNTERS_TYPE* buf);

    virtual ~CAutomatShortTables();
protected:
	size_t countIndex(const std::vector<INPUT_TYPE>& input, size_t currentsState) const;
	static void toParent(size_t* toMother, const size_t* myMas, const COUNTERS_TYPE* motherMask);
	void crossMasks(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand);
	void crossTables(COUNTERS_TYPE* childMask, const COUNTERS_TYPE* motherMask, const COUNTERS_TYPE* fatherMask, CRandom* rand);
	void mutateMask(COUNTERS_TYPE* currentMask, CRandom* rand);
	static void randomMask(COUNTERS_TYPE* mask, CRandom* rand);
	void mutateTable(COUNTERS_TYPE* currentTable, CRandom* rand);
	static void randomTable(COUNTERS_TYPE* table, CRandom* rand, AntCommon* );
	static size_t createParentIndex(const size_t* toParent, const size_t* myArray, size_t index, CRandom* rand);
private:
    CAutomatShortTables() {};
	static size_t commonDataSize, stateSize, maskSize, tableSize;
	COUNTERS_TYPE startState;
	COUNTERS_TYPE * buffer;
	AntCommon* pAntCommon;
};
