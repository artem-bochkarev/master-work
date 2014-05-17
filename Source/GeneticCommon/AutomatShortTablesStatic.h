#pragma once
#include "AntCommon.h"
#include "AutomatShortTables.h"


template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
class CAutomatShortTablesStatic;


template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT,
	typename C = CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT> >
	using CAutomatShortTablesStaticPtr = boost::shared_ptr< C >;

template<typename COUNTERS_TYPE, typename INPUT_TYPE, size_t SHORT_TABLE_COLUMNS, size_t INPUT_PARAMS_COUNT, size_t STATES_COUNT>
class CAutomatShortTablesStatic final : public CAutomat<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CAutomatShortTablesStatic();
	CAutomatShortTablesStatic(CAntCommon<COUNTERS_TYPE>* pAntCommon);// size_t stateSize);
	CAutomatShortTablesStatic(const CAutomatShortTablesStatic& automat);
	CAutomatShortTablesStatic& operator = (const CAutomatShortTablesStatic& automat);
	virtual void generateRandom(CRandom* rand) override;
	virtual COUNTERS_TYPE getNextState(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual std::pair<COUNTERS_TYPE, COUNTERS_TYPE> getNextStateAction(COUNTERS_TYPE currentState, const std::vector<INPUT_TYPE>& input) const override;
	virtual COUNTERS_TYPE getStartState() const override;

	virtual void mutate(CRandom* rand) override;
	virtual void crossover(const CAutomat* mother, const CAutomat* father, CRandom* rand) override;

	static void fillRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, char* buff, CRandom* rand);
	static char generateRandom(CAntCommon<COUNTERS_TYPE>* pAntCommon, CRandom* rand);
	//static std::vector<CAutomatImplPtr> cross( const CAutomat* mother, const CAutomat* father, CRandom* rand );
	static CAutomatShortTablesStaticPtr<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT>
		createFromBuffer(CAntCommon<COUNTERS_TYPE>* pAntCommon, COUNTERS_TYPE* buf);

	virtual ~CAutomatShortTablesStatic();
	size_t getBufferOffset() const;
	const CAntCommon<COUNTERS_TYPE>* getAntCommon() const;

	static const size_t commonDataSize = 4;
	static const size_t maskSize = INPUT_PARAMS_COUNT;
	static const size_t tableSize = 2 * (1 << SHORT_TABLE_COLUMNS);
	static const size_t stateSize = tableSize + maskSize;
	static const size_t bufferSize = STATES_COUNT * stateSize + commonDataSize;
protected:
	size_t countIndex(const std::vector<INPUT_TYPE>& input, size_t currentsState) const;
private:
	
	CAntCommon<COUNTERS_TYPE>* pAntCommon;
	COUNTERS_TYPE startState;
	mutable COUNTERS_TYPE buffer[bufferSize];
};