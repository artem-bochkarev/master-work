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
    static  CActionContainerPtr createActions( const std::vector< std::string >& strings );
    static  CStateContainerPtr createStates( const std::vector< std::string >& strings );
    static  CGeneticAlgorithmPtr createStrategy( const std::vector< std::string >& strings,
        CStateContainerPtr states, CActionContainerPtr actions, CLabResultMultiPtr labResults, Tools::Logger& logger );
    static CGeneticAlgorithmPtr CLaboratoryFactory::createCLStrategy( const std::vector< std::string >& strings,
        CStateContainerPtr states, CActionContainerPtr actions, CLabResultMultiPtr labResults, Tools::Logger& logger );

    static  CLaboratoryMultiPtr createLaboratory( CStateContainerPtr states, 
        CActionContainerPtr actions, CGeneticAlgorithmPtr strategy, CLabResultMultiPtr labResults );

    CLaboratoryFactory();
    CLaboratoryFactory( const CLaboratoryFactory& );
    CLaboratoryFactory& operator = ( const CLaboratoryFactory& );
};

class CStrategyFactory
{
public:
    static CGeneticAlgorithmPtr getStrategy();
private:
    static CGeneticAlgorithmPtr noFile();
    CStrategyFactory();
};