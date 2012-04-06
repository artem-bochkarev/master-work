#include "stdafx.h"
#include <fstream>
#include "CLaboratoryFactory.h"
#include "CStateContainerImpl.h"
#include "CActionContainerImpl.h"
#include "CLabResultMulti.h"
#include "CGeneticStrategyImpl.h"
#include "CGeneticStrategyCL.h"
#include "CGeneticStrategyCLWrap.h"
#include "CLaboratoryMulti.h"
#include <cstdlib>
#include <boost/algorithm/string/predicate.hpp>

CLaboratoryPtr CLaboratoryFactory::getLaboratory( const char* fileName, Tools::Logger& logger )
{
    std::ifstream in(fileName);
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

    CGeneticStrategyPtr strategy = createStrategy( strings, states, actions, labResults, logger );
    return createLaboratory( states, actions, strategy, labResults );
}

CLaboratoryPtr CLaboratoryFactory::noFile( Tools::Logger& logger )
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
    CGeneticStrategyPtr strategy( new CGeneticStrategyImpl( states.get(), actions.get(), labResults.get(), strings, logger ) );
    
    return CLaboratoryPtr( new CLaboratoryMulti( states, actions, strategy, labResults ) );
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

CGeneticStrategyPtr CLaboratoryFactory::createStrategy( const std::vector< std::string >& strings,
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
                    return CGeneticStrategyPtr( new CGeneticStrategyCL( states.get(), actions.get(), labResults.get(), strings, logger ) );
                }else
                {
                    return CGeneticStrategyPtr( new CGeneticStrategyCLWrap( states.get(), actions.get(), labResults.get(), strings, logger ) );
                }
            }
        }
    }
    return CGeneticStrategyPtr( new CGeneticStrategyImpl( states.get(), actions.get(), labResults.get(), strings, logger ) );
}

CLaboratoryPtr CLaboratoryFactory::createLaboratory( CStateContainerPtr states, 
        CActionContainerPtr actions, CGeneticStrategyPtr strategy, CLabResultMultiPtr labResults )
{
    return CLaboratoryPtr( new CLaboratoryMulti( states, actions, strategy, labResults ) );
}