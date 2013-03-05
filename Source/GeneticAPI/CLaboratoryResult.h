#pragma once
#include "CLaboratory.h"
#include "TimeRun/CTimeResult.h"

class CLaboratoryResult : public CTimeResult
{
public:
    virtual size_t getRunCount() const = 0;

    virtual size_t getGenerationsNumber() const = 0;
    virtual CAutomatPtr getBestInd( size_t i ) const = 0;
    virtual CAutomatPtr getLastInd() const = 0;
    virtual double getMaxFitnes( size_t i ) const = 0;
    virtual double getAvgFitnes( size_t i ) const = 0;
};

typedef boost::shared_ptr< CLaboratoryResult > CLaboratoryResultPtr;