#pragma once
#include "GeneticAPI/CFitnesCounter.h"
#include "CleverAntMap.h"

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename ANT_FITNESS_TYPE>
class CCleverAntFitnes : public CFitnesCounter<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>
{
public:
	virtual ANT_FITNESS_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const = 0;

	virtual void setMaps(std::vector<CMapPtr> maps) = 0;
	virtual const CMapPtr getMap(size_t i) = 0;
	virtual size_t getMapsCount() = 0;
	virtual size_t steps() const = 0;
protected:
};

template<typename CT, typename IT, typename FT, typename C = CCleverAntFitnes<CT, IT, FT> >
using CCleverAntFitnesPtr = boost::shared_ptr< C >;
