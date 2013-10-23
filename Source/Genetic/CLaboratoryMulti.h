#pragma once
#include "../GeneticAPI/CLaboratory.h"
#include "CLabResultMulti.h"
#include "../GeneticAPI/CGeneticAlgorithm.h"
#include "CAntFitnes.h"

class CLaboratoryMulti : public CLaboratory
{
public:
    CLaboratoryMulti( CStateContainerPtr states, CActionContainerPtr actions, 
        CGeneticAlgorithmPtr strategy, CLabResultMultiPtr labResult );
    virtual void setMaps( std::vector<CMapPtr> maps );
    
    /*virtual void start();
    virtual void runForTime( int milisec );
    virtual void pause();
    virtual bool isRunning() const;*/
    
    virtual size_t getGenerationNumber() const;
    virtual double getMaxFitness( size_t i ) const;
    virtual double getAvgFitness( size_t i ) const;
    virtual CAutomatPtr getBestInd( size_t i ) const;
    virtual const CGeneticAlgorithmPtr getStrategy() const;

    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();

    virtual const CStateContainerPtr getStates() const;
    virtual const CActionContainerPtr getActions() const;

    virtual ~CLaboratoryMulti();
private:
    CLaboratoryMulti();
    mutable CLabResultMultiPtr results;
    CGeneticAlgorithmPtr strategy;
    CAntFitnesPtr fitnesFunctor;

    CStateContainerPtr states;
    CActionContainerPtr actions;
};

typedef boost::shared_ptr<CLaboratoryMulti> CLaboratoryMultiPtr;