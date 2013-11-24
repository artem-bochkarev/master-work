#pragma once
#include "../GeneticAPI/CLaboratoryResult.h"
#include "GeneticParams.h"
#include "CLabResultMulti.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>


class CLabResultMulti : public CLaboratoryResult<COUNTERS_TYPE, INPUT_TYPE>
{
public:
    friend class CGeneticStrategyCommon;
    friend class CLaboratoryMulti;
    //friend class CGeneticStrategyImpl;
    friend class CGeneticStrategyCL;
    friend class CGeneticStrategyCLv2;
	friend class CGeneticStrategyCLWrap;
    CLabResultMulti();
    virtual size_t getGenerationsNumber() const;
    virtual CAutomat<COUNTERS_TYPE, INPUT_TYPE>* getBestInd( size_t i ) const;
	virtual CAutomat<COUNTERS_TYPE, INPUT_TYPE>* getLastInd() const;
    virtual double getMaxFitnes( size_t i ) const;
    virtual double getAvgFitnes( size_t i ) const;
    virtual size_t getRunCount() const;
protected:
    virtual void addGeneration( const CAutomatPtr ind, double maxF, double avgF );
    //boost::mutex& getMutex();
private:
    boost::mutex mutex;
    size_t genCnt;
    std::vector<CAutomatPtr> individs;
    std::vector<double> avgFitnes;
    std::vector<double> maxFitnes;
};

typedef boost::shared_ptr<CLabResultMulti> CLabResultMultiPtr;