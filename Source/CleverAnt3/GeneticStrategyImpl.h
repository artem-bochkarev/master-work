#pragma once
#include "GeneticParams.h"
#include "GeneticCommon/CleverAntStrategy.h"
#include "CleverAnt3Fitnes.h"
#include "GeneticCommon/AutomatShortTables.h"
#include "GeneticCommon/AntCommon.h"
#include "GeneticCommon/LabResultMulti.h"
#include "Tools/Logger.h"
#include "CleverAnt3Fitnes.h"

typedef CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, 4, 8> AUTOMAT;

class CGeneticStrategyImpl : public CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CGeneticStrategyImpl(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* res, 
		CCleverAnt3FitnesPtr fitnes,
		const std::vector< std::string >& strings, Tools::Logger& logger);
	virtual void nextGeneration(CRandom* rand);
	//virtual void setMaps(std::vector<CMapPtr> maps);
	//virtual const CMapPtr getMap(size_t i);
	//virtual size_t getMapsCount();
	virtual ANT_FITNES_TYPE getAvgFitness() const;
	virtual ANT_FITNES_TYPE getMaxFitness() const;
	virtual CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> getBestIndivid() const;
	virtual ~CGeneticStrategyImpl();
	virtual CInvoker* getInvoker() const;
	virtual std::string getDeviceType() const;
	virtual const boost::exception_ptr& getError() const;
	virtual std::string getPoolInfo() const;

	virtual void run();
protected:
	CCleverAnt3FitnesPtr fitnesFunctor;
	std::vector<AUTOMAT> individuals;

	int N, M;
	ANT_FITNES_TYPE m_avg, m_max;
	mutable AUTOMAT m_best;
	//CLabResultMulti* result;
	CInvoker* invoker;
	//Tools::Logger& logger;
	//std::vector<CMapPtr> maps;
	boost::exception_ptr error;
	CRandomPtr m_pRandom;
};

typedef boost::shared_ptr<CGeneticStrategyImpl> CGeneticStrategyImplPtr;