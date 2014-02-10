#pragma once
#include "CleverAntStrategy.h"
#include "CleverAntMapImpl.h"
#include "CleverAntFitnes.hpp"
#include <sstream>

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::CGeneticStrategyCommon(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, 
	CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE> *res, CCleverAntFitnesPtr<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE> fitnes,
	const std::vector<std::string> &strings, Tools::Logger &logger)
                        :logger(logger), pAntCommon(pAntCommon), N(0), M(0), result(res), fitnesFunctor(fitnes)
{}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
void CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::pushResults()
{
    boost::mutex& mutex = result->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    result->addGeneration( getBestIndivid(), getMaxFitness(), getAvgFitness() );
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
double CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getAvgFitness() const
{
    return 0.0;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
double CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getMaxFitness() const
{
    return 0.0;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getBestIndivid() const
{
	return CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE>();
}


template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CInvoker* CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getInvoker() const
{
    return invoker;
}


template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
size_t CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getN() const
{
    return N;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
size_t CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getM() const
{
    return M;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
std::string CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getPoolInfo() const
{
    std::stringstream out;
    out << "Size: " << getN() << "x" << getM();
    return out.str();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
const boost::exception_ptr& CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getError() const
{
    return error;
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
const CMapPtr CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getMap(size_t i)
{
    return fitnesFunctor->getMap(i);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
void CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::setMaps(std::vector<CMapPtr> maps)
{
	getFitnesFunctor()->setMaps(maps);
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
size_t CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getMapsCount()
{
    return fitnesFunctor->getMapsCount();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>* CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getFitnesFunctor()
{
    return fitnesFunctor.get();
}

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE>
const CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>* CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>::getFitnesFunctor() const
{
    return fitnesFunctor.get();
}