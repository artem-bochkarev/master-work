#pragma once
#include "GeneticAPI/CLaboratory.h"
typedef char COUNTERS_TYPE;
typedef boost::shared_ptr< CAutomat<COUNTERS_TYPE, COUNTERS_TYPE> > CAutomatPtr;
typedef boost::shared_ptr< CActionContainer<COUNTERS_TYPE> > CActionContainerPtr;
typedef boost::shared_ptr< CStateContainer<COUNTERS_TYPE> > CStateContainerPtr;
typedef boost::shared_ptr< CLaboratory<COUNTERS_TYPE, COUNTERS_TYPE> > CLaboratoryPtr;
typedef boost::shared_ptr< CGeneticAlgorithm<COUNTERS_TYPE, COUNTERS_TYPE> > CGeneticAlgorithmPtr;
typedef boost::shared_ptr< CLaboratoryResult<COUNTERS_TYPE, COUNTERS_TYPE> > CLaboratoryResultPtr;

extern const size_t SHORT_TABLE_COLUMNS;
extern const size_t INPUT_PARAMS_COUNT;