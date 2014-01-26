#pragma once
#include "CRandom.h"
#include "CAutomat.h"
#include <boost/exception_ptr.hpp>
#include "TimeRun/CTask.h"


class CInvoker;

template< typename NUMBERS_TYPE, typename INPUT_TYPE > class CGeneticAlgorithm : public CTask
{
public:
    virtual void nextGeneration( CRandom* rand )  = 0;
    virtual double getAvgFitness() const  = 0;
    virtual double getMaxFitness() const  = 0;
    //CAutomat<NUMBERS_TYPE, INPUT_TYPE>* getBestIndivid() const {};

    virtual std::string getDeviceType() const = 0;
    virtual std::string getPoolInfo() const = 0;
    
    //from CTask
    virtual void run() = 0;
    virtual CInvoker* getInvoker() const = 0;
    virtual const boost::exception_ptr& getError() const = 0;
};

