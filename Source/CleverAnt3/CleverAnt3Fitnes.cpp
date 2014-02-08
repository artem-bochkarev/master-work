#include "CleverAnt3Fitnes.h"
#include "GeneticCommon/Test.hpp"

void CCleverAnt3Fitnes::setMaps(std::vector<CMapPtr> maps)
{
	this->maps = maps;
}

const CMapPtr CCleverAnt3Fitnes::getMap(size_t i)
{
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

ANT_FITNESS_TYPE CCleverAnt3FitnesCPU::fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const
{
	return CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>::run(automat, maps[0].get(), DEFAULT_STEPS_COUNT);
}

void CCleverAnt3FitnesCPU::fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNESS_TYPE>& result) const
{
	for (size_t i = 0; i < individs.size(); ++i)
	{
		result[i] = fitnes(&individs[i]);
	}
}