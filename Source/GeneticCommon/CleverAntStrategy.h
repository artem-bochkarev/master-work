#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#include "GeneticCommon/CleverAntMap.h"
#include "GeneticCommon/LabResultMulti.hpp"
#include "Tools/Logger.h"
#include "CleverAntFitnes.h"
#include "GeneticCommon\AntCommon.h"

template<typename COUNTERS_TYPE, typename INPUT_TYPE, typename FITNES_TYPE = double>
class CGeneticStrategyCommon : public CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	CGeneticStrategyCommon(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* res, 
		CCleverAntFitnesPtr<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE> fitnes,
		const std::vector< std::string >& strings, Tools::Logger& logger );
    virtual void nextGeneration( CRandom* rand ) = 0;
    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
	virtual CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> getBestIndivid() const;
    virtual ~CGeneticStrategyCommon() {}
    virtual CInvoker* getInvoker() const;
    virtual size_t getN() const;
    virtual size_t getM() const;
    virtual std::string getDeviceType() const =0;
    virtual const boost::exception_ptr& getError() const;
    virtual std::string getPoolInfo() const;

	virtual CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>* getFitnesFunctor();
	virtual const CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE>* getFitnesFunctor() const;
protected:
    virtual void pushResults();

	CAntCommonPtr<COUNTERS_TYPE> pAntCommon;
	CCleverAntFitnesPtr<COUNTERS_TYPE, INPUT_TYPE, FITNES_TYPE> fitnesFunctor;

    int N, M;
	CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* result;
    CInvoker* invoker;
	Tools::Logger& logger;
    //std::vector<CMapPtr> maps;
    boost::exception_ptr error;
};

template<typename C, typename I, typename F = double, typename K = CGeneticStrategyCommon<C,I,F> >
using CGeneticStrategyCommonPtr = boost::shared_ptr< K >;
//typedef boost::shared_ptr<CGeneticStrategyCommon> CGeneticStrategyCommonPtr;