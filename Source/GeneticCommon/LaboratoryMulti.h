#pragma once
#include "GeneticAPI/CLaboratory.h"
#include "GeneticCommon/LabResultMulti.hpp"
#include "CleverAntStrategy.h"
//#include "CAntFitnes.h"

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE = double>
class CLaboratoryMulti : public CLaboratory<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CLaboratoryMulti(CAntCommonPtr<COUNTERS_TYPE> antCommonPtr,
		CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE> strategy, 
		CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResult);
    
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

	CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>* getFitnesFunctor();
	const CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>* getFitnesFunctor() const;

    virtual ~CLaboratoryMulti();
private:
 //   CLaboratoryMulti();
    mutable CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> results;
	CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> strategy;

    //CActionContainerPtr actions;
	CAntCommonPtr<COUNTERS_TYPE> antCommonPtr;
};

template<typename C, typename I, typename F = double, typename K = CLaboratoryMulti<C, I, F> >
using CLaboratoryMultiPtr = boost::shared_ptr< K >;