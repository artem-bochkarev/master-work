#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#include "CLaboratoryMulti.h"
#include "CLabResultMulti.h"
#include "Tools/Logger.h"

class CLaboratoryFactory
{
public:
    static CLaboratoryMultiPtr getLaboratory( Tools::Logger& logger, const std::string& fileName );
    static CLaboratoryPtr createLaboratory( Tools::Logger& logger, std::vector<std::string> &args );
private:
    static  CLaboratoryMultiPtr noFile( Tools::Logger& logger );
    
	static AntCommonPtr createAntCommon(const std::vector<std::string>& strings);

    static  CLaboratoryMultiPtr createLaboratory( AntCommonPtr antCommonPtr, CGeneticStrategyCommonPtr strategy, CLabResultMultiPtr labResults );

    CLaboratoryFactory();
    CLaboratoryFactory( const CLaboratoryFactory& );
    CLaboratoryFactory& operator = ( const CLaboratoryFactory& );
};

class CStrategyFactory
{
public:
    static CGeneticAlgorithmPtr getStrategy();
	static  CGeneticStrategyCommonPtr createStrategy(const std::vector< std::string >& strings,
		AntCommonPtr antCommon, CLabResultMultiPtr labResults, Tools::Logger& logger);
private:
	static CGeneticStrategyCommonPtr createStrategyCL(const std::vector< std::string >& strings,
		AntCommonPtr antCommon, CLabResultMultiPtr labResults, Tools::Logger& logger);
	static CGeneticStrategyCommonPtr createStrategyCPU(const std::vector< std::string >& strings,
		AntCommonPtr antCommon, CLabResultMultiPtr labResults, Tools::Logger& logger);
	static CGeneticStrategyCommonPtr createStrategyWRAP(const std::vector< std::string >& strings,
		AntCommonPtr antCommon, CLabResultMultiPtr labResults, Tools::Logger& logger);
    CStrategyFactory();
};