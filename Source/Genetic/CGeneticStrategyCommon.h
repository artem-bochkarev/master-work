#pragma once

#include "GeneticAPI/CGeneticAlgorithm.h"
#include "CMap.h"
#include "CLabResultMulti.h"
#include "Tools/Logger.h"

class CAntFitnes;


class CGeneticStrategyCommon : public CGeneticAlgorithm
{
public:
    CGeneticStrategyCommon(CStateContainer* states, CActionContainer* actions, 
        CLabResultMulti* res, const std::vector< std::string >& strings, Tools::Logger& logger );
    virtual void nextGeneration( CRandom* rand ) = 0;
    //virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
    virtual CAutomatPtr getBestIndivid() const;
    virtual ~CGeneticStrategyCommon() {}
    virtual CInvoker* getInvoker() const;
    virtual size_t getN() const;
    virtual size_t getM() const;
    virtual std::string getDeviceType() const =0;
    virtual const boost::exception_ptr& getError() const;
protected:
    virtual void pushResults();

    CStateContainer* states;
    CActionContainer* actions;
    CAntFitnes* fitnesFunctor;

    int N, M;
    CLabResultMulti* result;
    CInvoker* invoker;
	Tools::Logger& logger;
    //std::vector<CMapPtr> maps;
    boost::exception_ptr error;
};