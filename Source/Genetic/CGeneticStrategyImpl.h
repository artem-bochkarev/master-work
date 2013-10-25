#pragma once

#include "CGeneticStrategyCommon.h"
#include "CMap.h"
#include "CAutomatImpl.h"
#include "CLabResultMulti.h"
#include "Tools/Logger.h"

class CLocalInvoker;

class CGeneticStrategyImpl : public CGeneticStrategyCommon
{
public:
    friend class CLocalInvoker;
    friend class CMainInvoker;
    CGeneticStrategyImpl(CStateContainer* states, CActionContainer* actions, 
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
    void freeIndivids( CAutomatImpl*** ind );
    void fillCache() const;
    void nextGeneration( size_t start, size_t finish, CRandom* rand );
    void CGeneticStrategyImpl::preGeneration();
    void CGeneticStrategyImpl::postGeneration();

    mutable double** cachedResult;
    mutable bool isCacheValid;
    
    CAutomatImpl*** individs;
    CAutomatImpl*** newIndivids;
    std::vector<CMapPtr> maps;

    int cnt;

    CRandomPtr m_pRandom;
};