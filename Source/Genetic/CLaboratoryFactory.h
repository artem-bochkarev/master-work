#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#include "CLaboratoryMulti.h"
#include "GeneticCommon/LabResultMulti.hpp"
#include "Tools/Logger.h"

class CLaboratoryFactory
{
public:
    static CLaboratoryMultiPtr getLaboratory( Tools::Logger& logger, const std::string& fileName );
    static CLaboratoryPtr createLaboratory( Tools::Logger& logger, std::vector<std::string> &args );
private:
    static  CLaboratoryMultiPtr noFile( Tools::Logger& logger );
    
	static CAntCommonPtr<COUNTERS_TYPE> createAntCommon(const std::vector<std::string>& strings);

	static  CLaboratoryMultiPtr createLaboratory(CAntCommonPtr<COUNTERS_TYPE> antCommonPtr, 
		CGeneticStrategyCommonPtr strategy, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults);

    CLaboratoryFactory();
    CLaboratoryFactory( const CLaboratoryFactory& );
    CLaboratoryFactory& operator = ( const CLaboratoryFactory& );
};

class CStrategyFactory
{
public:
    static CGeneticAlgorithmPtr getStrategy();
	static  CGeneticStrategyCommonPtr createStrategy(const std::vector< std::string >& strings,
		CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger);
private:
	static CGeneticStrategyCommonPtr createStrategyCL(const std::vector< std::string >& strings,
		CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger);
	static CGeneticStrategyCommonPtr createStrategyCPU(const std::vector< std::string >& strings,
		CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger);
	static CGeneticStrategyCommonPtr createStrategyWRAP(const std::vector< std::string >& strings,
		CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger);
    CStrategyFactory();
};