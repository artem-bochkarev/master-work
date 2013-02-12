#include "stdafx.h"
#include "CGeneticStrategyCommon.h"
#include "CMapImpl.h"

CGeneticStrategyCommon::CGeneticStrategyCommon(CStateContainer *states, CActionContainer *actions, 
                        CLabResultMulti *res, const std::vector<std::string> &strings, Tools::Logger &logger)
                        :logger(logger),states(states), actions(actions), N(0), M(0), result(res)
{}

void CGeneticStrategyCommon::setMaps(std::vector<CMapPtr> maps)
{
    this->maps = maps;
}

const CMapPtr CGeneticStrategyCommon::getMap( size_t i )
{
    CMapImpl* map = new CMapImpl( maps.at(i).get() );
    return CMapPtr( map );
}

size_t CGeneticStrategyCommon::getMapsCount()
{
    return maps.size();
}

void CGeneticStrategyCommon::pushResults()
{
    boost::mutex& mutex = result->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    result->addGeneration( getBestIndivid(), getMaxFitness(), getAvgFitness() );
}

double CGeneticStrategyCommon::getAvgFitness() const
{
    return 0.0;
}
double CGeneticStrategyCommon::getMaxFitness() const
{
    return 0.0;
}
CAutomatPtr CGeneticStrategyCommon::getBestIndivid() const
{
    return CAutomatPtr();
}

CInvoker* CGeneticStrategyCommon::getInvoker() const
{
    return invoker;
}

size_t CGeneticStrategyCommon::getN() const
{
    return N;
}

size_t CGeneticStrategyCommon::getM() const
{
    return M;
}

const boost::exception_ptr& CGeneticStrategyCommon::getError() const
{
    return error;
}