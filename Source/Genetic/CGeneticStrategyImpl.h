#pragma once

#include "CGeneticStrategyCommon.h"
#include "CMap.h"
#include "CAutomatImpl.h"
#include "CLabResultMulti.h"
#include "Tools/Logger.h"

template<class AUTOMAT_TYPE>
class CLocalInvoker;

template<class AUTOMAT_TYPE>
class CMainInvoker;

template<class AUTOMAT_TYPE>
class CGeneticStrategyImpl : public CGeneticStrategyCommon
{
public:
	friend class CLocalInvoker<AUTOMAT_TYPE>;
	friend class CMainInvoker<AUTOMAT_TYPE>;
    CGeneticStrategyImpl(CStateContainer<COUNTERS_TYPE>* states, CActionContainer<COUNTERS_TYPE>* actions, 
        CLabResultMulti* res, CAntFitnesPtr fitnes, const std::vector< std::string >& strings, Tools::Logger& );
    
    virtual void run();

    virtual void nextGeneration( CRandom* rand );

    //virtual void setMaps( std::vector<CMapPtr> maps );

    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
    CAutomatPtr getBestIndivid() const;
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