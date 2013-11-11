#pragma once
#include "CLaboratory.h"
#include "TimeRun/CTimeResult.h"

template< typename NUMBERS_TYPE, typename INPUT_TYPE > class CLaboratoryResult : public CTimeResult
{
public:
    virtual size_t getRunCount() const = 0;

    virtual size_t getGenerationsNumber() const = 0;
	virtual CAutomat<NUMBERS_TYPE, INPUT_TYPE>* getBestInd(size_t i) const = 0;
	virtual CAutomat<NUMBERS_TYPE, INPUT_TYPE>* getLastInd() const = 0;
    virtual double getMaxFitnes( size_t i ) const = 0;
    virtual double getAvgFitnes( size_t i ) const = 0;
};