#pragma once
#include "CMap.h"
#include "CRandom.h"
#include "CAutomat.h"
#include <boost/exception_ptr.hpp>
#include "TimeRun/CTask.h"


class CInvoker;

class CGeneticAlgorithm : public CTask
{
public:
    virtual void nextGeneration( CRandom* rand )  = 0;
    virtual double getAvgFitness() const  = 0;
    virtual double getMaxFitness() const  = 0;
    CAutomatPtr getBestIndivid() const {};

    /*virtual void setMaps( std::vector<CMapPtr> maps ) = 0;
    virtual const CMapPtr getMap( size_t i ) = 0;
    virtual size_t getMapsCount() = 0;
    
    virtual size_t getN() const = 0;
    virtual size_t getM() const = 0;*/
    virtual std::string getDeviceType() const = 0;
    
    //from CTask
    virtual void run() = 0;
    virtual CInvoker* getInvoker() const = 0;
    virtual const boost::exception_ptr& getError() const = 0;
};

typedef boost::shared_ptr<CGeneticAlgorithm> CGeneticAlgorithmPtr;