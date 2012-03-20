#pragma once
#include "../GeneticAPI/CLaboratory.h"
#include "CLabResultMulti.h"
#include "../GeneticAPI/CGeneticStrategy.h"
#include "../GeneticAPI/CInvoker.h"

class CLaboratoryMulti : public CLaboratory
{
public:
    CLaboratoryMulti( CStateContainerPtr states, CActionContainerPtr actions, 
        CGeneticStrategyPtr strategy, CLabResultMultiPtr labResult );
    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual void start();
    virtual void runForTime( int milisec );
    virtual void pause();
    virtual bool isRunning() const;
    virtual size_t getGenerationNumber() const;
    virtual double getMaxFitness( size_t i ) const;
    virtual double getAvgFitness( size_t i ) const;
    virtual CAutomatPtr getBestInd( size_t i ) const;
    virtual const CGeneticStrategyPtr getStrategy() const;

    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();

    virtual const CStateContainerPtr getStates() const;
    virtual const CActionContainerPtr getActions() const;

    virtual ~CLaboratoryMulti();
private:
    CLaboratoryMulti();
    mutable CLabResultMultiPtr results;
    CGeneticStrategyPtr strategy;
    CInvoker* invoker;
    threadPtr pThread;
    bool running;

    CStateContainerPtr states;
    CActionContainerPtr actions;
};