#include "stdafx.h"
#include <fstream>
#include "CLaboratoryFactory.h"
#include "GeneticCommon/ActionContainerImpl.hpp"
#include "GeneticCommon/LabResultMulti.hpp"
#include "CGeneticStrategyImpl.h"
#include "CGeneticStrategyCLv3.h"
#include "CGeneticStrategyCLWrap.h"
#include "GeneticCommon/LaboratoryMulti.hpp"
#include "GeneticCommon/CleverAntMapImpl.h"
#include "GeneticCommon/CleverAntMapFactory.hpp"
#include "GeneticCommon/AutomatShortTables.h"
#include <cstdlib>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/assert.hpp>
#include <fstream>
 
CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> CLaboratoryFactory::getLaboratory(Tools::Logger& logger, const std::string& fileNameM)
{
    std::string fileName = fileNameM;
    if ( fileName.length() == 0 )
        fileName = "config.txt";
    std::ifstream in( fileName.c_str() );
    boost::filesystem::path source(fileName);
    if ( !boost::filesystem::exists(source) )
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
    
	CAntCommonPtr<COUNTERS_TYPE> antCommon = createAntCommon(strings);
	CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults(new CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>());

	CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> strategy = CStrategyFactory::createStrategy(strings, antCommon, labResults, logger);
    return createLaboratory( antCommon, strategy, labResults );
}

CLaboratoryPtr CLaboratoryFactory::createLaboratory( Tools::Logger& logger, std::vector<std::string> &args )
{
	CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> laboratory;

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
		map1 = CMapFactory<CMapImpl>::getMap("map1.txt");
    }else
    {
		map1 = CMapFactory<CMapImpl>::getMap(args[1].c_str());
    }

    std::vector<CMapPtr> maps;
    maps.push_back( map1 );
    laboratory->setMaps( maps );
    return laboratory;
}

CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> CLaboratoryFactory::noFile(Tools::Logger& logger)
{
	CActionContainerPtr<COUNTERS_TYPE> actions(new CActionContainerImpl<COUNTERS_TYPE>());
	actions->addAction(0, "move forward");
	actions->addAction(1, "turn right");
	actions->addAction(2, "turn left");
	CAntCommonPtr<COUNTERS_TYPE> antCommon(new CAntCommon<COUNTERS_TYPE>(4, actions));
    
	CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults(new CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>());
    std::vector< std::string > strings;
	CAntFitnesCPUPtr fitnesFunctor(new CAntFitnesCPU(DEFAULT_STEPS_COUNT));
	CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> strategy(new CGeneticStrategyImpl< CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE> >(antCommon, labResults.get(), fitnesFunctor, strings, logger));
    
	return CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(
		new CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(antCommon, strategy, labResults));
}

/*CActionContainerPtr CLaboratoryFactory::createActions( const std::vector< std::string >& strings )
{
    CActionContainerPtr actions( new CActionContainerImpl<COUNTERS_TYPE>() );
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
	ant_common::statesCount = rez;
    for ( char i=0; i<rez; ++i )
    {
        states->addState( i );
    }
    return states;
}*/

CAntCommonPtr<COUNTERS_TYPE> CLaboratoryFactory::createAntCommon(const std::vector<std::string>& strings)
{
	CActionContainerPtr<COUNTERS_TYPE> actions(new CActionContainerImpl<COUNTERS_TYPE>());
	actions->addAction(0, "move forward");
	actions->addAction(1, "turn right");
	actions->addAction(2, "turn left");

	int statesCount = 0;
	for (size_t i = 0; i < strings.size(); ++i)
	{
		const std::string& str = strings[i];
		if (boost::starts_with(str, "states"))
		{
			int b = str.find("=");
			++b;
			int e = str.find(";");
			const std::string tmp(str.substr(b, e));
			statesCount = std::atoi(tmp.c_str());
		}
	}
	CAntCommonPtr<COUNTERS_TYPE> a(new CAntCommon<COUNTERS_TYPE>(statesCount, actions));
	return a;
}

CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> CLaboratoryFactory::createLaboratory(CAntCommonPtr<COUNTERS_TYPE> antCommon,
	CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> strategy, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults)
{
	return CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(
		new CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(antCommon, strategy, labResults));
}

//-=---==---=-=---==---=-=---==---=-=---==---=-=---==---=-=---==---=-=---==---=-=---==---=-=---==---=-=---==---=

CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> CStrategyFactory::createStrategy(const std::vector< std::string >& strings,
	CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger)
{
	/*ant_common::states = states.get();
	ant_common::actions = actions.get();*/
	for (size_t i = 0; i < strings.size(); ++i)
	{
		const std::string& str = strings[i];
		if (boost::starts_with(str, "strategy"))
		{
			int b = str.find("=");
			++b;
			int e = str.find(";");
			const std::string tmp(str.substr(b, e));
			if (tmp[0] == 'C')
			{
				if (str.find("Wrap") == -1)
				{
					return createStrategyCL(strings, antCommon, labResults, logger);
				}
				else
				{
					return createStrategyWRAP(strings, antCommon, labResults, logger);
				}
			}
		}
	}
	return createStrategyCPU(strings, antCommon, labResults, logger);
}

CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> CStrategyFactory::createStrategyCPU(const std::vector< std::string >& strings,
	CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger)
{
	//size_t ste
	CAntFitnesCPUPtr fitnesFunctor(new CAntFitnesCPU(DEFAULT_STEPS_COUNT));
	for (size_t i = 0; i < strings.size(); ++i)
	{
		const std::string& str = strings[i];
		if (boost::starts_with(str, "transition"))
		{
			int b = str.find("=");
			++b;
			int e = str.find(";");
			const std::string type(str.substr(b, e));
			
			if (boost::starts_with(type, "Short") || boost::starts_with(type, "short"))
			{
				return CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(
					new CGeneticStrategyImpl< 
						CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT> 
					>(antCommon, labResults.get(), fitnesFunctor, strings, logger));
			}
			else
			if (boost::starts_with(type, "Decision") || boost::starts_with(type, "decision"))
			{
				BOOST_ASSERT_MSG(0, "Decision trees not implemented yet, use full tables instead");
			}else
			if (boost::starts_with(type, "Full") || boost::starts_with(type, "full"))
			{
				return CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(
					new CGeneticStrategyImpl< CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE> >(antCommon,
					labResults.get(), fitnesFunctor, strings, logger));
			}
		}
	}
	return CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(
		new CGeneticStrategyImpl< CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE> >(antCommon,
		labResults.get(), fitnesFunctor, strings, logger));
}

CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> CStrategyFactory::createStrategyCL(const std::vector< std::string >& strings,
	CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger)
{
	boost::filesystem::path source("gen.cl");
	if (!boost::filesystem::exists(source))
	{
		source = boost::filesystem::path("gen.c");
		if (!boost::filesystem::exists(source))
			throw std::runtime_error("[ERROR] Source file not found\n");
	}
	std::ifstream in(source.generic_string().c_str());
	char buf[500];
	in.getline(buf, 490);
	std::string str(buf);
	int version = 1;
	if (str.find("#version=") != -1)
	{
		str = str.substr(str.find("=") + 1, str.length() - str.find("="));
		version = boost::lexical_cast<int>(str);
	}
	in.close();
	//ToDo: continue it!
	CAntFitnesNonePtr fitnesFunctor(new CAntFitnesNone());
	if (version == 1)
		BOOST_ASSERT_MSG(0, "Not supported anymore");
		//return CGeneticStrategyCommonPtr(new CGeneticStrategyCL(antCommon, labResults.get(), fitnesFunctor, strings, logger));
	return CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(
		new CGeneticStrategyCLv3(source, antCommon, labResults.get(), fitnesFunctor, strings, logger));
}

CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE> CStrategyFactory::createStrategyWRAP(const std::vector< std::string >& strings,
	CAntCommonPtr<COUNTERS_TYPE> antCommon, CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults, Tools::Logger& logger)
{
	CAntFitnesNonePtr fitnesFunctor(new CAntFitnesNone());
	return CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>(
		new CGeneticStrategyCLWrap(antCommon, labResults.get(), fitnesFunctor, strings, logger));
}
