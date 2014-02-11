#include "GeneticStrategyImpl.h"
#include "GeneticCommon/CleverAntStrategy.hpp"
#include "GeneticCommon/RandomImpl.h"

CGeneticStrategyImpl::CGeneticStrategyImpl(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* res,
	CCleverAnt3FitnesPtr fitnes,
	const std::vector< std::string >& strings, Tools::Logger& logger)
	:CGeneticStrategyCommon(pAntCommon, res, fitnes, strings, logger), m_pRandom(new CRandomImpl())
{
	m_pResults = res;
	fitnesFunctor = fitnes;
	invoker = new CInvoker(this, error);
	N = 100;
	M = 2;
	for (int i = 0; i < N; ++i)
	{
		AUTOMAT aut(pAntCommon.get());
		aut.generateRandom(m_pRandom.get());
		individuals.push_back(aut);
	}
}

void CGeneticStrategyImpl::run()
{
	nextGeneration(m_pRandom.get());
}

void CGeneticStrategyImpl::nextGeneration(CRandom* rand)
{
	std::vector<ANT_FITNES_TYPE> results(individuals.size());
	fitnesFunctor->fitnes(individuals, results);

	ANT_FITNES_TYPE avg = 0;
	uint k = 0;
	for (uint i = 0; i < individuals.size(); ++i)
	{
		avg += results[i];
		if (results[i] > results[k])
			k = i;
	}
	avg /= individuals.size();
	ANT_FITNES_TYPE max = results[k];
	m_pResults->addGeneration(CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE>(new AUTOMAT(individuals[k])), max, avg);

	std::vector<AUTOMAT> newGeneration(individuals.size());
	for (int i = 0; i < N; ++i)
	{
		int k = -1;
		for (int j = 0; j < M; ++j)
		{
			uint a = rand->nextUINT()%individuals.size();
			if (k == -1)
				k = a;
			else
			{
				if (results[a] > results[k])
					k = a;
			}
		}
		newGeneration[i] = individuals[k];
	}
	individuals.swap(newGeneration);
}

//void CGeneticStrategyImpl::setMaps(std::vector<CMapPtr> maps);
//const CMapPtr CGeneticStrategyImpl::getMap(size_t i);
//size_t getMapsCount();
ANT_FITNES_TYPE CGeneticStrategyImpl::getAvgFitness() const
{
	return m_avg;
}

ANT_FITNES_TYPE CGeneticStrategyImpl::getMaxFitness() const
{
	return m_max;
}

CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> CGeneticStrategyImpl::getBestIndivid() const
{
	return CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE>(&m_best);
}

/*void CGeneticStrategyImpl::pushResults()
{
	boost::mutex& mutex = result->getMutex();
	boost::mutex::scoped_lock lock(mutex);
	result->addGeneration(getBestIndivid(), getMaxFitness(), getAvgFitness());
}*/


CInvoker* CGeneticStrategyImpl::getInvoker() const
{
	return invoker;
}

std::string CGeneticStrategyImpl::getPoolInfo() const
{
	std::stringstream out;
	out << "Size: " << getN() << "x" << getM();
	return out.str();
}

const boost::exception_ptr& CGeneticStrategyImpl::getError() const
{
	return error;
}
CGeneticStrategyImpl::~CGeneticStrategyImpl() {}

std::string CGeneticStrategyImpl::getDeviceType() const
{
	return "Clever ant 3 standard";
}

