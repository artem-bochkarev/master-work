#pragma once

#include "GeneticCommon/CleverAntStrategy.hpp"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include "CL/cl.hpp"
#include <boost/filesystem.hpp>
#include "SDKUtil/include/SDKCommon.hpp"


class CGeneticStrategyCLv2 : public CGeneticStrategyCommon<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>
{
public:
	CGeneticStrategyCLv2(const boost::filesystem::path& source, CAntCommonPtr<COUNTERS_TYPE> pAntCommon,
		CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* res, CAntFitnesPtr fitnes, const std::vector< std::string >& strings, Tools::Logger& logger);

    virtual void run();

    virtual void nextGeneration( CRandom* rand );
    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
    //virtual void addMap( CMap maps );
    virtual double getAvgFitness() const;
    virtual double getMaxFitness() const;
	CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> getBestIndivid() const;
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
    //cl::PlatformInfo platformInfo;
	streamsdk::SDKDeviceInfo deviceInfo;

    void setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand );
    void processString( std::string& str, const std::vector<size_t>& vals ) const;
    void createProgram( const boost::filesystem::path& source, const std::string& params );
    void countRanges( std::string& options );
    void addGeneration( char* buff, float* results );

    void initCLBuffers() ;
    void initMemory();
    void preStart();
    void swapBuffers();
    CGeneticStrategyCLv2();
	CGeneticStrategyCLv2(const CGeneticStrategyCLv2&);
    void freeIndivids();
    void fillCache() const;

	CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> curIndivid;
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
    std::vector<cl::Device> devices;
    cl::NDRange globalRange;
    cl::NDRange localRange;

    std::vector<CMapPtr> maps;
    size_t stateSize, gensToCount;
    CRandomPtr m_pRandom;
};