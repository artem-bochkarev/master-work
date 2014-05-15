#include "CleverAnt3FitnesCL.h"
#include "GeneticCommon/Test.hpp"
#include "CleverAnt3Map.h"
#include <boost/spirit/include/qi.hpp>
#include <boost/lexical_cast.hpp>

CCleverAnt3FitnesCLBoth::CCleverAnt3FitnesCLBoth(const std::vector< std::string >& strings, Tools::Logger& log)
{
	using namespace boost::spirit::qi;
	std::string deviceTypeStr;
	char c = 'C';
	size_t m_size = 0;
	std::vector< std::string > stringsCPU;
	std::vector< std::string > stringsGPU;
	for (size_t i = 0; i < strings.size(); ++i)
	{
		const std::string& str = strings[i];
		if (phrase_parse(str.begin(), str.end(), "GENERATION_SIZE=" >> int_ >> ";", space, m_size))
		{
			continue;
		}
		else
		{
			stringsCPU.push_back(str);
			stringsGPU.push_back(str);
		}
	}

	gpuCount = m_size / 2;
	cpuCount = m_size - gpuCount;
	stringsGPU.push_back("GENERATION_SIZE=" + boost::lexical_cast<std::string, size_t>(gpuCount)+";");
	stringsCPU.push_back("GENERATION_SIZE=" + boost::lexical_cast<std::string, size_t>(cpuCount)+";");
	clCPUFitnes = CCleverAnt3FitnesCLPtr(new CCleverAnt3FitnesCL(stringsCPU, log));
	clGPUFitnes = CCleverAnt3FitnesCLPtr(new CCleverAnt3FitnesCL(stringsGPU, log));
}

CCleverAnt3FitnesCLBoth::~CCleverAnt3FitnesCLBoth()
{

}

ANT_FITNES_TYPE CCleverAnt3FitnesCLBoth::fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const
{
	CMapPtr pMap(new CleverAnt3Map(maps[0]));
	return CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>::run(automat, pMap.get(), DEFAULT_STEPS_COUNT);
}

template<class AUTOMAT_TYPE>
class CFitnesInvoker
{
	const AUTOMAT* m_individs;
	ANT_FITNES_TYPE* m_result;
	CCleverAnt3FitnesCL* m_fitnesFunc;
	boost::barrier& barrier;
public:
	CFitnesInvoker(CCleverAnt3FitnesCL* fitnesFunc, const AUTOMAT* individs, ANT_FITNES_TYPE* result, boost::barrier& barrier)
		:m_fitnesFunc(fitnesFunc), m_individs(individs), m_result(result), barrier(barrier) {}

	void operator ()() const
	{
		boost::this_thread::disable_interruption di;
		m_fitnesFunc->fitnes(m_individs, m_result);
		barrier.wait();
		boost::this_thread::interruption_point();
	}

	virtual threadPtr getThread() const
	{
		return threadPtr(new boost::thread(*this));
	}
private:
	CFitnesInvoker();
	CFitnesInvoker& operator=(const CFitnesInvoker&);
};

void CCleverAnt3FitnesCLBoth::fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result)
{
	//individs
	//individs
	const AUTOMAT* individsCPU = individs.data();
	ANT_FITNES_TYPE* resultCPU = result.data();

	const AUTOMAT* individsGPU = individs.data() + cpuCount;
	ANT_FITNES_TYPE* resultGPU = result.data() + cpuCount;

	boost::thread_group group;
	boost::barrier firstBarrier(3);
	boost::this_thread::disable_interruption di;
	
	CFitnesInvoker<AUTOMAT> invokerCPU(clCPUFitnes.get(), individsCPU, resultCPU, firstBarrier);
	group.create_thread(invokerCPU);
	
	CFitnesInvoker<AUTOMAT> invokerGPU(clGPUFitnes.get(), individsGPU, resultGPU, firstBarrier);
	group.create_thread(invokerGPU);

	firstBarrier.wait();
	group.interrupt_all();
	group.join_all();
}

void CCleverAnt3FitnesCLBoth::setMaps(std::vector<CMapPtr> maps) 
{
	clCPUFitnes->setMaps(maps);
	clGPUFitnes->setMaps(maps);
}

std::string CCleverAnt3FitnesCLBoth::getInfo() const
{
	std::string result = clCPUFitnes->getInfo() + " + " + clGPUFitnes->getInfo();
	return result;
}