#pragma once
#include "CAutomat.h"
#include "CMap.h"
#include "CGeneticStrategy.h"

class CLaboratory
{
public:
    virtual void start() = 0;
    virtual void runForTime( int milisec ) = 0;
    virtual void pause() = 0;
    virtual size_t getGenerationNumber() const = 0;
    virtual double getMaxFitness( size_t i ) const = 0;
    virtual double getAvgFitness( size_t i ) const = 0;
    virtual CAutomatPtr getBestInd( size_t i ) const = 0;
    virtual void setMaps( std::vector<CMapPtr> maps ) = 0;
    virtual const CMapPtr getMap( size_t i ) = 0;
    virtual size_t getMapsCount() = 0;
    virtual bool isRunning() const = 0;
    virtual const CGeneticStrategyPtr getStrategy() const = 0;
    virtual const CStateContainerPtr getStates() const = 0;
    virtual const CActionContainerPtr getActions() const = 0;
};

typedef boost::shared_ptr<CLaboratory> CLaboratoryPtr;