#pragma once

#include "GeneticCommon/CleverAntStrategy.hpp"
#include "GeneticCommon/CleverAntMap.h"
#include "GeneticCommon/AutomatImpl.h"
#include "GeneticCommon/LabResultMulti.hpp"
#include "Tools/Logger.h"

template<class AUTOMAT_TYPE>
class CLocalInvoker;

template<class AUTOMAT_TYPE>
class CMainInvoker;

template<class AUTOMAT_TYPE>
class CGeneticStrategyImpl : public CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>
{
public:
	friend class CLocalInvoker<AUTOMAT_TYPE>;
	friend class CMainInvoker<AUTOMAT_TYPE>;
	CGeneticStrategyImpl(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* res, 
		CAntFitnesPtr fitnes,
		const std::vector< std::string >& strings, Tools::Logger& );
    
    virtual void run();

    virtual void nextGeneration( CRandom* rand );

    //virtual void setMaps( std::vector<CMapPtr> maps );

    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
	CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> getBestIndivid() const;
    virtual ~CGeneticStrategyImpl();
    virtual std::string getDeviceType() const;
private:
    void setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand );

    CGeneticStrategyImpl();
    CGeneticStrategyImpl( const CGeneticStrategyImpl& );
	void freeIndivids(AUTOMAT_TYPE*** ind);
    void fillCache() const;
    void nextGeneration( size_t start, size_t finish, CRandom* rand );
    void CGeneticStrategyImpl::preGeneration();
    void CGeneticStrategyImpl::postGeneration();

    mutable double** cachedResult;
    mutable bool isCacheValid;
    
	AUTOMAT_TYPE*** individs;
	AUTOMAT_TYPE*** newIndivids;
    std::vector<CMapPtr> maps;

    int cnt;
    CRandomPtr m_pRandom;
};

#include "CGeneticStrategyImpl.hpp"