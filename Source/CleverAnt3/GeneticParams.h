#pragma once
#include "GeneticAPI/CLaboratory.h"
#include "GeneticAPI/CLaboratoryResult.h"
typedef char COUNTERS_TYPE;
typedef int INPUT_TYPE;
typedef double ANT_FITNESS_TYPE;

//typedef boost::shared_ptr< CActionContainer<COUNTERS_TYPE> > CActionContainerPtr;
typedef boost::shared_ptr< CLaboratory<COUNTERS_TYPE, INPUT_TYPE> > CLaboratoryPtr;
typedef boost::shared_ptr< CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE> > CGeneticAlgorithmPtr;
typedef boost::shared_ptr< CLaboratoryResult<COUNTERS_TYPE, INPUT_TYPE> > CLaboratoryResultPtr;

static const size_t SHORT_TABLE_COLUMNS = 4;
static const size_t INPUT_PARAMS_COUNT = 4;
static const size_t DEFAULT_STEPS_COUNT = 110;

class AntCommon
{
public:
	AntCommon(size_t, CActionContainerPtr<COUNTERS_TYPE> );
	size_t statesCount() const;
	CActionContainer<COUNTERS_TYPE>* actions() const;
	COUNTERS_TYPE randomState(CRandom* rand) const;
	COUNTERS_TYPE randomAction(CRandom* rand) const;
private:
	AntCommon();
	CActionContainerPtr<COUNTERS_TYPE> actionsPtr;
	size_t m_statesCount;
};

typedef boost::shared_ptr<AntCommon> AntCommonPtr;