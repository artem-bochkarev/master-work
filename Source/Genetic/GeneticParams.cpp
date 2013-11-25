#include "stdafx.h"
#include "GeneticParams.h"

CStateContainer<COUNTERS_TYPE>* ant_common::states = nullptr;
CActionContainer<COUNTERS_TYPE>* ant_common::actions = nullptr;
size_t ant_common::statesCount = 4;