#pragma once
#include "GeneticAPI/CLaboratory.h"
#include "GeneticAPI/CLaboratoryResult.h"
typedef int COUNTERS_TYPE;
typedef int INPUT_TYPE;
typedef double ANT_FITNES_TYPE;

//typedef boost::shared_ptr< CActionContainer<COUNTERS_TYPE> > CActionContainerPtr;
typedef boost::shared_ptr< CLaboratory<COUNTERS_TYPE, INPUT_TYPE> > CLaboratoryPtr;
//typedef boost::shared_ptr< CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE> > CGeneticAlgorithmPtr;
typedef boost::shared_ptr< CLaboratoryResult<COUNTERS_TYPE, INPUT_TYPE> > CLaboratoryResultPtr;

static const size_t STATES_COUNT = 5;
static const size_t SHORT_TABLE_COLUMNS = 4;
static const size_t MAX_DEPTH = 4;
static const size_t INPUT_PARAMS_COUNT = 8;
static const size_t DEFAULT_STEPS_COUNT = 100;

#include "GeneticCommon/AutomatShortTables.h"
#include "GeneticCommon/AutomatShortTablesStatic.hpp"
#include "GeneticCommon/AutomatDecisionTreeStatic.hpp"

typedef CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT, STATES_COUNT> AUTOMAT;
static const char* clFilePath = "../CleverAnt3/genShortTables.cl";
//typedef CAutomatDecisionTreeStatic<COUNTERS_TYPE, INPUT_TYPE, MAX_DEPTH, INPUT_PARAMS_COUNT, STATES_COUNT> AUTOMAT;
//static const char* clFilePath = "../CleverAnt3/genDecisionTree.cl";