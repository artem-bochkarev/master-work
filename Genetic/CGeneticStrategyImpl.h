#pragma once

#include "GeneticAPI/CGeneticStrategy.h"
#include "GeneticAPI/CMap.h"
#include "CAutomatImpl.h"
#include "GeneticAPI/CInvoker.h"
#include "CLabResultMulti.h"

class CLocalInvoker;

class CGeneticStrategyImpl : public CGeneticStrategy
{
public:
    friend class CLocalInvoker;
    friend class CMainInvoker;
    CGeneticStrategyImpl(CStateContainer* states, CActionContainer* actions, 
        CLabResultMulti* res, const std::vector< std::string >& strings );
    virtual void nextGeneration( CRandom* rand );

    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();

    virtual void addMap( CMapPtr map );
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
    CAutomatPtr getBestIndivid() const;
    virtual ~CGeneticStrategyImpl();
    virtual CInvoker* getInvoker() const;
    virtual size_t getN() const;
    virtual size_t getM() const;
    virtual std::string getDeviceType() const;
private:
    void setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand );
    int N, M;

    CGeneticStrategyImpl();
    CGeneticStrategyImpl( const CGeneticStrategyImpl& );
    void freeIndivids( CAutomatImpl*** ind );
    void fillCache() const;
    void nextGeneration( size_t start, size_t finish, CRandom* rand );
    void CGeneticStrategyImpl::preGeneration();
    void CGeneticStrategyImpl::postGeneration();
    mutable double** cachedResult;
    mutable bool isCacheValid;
    void pushResults();
    CStateContainer* states;
    CActionContainer* actions;
    CAutomatImpl*** individs;
    CAutomatImpl*** newIndivids;
    std::vector<CMapPtr> maps;
    CLabResultMulti* result;
    CInvoker* invoker;

    int cnt;
};