#pragma once
#include "GeneticAPI/CGeneticStrategy.h"
#include "GeneticAPI/CLaboratory.h"
#include "CLabResultMulti.h"
#include "Tools/Logger.h"

class CLaboratoryFactory
{
public:
    static CLaboratoryPtr getLaboratory( Tools::Logger& logger, const std::string& fileName );
    static CLaboratoryPtr createLaboratory( Tools::Logger& logger, std::vector<std::string> &args );
private:
    static  CLaboratoryPtr noFile( Tools::Logger& logger );
    static  CActionContainerPtr createActions( const std::vector< std::string >& strings );
    static  CStateContainerPtr createStates( const std::vector< std::string >& strings );
    static  CGeneticStrategyPtr createStrategy( const std::vector< std::string >& strings,
        CStateContainerPtr states, CActionContainerPtr actions, CLabResultMultiPtr labResults, Tools::Logger& logger );
    static CGeneticStrategyPtr CLaboratoryFactory::createCLStrategy( const std::vector< std::string >& strings,
        CStateContainerPtr states, CActionContainerPtr actions, CLabResultMultiPtr labResults, Tools::Logger& logger );

    static  CLaboratoryPtr createLaboratory( CStateContainerPtr states, 
        CActionContainerPtr actions, CGeneticStrategyPtr strategy, CLabResultMultiPtr labResults );

    CLaboratoryFactory();
    CLaboratoryFactory( const CLaboratoryFactory& );
    CLaboratoryFactory& operator = ( const CLaboratoryFactory& );
};

class CStrategyFactory
{
public:
    static CGeneticStrategyPtr getStrategy();
private:
    static CGeneticStrategyPtr noFile();
    CStrategyFactory();
};