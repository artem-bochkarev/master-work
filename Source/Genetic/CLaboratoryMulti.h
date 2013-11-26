#pragma once
#include "GeneticParams.h"
#include "GeneticAPI/CLaboratory.h"
#include "CLabResultMulti.h"
#include "CGeneticStrategyCommon.h"
#include "CAntFitnes.h"

class CLaboratoryMulti : public CLaboratory<COUNTERS_TYPE, INPUT_TYPE>
{
public:
    CLaboratoryMulti(AntCommonPtr antCommonPtr, 
        CGeneticStrategyCommonPtr strategy, CLabResultMultiPtr labResult );
    
    /*virtual void start();
    virtual void runForTime( int milisec );
    virtual void pause();
    virtual bool isRunning() const;*/
    
    virtual size_t getGenerationNumber() const;
    virtual double getMaxFitness( size_t i ) const;
    virtual double getAvgFitness( size_t i ) const;
	virtual CAutomat<COUNTERS_TYPE, INPUT_TYPE>* getBestInd(size_t i) const;
	virtual const CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE>* getStrategy() const;

    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();

	virtual const CActionContainer<COUNTERS_TYPE>* getActions() const;

    CAntFitnes* getFitnesFunctor();
    const CAntFitnes* getFitnesFunctor() const;

    virtual ~CLaboratoryMulti();
private:
    CLaboratoryMulti();
    mutable CLabResultMultiPtr results;
    CGeneticStrategyCommonPtr strategy;

    //CActionContainerPtr actions;
	AntCommonPtr antCommonPtr;
};

typedef boost::shared_ptr<CLaboratoryMulti> CLaboratoryMultiPtr;