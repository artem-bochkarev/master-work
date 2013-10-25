#pragma once

#include "CGeneticStrategyCommon.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#define __NO_STD_VECTOR //no std vector
#define __MAX_DEFAULT_VECTOR_SIZE 100
#include "CL/cl.hpp"
#include <boost/filesystem.hpp>
#include "SDKUtil/SDKCustom.hpp"


class CGeneticStrategyCLv2 : public CGeneticStrategyCommon
{
public:
    CGeneticStrategyCLv2( const boost::filesystem3::path& source, CStateContainer* states, CActionContainer* actions, 
		CLabResultMulti* res, CAntFitnesPtr fitnes, const std::vector< std::string >& strings, Tools::Logger& logger );

    virtual void run();

    virtual void nextGeneration( CRandom* rand );
    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
    //virtual void addMap( CMap maps );
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
    CAutomatPtr getBestIndivid() const;
    virtual ~CGeneticStrategyCLv2();
    virtual CInvoker* getInvoker() const;
    virtual size_t getN() const;
    virtual size_t getM() const;
    virtual std::string getDeviceType() const;
    virtual const boost::exception_ptr& getError() const;
protected:
    void pushResults();
private:
    cl_device_type deviceType;
    streamsdk::PlatformInfo platformInfo;

    void setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand );
    void processString( std::string& str, const std::vector<size_t>& vals ) const;
    void createProgram( const boost::filesystem3::path& source, const std::string& params );
    void countRanges( std::string& options );
    void addGeneration( char* buff, float* results );

    void initCLBuffers() ;
    void initMemory();
    void preStart();
    void swapBuffers();
    CGeneticStrategyCLv2();
    CGeneticStrategyCLv2( const CGeneticStrategyImpl& );
    void freeIndivids();
    void fillCache() const;

    CAutomatPtr curIndivid;
    char *buffer, *buffer2;
    int* mapsBuffer;
    float *cachedResults;
    unsigned int *sizes, *srands;

    cl::Buffer statesBufCL1, statesBufCL2, globalTmpBuffer;
    cl::Buffer mapBufCL, mapCL;
    cl::Buffer sizesBuf, srandBuf;
    cl::Buffer resultCache;


    cl::Kernel kernelGen;
    cl::Context context;
    cl::CommandQueue queue;
    cl::vector<cl::Device> devices;
    cl::NDRange globalRange;
    cl::NDRange localRange;

    std::vector<CMapPtr> maps;
    size_t statesCount, stateSize, gensToCount;
    CRandomPtr m_pRandom;
};