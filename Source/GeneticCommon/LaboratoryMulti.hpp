#pragma once
#include "LaboratoryMulti.h"
//#include "CGeneticStrategyImpl.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/chrono.hpp>
#include "TimeRun/CTimeCounter.h"

const long sleep_time = 100; //millisec

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
double CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getAvgFitness( size_t i ) const
{
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getAvgFitnes( i );
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CAutomat<COUNTERS_TYPE, INPUT_TYPE>* CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getBestInd(size_t i) const
{
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getBestInd( i );
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
size_t CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getGenerationNumber() const
{
    if ( strategy->getError() )
        boost::rethrow_exception(strategy->getError());
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getGenerationsNumber();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
double CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getMaxFitness(size_t i) const
{
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getMaxFitnes( i );
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::CLaboratoryMulti(CAntCommonPtr<COUNTERS_TYPE> antCommonPtr,
	CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE> strategy, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResult)
	:antCommonPtr(antCommonPtr), strategy(strategy), results(labResult)
{
    m_pTask = strategy;
	m_pTimeResult = results;
    //strategy = static_cast<CGeneticAlgorithm*>(new CGeneticStrategyImpl( states, actions, &results ));
    //strategy = static_cast<CGeneticAlgorithm*>(new CGeneticStrategyCL( states, actions, &results ));
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::~CLaboratoryMulti()
{
    
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
void CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::setMaps(std::vector<CMapPtr> maps)
{
	strategy->setMaps(maps);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
const CMapPtr CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getMap(size_t i)
{
    return getFitnesFunctor()->getMap( i );
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
size_t CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getMapsCount()
{
    return getFitnesFunctor()->getMapsCount();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
const CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE>* CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getStrategy() const
{
    return strategy.get();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
const CActionContainer<COUNTERS_TYPE>* CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getActions() const
{
    return antCommonPtr->actions();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>* CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getFitnesFunctor()
{
    return strategy->getFitnesFunctor();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
const CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>* CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getFitnesFunctor() const
{
    return strategy->getFitnesFunctor();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
void CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::writeResult(std::ostream& out) const
{
	if (m_pTimeResult.get() != 0)
		out << "Result: " << m_pTimeResult->getRunCount() << std::endl;
	else
	if (m_pInvoker != 0)
		out << "Result: " << m_pInvoker->getLoopCounter() << std::endl;
	else
		out << "No Invoker - No Result: " << std::endl;
		
	if (GetTimeManager().getTimers().size() > 0)
	{
		bool bUseSeconds = false;
		for (std::map<std::string, TimerData>::value_type val : GetTimeManager().getTimers())
		{
			if (boost::chrono::duration_cast<boost::chrono::milliseconds>(val.second.duration) >= boost::chrono::milliseconds(10000))
			{
				bUseSeconds = true;
				break;
			}
		}
		for (std::map<std::string, TimerData>::value_type val : GetTimeManager().getTimers())
		{
			boost::chrono::microseconds mcs = boost::chrono::duration_cast<boost::chrono::microseconds>(val.second.duration);
			double speed = (double)val.second.counter / mcs.count();
			speed *= 1000000;
			int a = static_cast<int>(speed);
			int b = static_cast<int>(speed * 1000 - a * 1000);
			if (!bUseSeconds)
			{
				out << val.first << ": " << boost::chrono::duration_cast<boost::chrono::milliseconds>(val.second.duration) << ", Speed=" << a << "." << b << std::endl;
			}
			else
			{
				out << val.first << ": " << boost::chrono::duration_cast<boost::chrono::seconds>(val.second.duration) << ", Speed=" << a << "." << b << std::endl;
			}
		}

	}
}

/*void CLaboratoryMulti::getFitnesFunctor()
{
    fitnesFunctor = fitnes;
}*/