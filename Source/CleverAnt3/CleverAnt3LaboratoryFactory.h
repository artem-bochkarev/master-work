#pragma once
#include "GeneticParams.h"
#include "GeneticAPI/CGeneticAlgorithm.h"
#include "GeneticCommon/LaboratoryMulti.hpp"
#include "GeneticCommon/LabResultMulti.hpp"
#include "Tools/Logger.h"

class CleverAnt3LaboratoryFactory
{
public:
	static CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> getLaboratory(Tools::Logger& logger, const std::string& fileName);
	static CLaboratoryPtr createLaboratory(Tools::Logger& logger, std::vector<std::string> &args);
private:
	static  CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> noFile(Tools::Logger& logger);

	static CAntCommonPtr<COUNTERS_TYPE> createAntCommon(const std::vector<std::string>& strings);

	/*static  CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE> createLaboratory(CAntCommonPtr<COUNTERS_TYPE> antCommonPtr,
		CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE> strategy, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults);
*/
	CleverAnt3LaboratoryFactory();
	CleverAnt3LaboratoryFactory(const CleverAnt3LaboratoryFactory&);
	CleverAnt3LaboratoryFactory& operator = (const CleverAnt3LaboratoryFactory&);
};