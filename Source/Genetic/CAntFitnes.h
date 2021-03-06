#pragma once
#include "GeneticParams.h"
#include "GeneticCommon/CleverAntFitnes.h"
#include "GeneticCommon/CleverAntMap.h"

class CAntFitnes : public CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>
{
public:
	virtual ANT_FITNES_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const = 0;

    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
	virtual size_t steps() const;
protected:
	CAntFitnes(size_t);
    std::vector<CMapPtr> maps;
	size_t m_steps;
};

typedef boost::shared_ptr<CAntFitnes> CAntFitnesPtr;

class CAntFitnesCPU : public CAntFitnes
{
public:
	CAntFitnesCPU();
	CAntFitnesCPU(size_t);
	virtual ANT_FITNES_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const;
};
typedef boost::shared_ptr<CAntFitnesCPU> CAntFitnesCPUPtr;


class CAntFitnesNone : public CAntFitnes
{
public:
	CAntFitnesNone();
	CAntFitnesNone(size_t);
	virtual ANT_FITNES_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const;
};
typedef boost::shared_ptr<CAntFitnesNone> CAntFitnesNonePtr;