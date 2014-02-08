#pragma once
#include "GeneticParams.h"
#include "GeneticAPI/CGeneticAlgorithm.h"
#include "GeneticCommon/LaboratoryMulti.hpp"
#include "GeneticCommon/LabResultMulti.hpp"
#include "Tools/Logger.h"

class CLaboratoryFactory
{
public:
	static CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> getLaboratory(Tools::Logger& logger, const std::string& fileName);
    static CLaboratoryPtr createLaboratory( Tools::Logger& logger, std::vector<std::string> &args );
private:
	static  CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> noFile(Tools::Logger& logger);
    
	static CAntCommonPtr<COUNTERS_TYPE> createAntCommon(const std::vector<std::string>& strings);

	static  CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE> createLaboratory(CAntCommonPtr<COUNTERS_TYPE> antCommonPtr,
		CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE> strategy, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults);

    CLaboratoryFactory();
    CLaboratoryFactory( const CLaboratoryFactory& );
    CLaboratoryFactory& operator = ( const CLaboratoryFactory& );
};

class CStrategyFactory
{
public:
    static CGeneticAlgorithmPtr<COUNTERS_TYPE, INPUT_TYPE> getStrategy();
	static  CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> createStrategy(const std::vector< std::string >& strings,
		CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger);
private:
	static CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> createStrategyCL(const std::vector< std::string >& strings,
		CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger);
	static CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> createStrategyCPU(const std::vector< std::string >& strings,
		CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger);
	static CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> createStrategyWRAP(const std::vector< std::string >& strings,
		CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger);
    CStrategyFactory();
};