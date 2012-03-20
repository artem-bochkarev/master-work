#include "stdafx.h"
#include "CLaboratoryFactory.h"
#include "CStateContainerImpl.h"
#include "CActionContainerImpl.h"
#include "CLabResultMulti.h"
#include "CGeneticStrategyImpl.h"
#include "CGeneticStrategyCL.h"
#include "CLaboratoryMulti.h"
#include <fstream>
#include <cstdlib>

CLaboratoryPtr CLaboratoryFactory::getLaboratory()
{
    std::ifstream in("config.txt");
    std::vector< std::string > strings;
    if ( !in.is_open() )
        return noFile();
    while ( in.good() )
    {
        char tmp[300];
        in.getline( tmp, 250 );
        strings.push_back( tmp );
    }
    
    CStateContainerPtr states = createStates( strings );
    CActionContainerPtr actions = createActions( strings );
    CLabResultMultiPtr labResults( new CLabResultMulti() );

    CGeneticStrategyPtr strategy = createStrategy( strings, states, actions, labResults );
    return createLaboratory( states, actions, strategy, labResults );
}

CLaboratoryPtr CLaboratoryFactory::noFile()
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
    CGeneticStrategyPtr strategy( new CGeneticStrategyImpl( states.get(), actions.get(), labResults.get(), strings ) );
    
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
        if ( str.find( "states" ) != -1 )
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
        CStateContainerPtr states, CActionContainerPtr actions, CLabResultMultiPtr labResults )
{
    for ( size_t i=0; i < strings.size(); ++i )
    {
        const std::string& str = strings[i];
        if ( str.find( "strategy" ) != -1 )
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            if ( tmp[0] == 'C' )
                return CGeneticStrategyPtr( new CGeneticStrategyCL( states.get(), actions.get(), labResults.get(), strings ) );
        }
    }
    return CGeneticStrategyPtr( new CGeneticStrategyImpl( states.get(), actions.get(), labResults.get(), strings ) );
}

CLaboratoryPtr CLaboratoryFactory::createLaboratory( CStateContainerPtr states, 
        CActionContainerPtr actions, CGeneticStrategyPtr strategy, CLabResultMultiPtr labResults )
{
    return CLaboratoryPtr( new CLaboratoryMulti( states, actions, strategy, labResults ) );
}