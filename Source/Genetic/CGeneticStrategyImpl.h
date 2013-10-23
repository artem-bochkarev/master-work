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
        CLabResultMulti* res, const std::vector< std::string >& strings, Tools::Logger& );
    
    virtual void run();

    virtual void nextGeneration( CRandom* rand );

    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();

    //virtual void addMap( CMapPtr map );
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
    CAutomatPtr getBestIndivid() const;
    virtual ~CGeneticStrategyImpl();
    virtual CInvoker* getInvoker() const;
    virtual size_t getN() const;
    virtual size_t getM() const;
    virtual std::string getDeviceType() const;
    virtual const boost::exception_ptr& getError() const;
protected:
    void pushResults();
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