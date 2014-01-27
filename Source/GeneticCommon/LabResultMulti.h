#pragma once
#include "../GeneticAPI/CLaboratoryResult.h"
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>

template<typename COUNTERS_TYPE, typename INPUT_TYPE>
class CLabResultMulti : public CLaboratoryResult<COUNTERS_TYPE, INPUT_TYPE>
{
public:
    CLabResultMulti();
    virtual size_t getGenerationsNumber() const;
    virtual CAutomat<COUNTERS_TYPE, INPUT_TYPE>* getBestInd( size_t i ) const;
	virtual CAutomat<COUNTERS_TYPE, INPUT_TYPE>* getLastInd() const;
    virtual double getMaxFitnes( size_t i ) const;
    virtual double getAvgFitnes( size_t i ) const;
    virtual size_t getRunCount() const;

	virtual void addGeneration( const CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> ind, double maxF, double avgF );
    //boost::mutex& getMutex();
private:
    boost::mutex mutex;
    size_t genCnt;
	std::vector< CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> > individs;
    std::vector<double> avgFitnes;
    std::vector<double> maxFitnes;
};

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename C = CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE> >
using CLabResultMultiPtr = boost::shared_ptr<C>;