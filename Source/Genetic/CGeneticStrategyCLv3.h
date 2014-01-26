#pragma once

#include "CGeneticStrategyCommon.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include "CL/cl.hpp"
#include <boost/filesystem.hpp>
#include "SDKUtil/include/SDKCommon.hpp"


class CGeneticStrategyCLv3 : public CGeneticStrategyCommon
{
	enum AUTOMAT_TYPE
	{
		FULL_TABLES,
		SHORT_TABLES,
		DECISION_TREE
	};
public:
	CGeneticStrategyCLv3(const boost::filesystem::path& source, CAntCommonPtr<COUNTERS_TYPE> pAntCommon,
		CLabResultMulti* res, CAntFitnesPtr fitnes, const std::vector< std::string >& strings, Tools::Logger& logger);

	virtual void run();

	virtual void nextGeneration(CRandom* rand);
	virtual void setMaps(std::vector<CMapPtr> maps);
	virtual ~CGeneticStrategyCLv3();
	virtual std::string getDeviceType() const;
private:
	cl_device_type deviceType;
	streamsdk::SDKDeviceInfo deviceInfo;

	void checkProgrmType(const std::string &source);
	void setFromStrings(const std::vector< std::string >& strings, CRandomPtr rand);
	void processString(std::string& str, const std::vector<size_t>& vals) const;
	void createProgram(const boost::filesystem::path& source, const std::string& params);
	void countRanges(std::string& options);
	void addGeneration(char* buff, float* results);

	void initCLBuffers();
	void initMemory();
	void preStart();
	void swapBuffers();
	CGeneticStrategyCLv3();
	CGeneticStrategyCLv3(const CGeneticStrategyCLv3&);
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
	std::vector<cl::Device> devices;
	cl::NDRange globalRange;
	cl::NDRange localRange;

	//std::vector<CMapPtr> maps;
	size_t automatSize, gensToCount;
	CRandomPtr m_pRandom;
	AUTOMAT_TYPE automatType;

	static size_t commonDataSize, stateSize, maskSize, tableSize;
};