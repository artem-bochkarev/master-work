#include "CleverAnt3Fitnes.h"
#include "CleverAnt3Map.h"
#include "GeneticCommon/Test.hpp"
#include "GeneticCommon/RandomImpl.h"
#include <boost/spirit/include/qi.hpp>
#include "performanceInfo.h"
#include "Tools/SuperQueue.h"

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

CCleverAnt3FitnesCPU::CCleverAnt3FitnesCPU(const std::vector< std::string >& strings)
{
	using namespace boost::spirit::qi;
	for (size_t i = 0; i < strings.size(); ++i)
	{
		const std::string& str = strings[i];
		if (phrase_parse(str.begin(), str.end(), "STEPS_COUNT=" >> int_ >> ";", space, m_steps))
			continue;
	}
}

ANT_FITNES_TYPE CCleverAnt3FitnesCPU::fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const
{
	CMapPtr pMap(new CleverAnt3Map(maps[0]));
	return CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>::run(automat, pMap.get(), m_steps);
}

template<class AUTOMAT_TYPE>
class CLocalInvoker
{
	const std::vector<AUTOMAT>& m_individs;
	std::vector<ANT_FITNES_TYPE>& m_result;
	size_t size;
	SuperQueue& queue;
	boost::barrier& barrier;
	const CCleverAnt3FitnesCPU* m_fitnesFunc;
public:
	CLocalInvoker(const CCleverAnt3FitnesCPU* fitnesFunc, const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result, SuperQueue& queue, size_t size, boost::barrier& barrier)
		:m_fitnesFunc(fitnesFunc), m_individs(individs), m_result(result), size(size), queue(queue),
		barrier(barrier) {}

	void operator ()() const
	{
		boost::this_thread::disable_interruption di;
		uint32_t i = queue.getNext();
		while (i < size)
		{
			m_result[i] = m_fitnesFunc->fitnes(&m_individs[i]);
			i = queue.getNext();
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

void CCleverAnt3FitnesCPU::fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result)
{
	CTimeCounter counter(perfFitnesFunctionCPU);
	boost::thread_group group;
	size_t N = individs.size();
	unsigned int nthreads = boost::thread::hardware_concurrency();

	boost::barrier firstBarrier(nthreads + 1);
	boost::this_thread::disable_interruption di;
	SuperQueue queue;
	for (size_t i = 0; i<nthreads; ++i)
	{
		CLocalInvoker<AUTOMAT> invoker(this, individs, result, queue, individs.size(),
			firstBarrier);
		group.create_thread(invoker);
	}

	firstBarrier.wait();
	group.interrupt_all();
	group.join_all();
}

std::string CCleverAnt3FitnesCPU::getInfo() const
{
	return "C++ on CPU";
}