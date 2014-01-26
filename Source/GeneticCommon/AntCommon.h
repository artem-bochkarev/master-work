#pragma once
#include <boost\shared_ptr.hpp>

template<typename COUNTERS_TYPE>
class CAntCommon
{
public:
	CAntCommon(size_t statesCount, CActionContainerPtr aPtr)
		:m_statesCount(statesCount), actionsPtr(aPtr) {}
	
	size_t statesCount() const
	{
		return m_statesCount;
	}

	CActionContainer<COUNTERS_TYPE>* actions() const
	{
		return actionsPtr.get();
	}

	COUNTERS_TYPE randomState(CRandom* rand) const
	{
		return rand->nextUINT() % statesCount();
	}

	COUNTERS_TYPE randomAction(CRandom* rand) const
	{
		return actions()->randomAction(rand);
	}

private:
	CAntCommon();
	CActionContainerPtr actionsPtr;
	size_t m_statesCount;
};

template<typename T, typename C = CAntCommon<T> >
using CAntCommonPtr = boost::shared_ptr< C >;
