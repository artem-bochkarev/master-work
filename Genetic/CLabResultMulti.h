#pragma once
#include "../GeneticAPI/CLaboratoryResult.h"
#include "CLabResultMulti.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

class CLabResultMulti : public CLaboratoryResult
{
public:
    friend class CLaboratoryMulti;
    friend class CGeneticStrategyImpl;
    friend class CGeneticStrategyCL;
	friend class CGeneticStrategyCLWrap
    CLabResultMulti();
    virtual size_t getGenerationsNumber() const;
    virtual CAutomatPtr getBestInd( size_t i ) const;
    virtual CAutomatPtr getLastInd() const;
    virtual double getMaxFitnes( size_t i ) const;
    virtual double getAvgFitnes( size_t i ) const;
protected:
    virtual void addGeneration( const CAutomatPtr ind, double maxF, double avgF );
    boost::mutex& getMutex();
private:
    boost::mutex mutex;
    size_t genCnt;
    std::vector<CAutomatPtr> individs;
    std::vector<double> avgFitnes;
    std::vector<double> maxFitnes;
};

typedef boost::shared_ptr<CLabResultMulti> CLabResultMultiPtr;