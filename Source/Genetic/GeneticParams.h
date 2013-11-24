#pragma once
#include "GeneticAPI/CLaboratory.h"
#include "GeneticAPI/CLaboratoryResult.h"
typedef char COUNTERS_TYPE;
typedef int INPUT_TYPE;
typedef double ANT_FITNESS_TYPE;

typedef boost::shared_ptr< CAutomat<COUNTERS_TYPE, INPUT_TYPE> > CAutomatPtr;
typedef boost::shared_ptr< CActionContainer<COUNTERS_TYPE> > CActionContainerPtr;
typedef boost::shared_ptr< CStateContainer<COUNTERS_TYPE> > CStateContainerPtr;
typedef boost::shared_ptr< CLaboratory<COUNTERS_TYPE, INPUT_TYPE> > CLaboratoryPtr;
typedef boost::shared_ptr< CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE> > CGeneticAlgorithmPtr;
typedef boost::shared_ptr< CLaboratoryResult<COUNTERS_TYPE, INPUT_TYPE> > CLaboratoryResultPtr;

static const size_t SHORT_TABLE_COLUMNS = 3;
static const size_t INPUT_PARAMS_COUNT = 4;