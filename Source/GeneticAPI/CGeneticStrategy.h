#pragma once
#include "CMap.h"
#include "CRandom.h"
#include "CAutomat.h"
#include <boost/exception_ptr.hpp>


class CInvoker;

class CGeneticStrategy
{
public:
    virtual void nextGeneration( CRandom* rand )  = 0;
    virtual double getAvgFitness() const  = 0;
    virtual double getMaxFitness() const  = 0;
    CAutomatPtr getBestIndivid() const {};

    virtual void setMaps( std::vector<CMapPtr> maps ) = 0;
    virtual const CMapPtr getMap( size_t i ) = 0;
    virtual size_t getMapsCount() = 0;
    virtual CInvoker* getInvoker() const = 0;
    virtual size_t getN() const = 0;
    virtual size_t getM() const = 0;
    virtual std::string getDeviceType() const = 0;
    virtual const boost::exception_ptr& getError() const = 0;
};

typedef boost::shared_ptr<CGeneticStrategy> CGeneticStrategyPtr;