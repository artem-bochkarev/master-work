#pragma once
#include "CAutomat.h"
#include "CMap.h"
#include "CGeneticAlgorithm.h"
#include "TimeRun/CTimeRunner.h"

class CLaboratory : public CTimeRunner
{
public:
    //from CTimeRunner
    /*virtual void start();
    virtual void runForTime( int milisec );
    virtual void pause();
    virtual size_t getRunCount() const;
    virtual bool isRunning() const;*/

    virtual size_t getGenerationNumber() const = 0;
    virtual double getMaxFitness( size_t i ) const = 0;
    virtual double getAvgFitness( size_t i ) const = 0;
    virtual CAutomatPtr getBestInd( size_t i ) const = 0;
    /*virtual void setMaps( std::vector<CMapPtr> maps ) = 0;
    virtual const CMapPtr getMap( size_t i ) = 0;
    virtual size_t getMapsCount() = 0;*/
    
    virtual const CGeneticAlgorithmPtr getStrategy() const = 0;
    virtual const CStateContainerPtr getStates() const = 0;
    virtual const CActionContainerPtr getActions() const = 0;

    virtual void writeInfo( std::ostream& ) const;
};

typedef boost::shared_ptr<CLaboratory> CLaboratoryPtr;