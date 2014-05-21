#include "GeneticStrategyImpl.h"
#include "GeneticCommon/CleverAntStrategy.hpp"
#include "GeneticCommon/RandomImpl.h"
#include <boost/spirit/include/qi.hpp>
#include "performanceInfo.h"

CGeneticStrategyImpl::CGeneticStrategyImpl(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* res,
	CCleverAnt3FitnesPtr fitnes,
	const std::vector< std::string >& strings, Tools::Logger& logger)
	:CGeneticStrategyCommon(pAntCommon, res, fitnes, strings, logger), m_pRandom(new CRandomImpl())
{
	m_pResults = res;
	fitnesFunctor = fitnes;
	invoker = new CInvoker(this, error);

	generation_size = 128;
	for (size_t i = 0; i < strings.size(); ++i)
	{
		using namespace boost::spirit::qi;
		const std::string& str = strings[i];
		if (phrase_parse(str.begin(), str.end(), "GENERATION_SIZE=" >> int_ >> ";", space, generation_size))
			continue;
	}

	try
	{
		M = 2;
		N = generation_size;
		individuals.resize(N);
		for (int i = 0; i < N; ++i)
		{
			AUTOMAT aut(pAntCommon.get());
			aut.generateRandom(m_pRandom.get());
			individuals[i] = aut;
		}
	}
	catch (std::runtime_error& err)
	{
		std::cout << "[WARNING] CGeneticStrategyImpl::Init<>: arr size =" << N*sizeof(AUTOMAT)/(1024*1024) << "MB(" << N << ")" << err.what() << std::endl;
		logger << "[FAILED] CGeneticStrategyImpl::Init<>: " << N*sizeof(AUTOMAT) / (1024 * 1024) << "MB(" << N << ")" << err.what() << "\n";
		throw err;
	}
	catch (std::exception& err)
	{
		std::cout << "[WARNING] CGeneticStrategyImpl::Init<>: arr size =" << N*sizeof(AUTOMAT) / (1024 * 1024) << "MB(" << N << ")" << err.what() << std::endl;
		logger << "[FAILED] CGeneticStrategyImpl::Init<>: " << N*sizeof(AUTOMAT) / (1024 * 1024) << "MB(" << N << ")" << err.what() << "\n";
		throw err;
	}
}

void CGeneticStrategyImpl::run()
{
	nextGeneration(m_pRandom.get());
}

void CGeneticStrategyImpl::nextGeneration(CRandom* rand)
{
	CTimeCounter counter(perfNextGeneration);
	std::vector<ANT_FITNES_TYPE> results(individuals.size());
	fitnesFunctor->fitnes(individuals, results);

	CTimeCounter counter2(perfNextGenerationNF);
	ANT_FITNES_TYPE avg = 0;
	uint k = 0;
	std::vector<std::pair<ANT_FITNES_TYPE, int>> sorted(individuals.size());
	for (uint i = 0; i < individuals.size(); ++i)
	{
		sorted[i] = std::pair<ANT_FITNES_TYPE, int>(results[i], i);
		avg += results[i];
		if (results[i] > results[k])
			k = i;
	}
	avg /= individuals.size();
	ANT_FITNES_TYPE max = results[k];
	m_pResults->addGeneration(CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE>(new AUTOMAT(individuals[k])), max, avg);
	std::sort(sorted.begin(), sorted.end());

	std::vector<AUTOMAT> tournamentWinners(N);
	for (int i = 0; i < N; ++i)
	{
		int k = -1;
		for (int j = 0; j < M; ++j)
		{
			uint a = rand->nextUINT()%N;
			if (k == -1)
				k = a;
			else
			{
				if (results[a] > results[k])
					k = a;
			}
		}
		tournamentWinners[i] = individuals[k];
	}

	std::vector<AUTOMAT> newGeneration(individuals.size());
	float SPLIT_KOEFF = 0.3f;
	float ELITE_KOEFF = 0.05f;
	size_t z = static_cast<size_t>(individuals.size() * (1 - SPLIT_KOEFF - ELITE_KOEFF));
	for (size_t i = 0; i < z; ++i)
	{
		int a = rand->nextUINT() % N;
		int b = rand->nextUINT() % N;
		newGeneration[i].crossover(&tournamentWinners[a], &tournamentWinners[b], rand);
		//newGeneration[i] = tournamentWinners[a];
		//newGeneration[i].mutate(rand);
	}
	size_t z1 = static_cast<size_t>(individuals.size() * (1 - ELITE_KOEFF));
	for (size_t i = z; i < z1; ++i)
	{
		int a = rand->nextUINT() % N;
		newGeneration[i] = tournamentWinners[a];
	}

	for (size_t i = z1; i < individuals.size(); ++i)
	{
		int a = sorted[i].second;
		newGeneration[i] = individuals[a];
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
	out << "Size of generation = " << generation_size << ", Steps on map = " << fitnesFunctor->steps();
	return out.str();
}

const boost::exception_ptr& CGeneticStrategyImpl::getError() const
{
	return error;
}
CGeneticStrategyImpl::~CGeneticStrategyImpl() {}

std::string CGeneticStrategyImpl::getDeviceType() const
{
	std::stringstream out;
	out << "Clever ant 3 standard, " << fitnesFunctor->getInfo();
	return out.str();
}

