#pragma once
#include "CAutomat.h"
#include "CGeneticAlgorithm.h"
#include "TimeRun/CTimeRunner.h"

template< typename NUMBERS_TYPE, typename INPUT_TYPE > class CLaboratory : public CTimeRunner
{
public:
    virtual size_t getGenerationNumber() const = 0;
    virtual double getMaxFitness( size_t i ) const = 0;
    virtual double getAvgFitness( size_t i ) const = 0;
	virtual CAutomat<NUMBERS_TYPE, INPUT_TYPE>* getBestInd(size_t i) const = 0;
    
	virtual const CGeneticAlgorithm<NUMBERS_TYPE, INPUT_TYPE>* getStrategy() const = 0;
	virtual const CStateContainer<NUMBERS_TYPE>* getStates() const = 0;
	virtual const CActionContainer<NUMBERS_TYPE>* getActions() const = 0;

	virtual void writeInfo(std::ostream&) const
	{
		ost << getStrategy()->getDeviceType() <<
			", states count: " << getStates()->size() << ", " << getStrategy()->getPoolInfo() << std::endl;
	}
};