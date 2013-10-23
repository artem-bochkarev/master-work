#include "stdafx.h"
#include <fstream>
#include "CLaboratoryFactory.h"
#include "CStateContainerImpl.h"
#include "CActionContainerImpl.h"
#include "CLabResultMulti.h"
#include "CGeneticStrategyImpl.h"
#include "CGeneticStrategyCL.h"
#include "CGeneticStrategyCLv2.h"
#include "CGeneticStrategyCLWrap.h"
#include "CLaboratoryMulti.h"
#include "CMapFactory.h"
#include <cstdlib>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <fstream>
 
CLaboratoryMultiPtr CLaboratoryFactory::getLaboratory( Tools::Logger& logger, const std::string& fileNameM )
{
    std::string fileName = fileNameM;
    if ( fileName.length() == 0 )
        fileName = "config.txt";
    std::ifstream in( fileName.c_str() );
    boost::filesystem3::path source(fileName);
    if ( !boost::filesystem3::exists(source) )
    {
        int k = 0;
        std::string str = boost::filesystem::current_path().string();
        int z = 1;
    }
    std::vector< std::string > strings;
    if ( !in.is_open() )
	{
		logger << "[WARNING] Can't find config file, so using default settings.\n";
        return noFile( logger );
	}
    while ( in.good() )
    {
        char tmp[300];
        in.getline( tmp, 250 );
        strings.push_back( tmp );
    }
    
    CStateContainerPtr states = createStates( strings );
    CActionContainerPtr actions = createActions( strings );
    CLabResultMultiPtr labResults( new CLabResultMulti() );

    CGeneticAlgorithmPtr strategy = createStrategy( strings, states, actions, labResults, logger );
    return createLaboratory( states, actions, strategy, labResults );
}

CLaboratoryPtr CLaboratoryFactory::createLaboratory( Tools::Logger& logger, std::vector<std::string> &args )
{
    CLaboratoryMultiPtr laboratory;

    if ( args.size() < 1 )
    {
        laboratory = CLaboratoryFactory::getLaboratory( logger, "" );
    }else
    {
        laboratory = CLaboratoryFactory::getLaboratory( logger, args[0].c_str() );
    }
    CMapPtr map1;
    if ( args.size() < 2 )
    {
        map1 = CMapFactory::getMap( "map1.txt" );
    }else
    {
        map1 = CMapFactory::getMap( args[1].c_str() );
    }

    std::vector<CMapPtr> maps;
    maps.push_back( map1 );
    laboratory->setMaps( maps );
    return laboratory;
}

CLaboratoryMultiPtr CLaboratoryFactory::noFile( Tools::Logger& logger )
{
    CStateContainerPtr states( new CStateContainerImpl() );
    for ( char i=0; i<5; ++i )
    {
        states->addState( i );
    }
    CActionContainerPtr actions( new CActionContainerImpl() );
    actions->addAction( 0, "move forward" );
    actions->addAction( 1, "turn right" );
    actions->addAction( 2, "turn left" );
    CLabResultMultiPtr labResults( new CLabResultMulti() );
    std::vector< std::string > strings;
    CGeneticAlgorithmPtr strategy( new CGeneticStrategyImpl( states.get(), actions.get(), labResults.get(), strings, logger ) );
    
    return CLaboratoryMultiPtr( new CLaboratoryMulti( states, actions, strategy, labResults ) );
}

CActionContainerPtr CLaboratoryFactory::createActions( const std::vector< std::string >& strings )
{
    CActionContainerPtr actions( new CActionContainerImpl() );
    actions->addAction( 0, "move forward" );
    actions->addAction( 1, "turn right" );
    actions->addAction( 2, "turn left" );
    return actions;
}

CStateContainerPtr CLaboratoryFactory::createStates( const std::vector< std::string >& strings )
{
    CStateContainerPtr states( new CStateContainerImpl() );
    int rez = 0;
    for ( size_t i=0; i < strings.size(); ++i )
    {
        const std::string& str = strings[i];
        if ( boost::starts_with( str, "states" ) )
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            rez = std::atoi( tmp.c_str() );
        }
    }
    for ( char i=0; i<rez; ++i )
    {
        states->addState( i );
    }
    return states;
}

CGeneticAlgorithmPtr CLaboratoryFactory::createStrategy( const std::vector< std::string >& strings,
        CStateContainerPtr states, CActionContainerPtr actions, CLabResultMultiPtr labResults, Tools::Logger& logger )
{
    for ( size_t i=0; i < strings.size(); ++i )
    {
        const std::string& str = strings[i];
        if ( boost::starts_with( str, "strategy" ) )
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            if ( tmp[0] == 'C' )
            {
                if ( str.find("Wrap") == -1 )
                {
                    return createCLStrategy( strings, states, actions, labResults, logger );
                }else
                {
                    return CGeneticAlgorithmPtr( new CGeneticStrategyCLWrap( states.get(), actions.get(), labResults.get(), strings, logger ) );
                }
            }
        }
    }
    return CGeneticAlgorithmPtr( new CGeneticStrategyImpl( states.get(), actions.get(), labResults.get(), strings, logger ) );
}

CGeneticAlgorithmPtr CLaboratoryFactory::createCLStrategy( const std::vector< std::string >& strings,
        CStateContainerPtr states, CActionContainerPtr actions, CLabResultMultiPtr labResults, Tools::Logger& logger )
{
    boost::filesystem3::path source("gen.cl");
    if ( !boost::filesystem3::exists(source) )
    {
        source = boost::filesystem3::path("gen.c");
        if ( !boost::filesystem3::exists(source) )
            throw std::runtime_error("[ERROR] Source file not found\n");
    }
    std::ifstream in (source.generic_string().c_str() );
    char buf[500];
    in.getline( buf, 490 );
    std::string str(buf);
    int version = 1;
    if ( str.find("#version=") != -1 )
    {
        str = str.substr(str.find("=")+1, str.length()-str.find("=") );
        version = boost::lexical_cast<int>( str );
    }
    in.close();
    //ToDo: continue it!
    if (version == 1)
        return CGeneticAlgorithmPtr( new CGeneticStrategyCL( states.get(), actions.get(), labResults.get(), strings, logger ) );
    else
        return CGeneticAlgorithmPtr( new CGeneticStrategyCLv2( source, states.get(), actions.get(), labResults.get(), strings, logger ) );
}

CLaboratoryMultiPtr CLaboratoryFactory::createLaboratory( CStateContainerPtr states, 
        CActionContainerPtr actions, CGeneticAlgorithmPtr strategy, CLabResultMultiPtr labResults )
{
    return CLaboratoryMultiPtr( new CLaboratoryMulti( states, actions, strategy, labResults ) );
}