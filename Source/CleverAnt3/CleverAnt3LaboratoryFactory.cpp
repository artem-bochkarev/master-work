#include "CleverAnt3LaboratoryFactory.h"

#include "GeneticCommon/ActionContainerImpl.hpp"
#include "GeneticCommon/LabResultMulti.hpp"
#include "GeneticStrategyImpl.h"
#include "CleverAnt3Map.h"
#include "CleverAnt3FitnesCL.h"
#include "GeneticCommon/LaboratoryMulti.hpp"
#include "GeneticCommon/CleverAntMapFactory.hpp"
#include "GeneticCommon/AutomatShortTables.hpp"
#include <cstdlib>
#include <boost/lexical_cast.hpp>
#include <boost/filesystem.hpp>
#include <boost/assert.hpp>
#include <fstream>
#include <boost/spirit/include/qi.hpp>

CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> CleverAnt3LaboratoryFactory::getLaboratory(Tools::Logger& logger, const std::string& fileNameM)
{
	std::string fileName = fileNameM;
	if (fileName.length() == 0)
		fileName = "config.txt";
	std::ifstream in(fileName.c_str());
	boost::filesystem::path source(fileName);
	if (!boost::filesystem::exists(source))
	{
		int k = 0;
		std::string str = boost::filesystem::current_path().string();
		int z = 1;
	}
	std::vector< std::string > strings;
	if (!in.is_open())
	{
		logger << "[WARNING] Can't find config file, so using default settings.\n";
		return noFile(logger);
	}
	while (in.good())
	{
		char tmp[300];
		in.getline(tmp, 250);
		strings.push_back(tmp);
	}

	CActionContainerPtr<COUNTERS_TYPE> actions(new CActionContainerImpl<COUNTERS_TYPE>());
	actions->addAction(0, "move forward");
	actions->addAction(1, "turn right");
	actions->addAction(2, "turn left");
	CAntCommonPtr<COUNTERS_TYPE> antCommon(new CAntCommon<COUNTERS_TYPE>(4, actions));

	CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults(new CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>());

	bool useOpenCL = false;
	using namespace boost::spirit::qi;
	char c = 'C';
	for (const std::string& str : strings)
	{
		if (phrase_parse(str.begin(), str.end(), "USE_OPENCL=" >> bool_ >> ";", space, useOpenCL))
			continue;
		if (phrase_parse(str.begin(), str.end(), "DEVICE_TYPE=" >> char_ >> ";", space, c))
			continue;
	}

	CCleverAnt3FitnesPtr fitnesFunctor;
	if (useOpenCL)
	{
		if ( c != 'B' )
			fitnesFunctor = CCleverAnt3FitnesPtr(new CCleverAnt3FitnesCL(strings, logger));
		else
			fitnesFunctor = CCleverAnt3FitnesPtr(new CCleverAnt3FitnesCLBoth(strings, logger));
	}
	else
		fitnesFunctor = CCleverAnt3FitnesPtr(new CCleverAnt3FitnesCPU(strings));
	


	CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> strategy(
		new CGeneticStrategyImpl/*< CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, 3, INPUT_PARAMS_COUNT> >*/
		(antCommon, labResults.get(), fitnesFunctor, strings, logger));

	return CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>(
		new CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>(antCommon, strategy, labResults));
}

CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> CleverAnt3LaboratoryFactory::noFile(Tools::Logger& logger)
{
	CActionContainerPtr<COUNTERS_TYPE> actions(new CActionContainerImpl<COUNTERS_TYPE>());
	actions->addAction(0, "move forward");
	actions->addAction(1, "turn right");
	actions->addAction(2, "turn left");
	CAntCommonPtr<COUNTERS_TYPE> antCommon(new CAntCommon<COUNTERS_TYPE>(4, actions));

	CLabResultMultiPtr<COUNTERS_TYPE, INPUT_TYPE> labResults(new CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>());
	std::vector< std::string > strings;
	
	size_t count = 256;
	//CCleverAnt3FitnesPtr fitnesFunctor(new CCleverAnt3FitnesCPU(DEFAULT_STEPS_COUNT));
	CCleverAnt3FitnesPtr fitnesFunctor(new CCleverAnt3FitnesCL(strings, logger));
	
	
	CGeneticStrategyCommonPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> strategy(
		new CGeneticStrategyImpl/*< CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, 3, INPUT_PARAMS_COUNT> >*/
		(antCommon, labResults.get(), fitnesFunctor, strings, logger));

	return CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>(
		new CLaboratoryMulti<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>(antCommon, strategy, labResults));
}

CAntCommonPtr<COUNTERS_TYPE> CleverAnt3LaboratoryFactory::createAntCommon(const std::vector<std::string>& strings)
{
	CActionContainerPtr<COUNTERS_TYPE> actions(new CActionContainerImpl<COUNTERS_TYPE>());
	actions->addAction(0, "move forward");
	actions->addAction(1, "turn right");
	actions->addAction(2, "turn left");

	int statesCount = 0;
	using namespace boost::spirit::qi;
	for (const std::string& str : strings)
	{
		if (phrase_parse(str.begin(), str.end(), "STATES_COUNT=" >> bool_ >> ";", space, statesCount))
			continue;
	}
	
	CAntCommonPtr<COUNTERS_TYPE> a(new CAntCommon<COUNTERS_TYPE>(statesCount, actions));
	return a;
}

CLaboratoryPtr CleverAnt3LaboratoryFactory::createLaboratory(Tools::Logger& logger, std::vector<std::string> &args)
{
	CLaboratoryMultiPtr<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE> laboratory;

	if (args.size() < 1)
	{
		laboratory = CleverAnt3LaboratoryFactory::getLaboratory(logger, "");
	}
	else
	{
		laboratory = CleverAnt3LaboratoryFactory::getLaboratory(logger, args[0].c_str());
	}
	CMapPtr map1;
	if (args.size() < 2)
	{
		map1 = CMapFactory<CleverAnt3Map>::getMap("map1.txt");
	}
	else
	{
		map1 = CMapFactory<CleverAnt3Map>::getMap(args[1].c_str());
	}

	std::vector<CMapPtr> maps;
	maps.push_back(map1);
	laboratory->setMaps(maps);
	return laboratory;
}