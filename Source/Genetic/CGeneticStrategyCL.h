#pragma once

#include "CGeneticStrategyCommon.h"
#include "CMap.h"
#include "CAutomatImpl.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#define __NO_STD_VECTOR //no std vector
#define __MAX_DEFAULT_VECTOR_SIZE 100
#include "CL/cl.hpp"
#include "CLabResultMulti.h"
#include "Tools/Logger.h"


class CGeneticStrategyCL : public CGeneticStrategyCommon
{
    enum DeviceType
    {
        CPU,
        GPU
    };
    enum Mode
    {
        OLD,
        NEW
    };
public:
    CGeneticStrategyCL(CStateContainer* states, CActionContainer* actions, 
		CLabResultMulti* res, const std::vector< std::string >& strings, Tools::Logger& logger );

    virtual void run();

    virtual void nextGeneration( CRandom* rand );
    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
    //virtual void addMap( CMap maps );
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
    CAutomatPtr getBestIndivid() const;
    virtual ~CGeneticStrategyCL();
    virtual CInvoker* getInvoker() const;
    virtual size_t getN() const;
    virtual size_t getM() const;
    virtual std::string getDeviceType() const;
    virtual const boost::exception_ptr& getError() const;
protected:
    void pushResults();
private:
    DeviceType deviceType;
    Mode mode;
    std::string sProfile;
    std::string sName;
    std::string sVendor;
    std::string sVersion;

    void setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand );
    void processString( std::string& str, const std::vector<size_t>& vals ) const;
    void createProgram( const std::vector<size_t>& vals );

    void initCLBuffers() ;
    void initMemory();
    void preStart();
    void swapBuffers();
    CGeneticStrategyCL();
    CGeneticStrategyCL( const CGeneticStrategyImpl& );
    void freeIndivids();
    void fillCache() const;

    CAutomatPtr curIndivid;
    char* buffer;
    int* mapsBuffer;
    float *cache, *bestResults, *sumResults;
    char *steps, *bestIndivids;

    cl::Buffer statesBufCL1, statesBufCL2, tempBuffer;
    cl::Buffer maxCache, avgCache, bestPos;
    cl::Buffer mapBufCL, mapCL;
    cl::Buffer cacheBuffer;
    cl::Buffer sizesBuf, srandBuf;
    unsigned int *sizes, *srands;

    cl::Buffer bestResult, sumResult;
    cl::Buffer bestIndivid;

    cl::Kernel kernelGen;
    cl::Context context;
    cl::CommandQueue queue;
    //cl::Device device;
    cl::vector<cl::Device> devices;
    cl::NDRange globalRange;
    cl::NDRange localRange;

    std::vector<CMapPtr> maps;
    size_t statesCount, stateSize, gensToCount;

    CRandomPtr m_pRandom;
};