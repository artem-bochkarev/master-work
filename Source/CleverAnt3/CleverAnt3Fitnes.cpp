#include "CleverAnt3Fitnes.h"
#include "CleverAnt3Map.h"
#include "GeneticCommon/Test.hpp"

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

void CCleverAnt3FitnesCPU::fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result) const
{
	for (size_t i = 0; i < individs.size(); ++i)
	{
		result[i] = fitnes(&individs[i]);
	}
}