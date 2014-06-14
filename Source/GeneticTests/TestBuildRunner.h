#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include <boost/filesystem.hpp>
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/Logger.h"
#include <string>
#include "TypeDefines.h"
#include "GeneticCommon/KernelRunner.h"

class TestBuildRunner : public CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE>, public KernelRunner
{
public:
	TestBuildRunner(const std::vector< std::string >& strings, Tools::Logger& log);
	virtual ~TestBuildRunner();

	std::string getInfo() const;
	virtual void run() override;

protected:

	virtual void initCLBuffers() override;
	virtual std::string getOptions() const override;

	void setFromStrings(const std::vector< std::string >& strings);
	void prepareData();
	void getData(FITNES_TYPE* result) const;

	size_t getTestSizesSize() const;
	size_t getTestsBufferSize() const;

	const void* getTestSizesPtr() const;
	const void* getTestsBufferPtr() const;

	size_t m_size, bufSize;
private:
	void checkProgrmType(const std::string &source);

	Tools::Logger& logger;
	float* cachedResults;

	cl_device_type deviceType;

	cl::Buffer automatBuffer;
	cl::Buffer clConstSizesBuffer;
	cl::Buffer clResultCacheBuffer;
	cl::Buffer clTestSizesBuffer, clTestsBuffer;

	cl::NDRange globalRange;
	cl::NDRange localRange;
};