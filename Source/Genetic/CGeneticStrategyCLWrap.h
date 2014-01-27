#pragma once

#include "CGeneticStrategyCommon.h"
#include "CMap.h"
#include "GeneticCommon/AutomatImpl.h"
#include "GeneticCommon/LabResultMulti.hpp"
#include "GenCLWrap.h"
#include "Tools/Logger.h"

class CGeneticStrategyCLWrap : public CGeneticStrategyCommon
{
public:
	CGeneticStrategyCLWrap(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* res, CAntFitnesPtr fitnes,
		const std::vector< std::string >& strings, Tools::Logger& logger );

    virtual void run();

    virtual void nextGeneration( CRandom* rand );
    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
    //virtual void addMap( CMap maps );
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
	CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> getBestIndivid() const;
    virtual ~CGeneticStrategyCLWrap();
    virtual CInvoker* getInvoker() const;
    virtual size_t getN() const;
    virtual size_t getM() const;
    virtual std::string getDeviceType() const;
    virtual const boost::exception_ptr& getError() const;
protected:
    void pushResults();
private:
    void setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand );

    void initMemory();
    void preStart();
    void swapBuffers();
    CGeneticStrategyCLWrap();
    CGeneticStrategyCLWrap( const CGeneticStrategyCLWrap& );
    void freeIndivids();
    void fillCache() const;

	CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> curIndivid;
    CLWrapSettings settings;
    uint *buffer, *outBuffer, *tempBuffer;

    int *mapsBuffer, *mapBuffer;
    int *cache;

    unsigned int *sizes, *srands;

    size_t stateSize, gensToCount;
    CRandomPtr m_pRandom;
};