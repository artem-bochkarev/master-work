#pragma once

#include "GeneticAPI/CGeneticStrategy.h"
#include "GeneticAPI/CMap.h"
#include "CAutomatImpl.h"
#include "GeneticAPI/CInvoker.h"
#include "CLabResultMulti.h"


class CGeneticStrategyCLWrap : public CGeneticStrategy
{
public:
    CGeneticStrategyCLWrap(CStateContainer* states, CActionContainer* actions, 
        CLabResultMulti* res, const std::vector< std::string >& strings );
    virtual void nextGeneration( CRandom* rand );
    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
    //virtual void addMap( CMap maps );
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
    CAutomatPtr getBestIndivid() const;
    virtual ~CGeneticStrategyCLWrap();
    virtual CInvoker* getInvoker() const;
    virtual size_t getN() const;
    virtual size_t getM() const;
    virtual std::string getDeviceType() const;
private:
    int N, M;

    void setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand );

    void initMemory();
    void preStart();
    void swapBuffers();
    CGeneticStrategyCLWrap();
    CGeneticStrategyCLWrap( const CGeneticStrategyCLWrap& );
    void freeIndivids();
    void fillCache() const;
    void pushResults();

    CStateContainer* states;
    CActionContainer* actions;
    CAutomatPtr curIndivid;
    char* buffer;
    int* mapsBuffer;
    float *cache, *bestResults, *sumResults;
    char *steps, *bestIndivids;

    unsigned int *sizes, *srands;

    std::vector<CMapPtr> maps;
    size_t statesCount, stateSize, gensToCount;
    CLabResultMulti* result;
    CInvoker* invoker;
};