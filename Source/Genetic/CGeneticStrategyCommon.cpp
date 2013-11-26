#include "stdafx.h"
#include "CGeneticStrategyCommon.h"
#include "CMapImpl.h"
#include "CAntFitnes.h"
#include <sstream>

CGeneticStrategyCommon::CGeneticStrategyCommon(AntCommonPtr pAntCommon, CLabResultMulti *res, CAntFitnesPtr fitnes, 
	const std::vector<std::string> &strings, Tools::Logger &logger)
                        :logger(logger), pAntCommon(pAntCommon), N(0), M(0), result(res), fitnesFunctor(fitnes)
{}

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

std::string CGeneticStrategyCommon::getPoolInfo() const
{
    std::stringstream out;
    out << "Size: " << getN() << "x" << getM();
    return out.str();
}

const boost::exception_ptr& CGeneticStrategyCommon::getError() const
{
    return error;
}

const CMapPtr CGeneticStrategyCommon::getMap( size_t i )
{
    return fitnesFunctor->getMap(i);
}

void CGeneticStrategyCommon::setMaps(std::vector<CMapPtr> maps)
{
	getFitnesFunctor()->setMaps(maps);
}

size_t CGeneticStrategyCommon::getMapsCount()
{
    return fitnesFunctor->getMapsCount();
}

CAntFitnes* CGeneticStrategyCommon::getFitnesFunctor()
{
    return fitnesFunctor.get();
}

const CAntFitnes* CGeneticStrategyCommon::getFitnesFunctor() const
{
    return fitnesFunctor.get();
}