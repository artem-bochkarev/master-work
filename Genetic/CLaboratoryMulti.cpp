#include "stdafx.h"
#include "CLaboratoryMulti.h"
#include "CGeneticStrategyImpl.h"
#include <boost/date_time/posix_time/posix_time.hpp>

double CLaboratoryMulti::getAvgFitness( size_t i ) const
{
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getAvgFitnes( i );
}

CAutomatPtr CLaboratoryMulti::getBestInd( size_t i ) const
{
    boost::mutex& mutex = results->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return results->getBestInd( i );
}

size_t CLaboratoryMulti::getGenerationNumber() const
{
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

void CLaboratoryMulti::start()
{
    if ( !running  )
    {
        running = true;
        invoker = strategy->getInvoker( );
        //boost::thread thrd( *invoker );
        //pThread = boost::move(thrd);
        pThread = invoker->getThread();
    }
}

void CLaboratoryMulti::pause()
{
    if ( pThread.get() != 0 )
    {
        pThread->interrupt();
//        pThread->join();
        running = false;
        pThread->detach();
        pThread.reset();
    }
}

void CLaboratoryMulti::runForTime( int milisec )
{
    start();
    boost::this_thread::sleep( boost::posix_time::millisec( milisec ) );
    pause();
}

bool CLaboratoryMulti::isRunning() const
{
    return running;
}

CLaboratoryMulti::CLaboratoryMulti( CStateContainerPtr states, CActionContainerPtr actions, 
                                   CGeneticStrategyPtr strategy, CLabResultMultiPtr labResult )
:invoker(0), running(false), states(states), actions(actions), strategy(strategy), 
    results(labResult)
{
    //strategy = static_cast<CGeneticStrategy*>(new CGeneticStrategyImpl( states, actions, &results ));
    //strategy = static_cast<CGeneticStrategy*>(new CGeneticStrategyCL( states, actions, &results ));
}

CLaboratoryMulti::~CLaboratoryMulti()
{
    
}

void CLaboratoryMulti::setMaps( std::vector<CMapPtr> maps )
{
    strategy->setMaps( maps );
}

const CMapPtr CLaboratoryMulti::getMap( size_t i )
{
    return strategy->getMap( i );
}
size_t CLaboratoryMulti::getMapsCount()
{
    return strategy->getMapsCount();
}

const CGeneticStrategyPtr CLaboratoryMulti::getStrategy() const
{
    return strategy;
}

const CStateContainerPtr CLaboratoryMulti::getStates() const
{
    return states;
}

const CActionContainerPtr CLaboratoryMulti::getActions() const
{
    return actions;
}