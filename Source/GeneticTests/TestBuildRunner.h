#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include <boost/filesystem.hpp>
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/Logger.h"
#include <string>
#include "TypeDefines.h"
#include "GeneticCommon/KernelRunner.h"
#include "TestsReader.h"

class TestBuildRunner : /*public CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE>,*/ public KernelRunner
{
public:
	TestBuildRunner(const std::string& clFileName, const std::string& xmlFileName, Tools::Logger& log);
	virtual ~TestBuildRunner();

	std::string getInfo() const;
	virtual void run() override;

protected:

	virtual void initCLBuffers() override;
	virtual std::string getOptions() const override;

	void setFromStrings(const std::vector< std::string >& strings);
	void prepareData();
	void getData(FITNES_TYPE* result) const;

	size_t m_size, bufSize;
	static const size_t constArraySize = 5;
private:
	TestsReader m_testReader;
	void checkProgrmType(const std::string &source);

	Tools::Logger& logger;
	std::vector<TransitionListAutomat> m_automatBuffer;
	std::vector<cl_float> m_cachedResultBuffer;

	cl_device_type deviceType;

	cl::Buffer clAutomatBuffer;
	cl::Buffer clConstSizesBuffer;
	cl::Buffer clResultCacheBuffer;
	cl::Buffer clTestInfoBuffer, clTestsBuffer;

	cl::NDRange globalRange;
	cl::NDRange localRange;
};