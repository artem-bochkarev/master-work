#include "stdafx.h"
#include "CLaboratoryMulti.h"
#include "CGeneticStrategyImpl.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/chrono.hpp>

const long sleep_time = 100; //millisec

double CLaboratoryMulti::getAvgFitness( size_t i ) const
{
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getAvgFitnes( i );
}

CAutomat<COUNTERS_TYPE, INPUT_TYPE>* CLaboratoryMulti::getBestInd(size_t i) const
{
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getBestInd( i );
}

size_t CLaboratoryMulti::getGenerationNumber() const
{
    if ( strategy->getError() )
        boost::rethrow_exception(strategy->getError());
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getGenerationsNumber();
}

double CLaboratoryMulti::getMaxFitness( size_t i ) const
{
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getMaxFitnes( i );
}

CLaboratoryMulti::CLaboratoryMulti( CStateContainerPtr states, CActionContainerPtr actions, 
                                   CGeneticStrategyCommonPtr strategy, CLabResultMultiPtr labResult )
:states(states), actions(actions), strategy(strategy), 
    results(labResult)
{
    m_pTask = strategy;
    //strategy = static_cast<CGeneticAlgorithm*>(new CGeneticStrategyImpl( states, actions, &results ));
    //strategy = static_cast<CGeneticAlgorithm*>(new CGeneticStrategyCL( states, actions, &results ));
}

CLaboratoryMulti::~CLaboratoryMulti()
{
    
}

void CLaboratoryMulti::setMaps( std::vector<CMapPtr> maps )
{
	strategy->setMaps(maps);
}

const CMapPtr CLaboratoryMulti::getMap( size_t i )
{
    return getFitnesFunctor()->getMap( i );
}

size_t CLaboratoryMulti::getMapsCount()
{
    return getFitnesFunctor()->getMapsCount();
}

const CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE>* CLaboratoryMulti::getStrategy() const
{
    return strategy.get();
}

const CStateContainer<COUNTERS_TYPE>* CLaboratoryMulti::getStates() const
{
    return states.get();
}

const CActionContainer<COUNTERS_TYPE>* CLaboratoryMulti::getActions() const
{
    return actions.get();
}

CAntFitnes* CLaboratoryMulti::getFitnesFunctor()
{
    return strategy->getFitnesFunctor();
}

const CAntFitnes* CLaboratoryMulti::getFitnesFunctor() const
{
    return strategy->getFitnesFunctor();
}

/*void CLaboratoryMulti::getFitnesFunctor()
{
    fitnesFunctor = fitnes;
}*/