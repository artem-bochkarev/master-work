#pragma once
#include "../GeneticAPI/CLaboratory.h"
#include "CLabResultMulti.h"
#include "CGeneticStrategyCommon.h"
#include "CAntFitnes.h"

class CLaboratoryMulti : public CLaboratory
{
public:
    CLaboratoryMulti( CStateContainerPtr states, CActionContainerPtr actions, 
        CGeneticStrategyCommonPtr strategy, CLabResultMultiPtr labResult );
    
    /*virtual void start();
    virtual void runForTime( int milisec );
    virtual void pause();
    virtual bool isRunning() const;*/
    
    virtual size_t getGenerationNumber() const;
    virtual double getMaxFitness( size_t i ) const;
    virtual double getAvgFitness( size_t i ) const;
    virtual CAutomatPtr getBestInd( size_t i ) const;
    virtual const CGeneticAlgorithmPtr getStrategy() const;

    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();

    virtual const CStateContainerPtr getStates() const;
    virtual const CActionContainerPtr getActions() const;

    CAntFitnes* getFitnesFunctor();
    const CAntFitnes* getFitnesFunctor() const;

    virtual ~CLaboratoryMulti();
private:
    CLaboratoryMulti();
    mutable CLabResultMultiPtr results;
    CGeneticStrategyCommonPtr strategy;

    CStateContainerPtr states;
    CActionContainerPtr actions;
};

typedef boost::shared_ptr<CLaboratoryMulti> CLaboratoryMultiPtr;