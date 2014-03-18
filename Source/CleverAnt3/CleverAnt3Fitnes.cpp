#include "CleverAnt3Fitnes.h"
#include "CleverAnt3Map.h"
#include "GeneticCommon/Test.hpp"
#include "GeneticCommon/RandomImpl.h"

void CCleverAnt3Fitnes::setMaps(std::vector<CMapPtr> maps)
{
	this->maps = maps;
}

const CMapPtr CCleverAnt3Fitnes::getMap(size_t i)
{
	CMapPtr pMap(new CleverAnt3Map(maps[i]));
	return maps[i];
}

size_t CCleverAnt3Fitnes::getMapsCount()
{
	return maps.size();
}

size_t CCleverAnt3Fitnes::steps() const
{
	return m_steps;
}

CCleverAnt3FitnesCPU::CCleverAnt3FitnesCPU(size_t stepsCount)
{
	m_steps = stepsCount;
}

ANT_FITNES_TYPE CCleverAnt3FitnesCPU::fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const
{
	CMapPtr pMap(new CleverAnt3Map(maps[0]));
	return CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>::run(automat, pMap.get(), DEFAULT_STEPS_COUNT);
}

template<class AUTOMAT_TYPE>
class CLocalInvoker
{
	const std::vector<AUTOMAT>& m_individs;
	std::vector<ANT_FITNES_TYPE>& m_result;
	size_t start, finish;
	boost::barrier& barrier;
	const CCleverAnt3FitnesCPU* m_fitnesFunc;
	mutable CRandomImpl rand;
public:
	CLocalInvoker( const CCleverAnt3FitnesCPU* fitnesFunc, const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result, size_t start,
		size_t finish, boost::barrier& barrier)
		:m_fitnesFunc(fitnesFunc), m_individs(individs), m_result(result), start(start), finish(finish),
		barrier(barrier), rand(std::clock()) {}

	void operator ()() const
	{
		boost::this_thread::disable_interruption di;
		for (size_t i = start; i < finish; ++i)
		{
			m_result[i] = m_fitnesFunc->fitnes(&m_individs[i]);
		}
		barrier.wait();
		boost::this_thread::interruption_point();
	}

	virtual threadPtr getThread() const
	{
		return threadPtr(new boost::thread(*this));
	}
private:
	CLocalInvoker();
	CLocalInvoker& operator=(const CLocalInvoker&);
};

void CCleverAnt3FitnesCPU::fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result) const
{
	/*for (size_t i = 0; i < individs.size(); ++i)
	{
		result[i] = fitnes(&individs[i]);
	}*/

	boost::thread_group group;
	size_t N = individs.size();
	size_t cnt = 3;
	size_t step = std::max((size_t)1, N / cnt);
	
	size_t prev = 0;
	size_t next = prev + step;

	boost::barrier firstBarrier(cnt + 1);
	boost::this_thread::disable_interruption di;
	for (size_t i = 0; i<cnt; ++i)
	{
		if (i == cnt - 1)
			next = N;
		CLocalInvoker<AUTOMAT> invoker(this, individs, result, prev, next,
			firstBarrier);
		group.create_thread(invoker);
		prev = next;
		next += step;
	}

	firstBarrier.wait();
	group.interrupt_all();
	group.join_all();
}