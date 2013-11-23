#pragma once
#include "GeneticParams.h"
#include "GeneticAPI/CFitnessCounter.h"
#include "CMap.h"

class CAntFitnes : public CFitnesCounter<ANT_FITNESS_TYPE, COUNTERS_TYPE, INPUT_TYPE>
{
public:
	virtual ANT_FITNESS_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const = 0;

    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
protected:
    std::vector<CMapPtr> maps;
};

typedef boost::shared_ptr<CAntFitnes> CAntFitnesPtr;

class CAntFitnesCPU : public CAntFitnes
{
public:
	virtual ANT_FITNESS_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const;
};
typedef boost::shared_ptr<CAntFitnesCPU> CAntFitnesCPUPtr;


class CAntFitnesNone : public CAntFitnes
{
public:
	virtual ANT_FITNESS_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const;
};
typedef boost::shared_ptr<CAntFitnesNone> CAntFitnesNonePtr;