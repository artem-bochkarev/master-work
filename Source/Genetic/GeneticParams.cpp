#include "stdafx.h"
#include "GeneticParams.h"

AntCommon::AntCommon(size_t statesCount, CActionContainerPtr aPtr)
:m_statesCount(statesCount), actionsPtr(aPtr) {}

size_t AntCommon::statesCount() const 
{ 
	return m_statesCount; 
}

CActionContainer<COUNTERS_TYPE>* AntCommon::actions() const 
{ 
	return actionsPtr.get(); 
}

COUNTERS_TYPE AntCommon::randomState(CRandom* rand) const 
{ 
	return rand->nextUINT()%statesCount(); 
}

COUNTERS_TYPE AntCommon::randomAction(CRandom* rand) const
{
	return actions()->randomAction(rand);
}