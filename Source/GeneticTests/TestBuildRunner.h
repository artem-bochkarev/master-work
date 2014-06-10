#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include <boost/filesystem.hpp>
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/Logger.h"
#include <string>
#include "TypeDefines.h"

class TestBuildRunner : public CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE>
{
public:
	TestBuildRunner(const std::vector< std::string >& strings, Tools::Logger& log);
	virtual ~TestBuildRunner();

	std::string getInfo() const;
	virtual void run() override;

protected:
	void setFromStrings(const std::vector< std::string >& strings);
	void prepareData();
	void getData(FITNES_TYPE* result) const;

	size_t getTestSizesSize() const;
	size_t getTestsBufferSize() const;

	const void* getTestSizesPtr() const;
	const void* getTestsBufferPtr() const;

	size_t m_size, bufSize;
private:

	std::string getOptions() const;
	void initCLBuffers();
	void createProgram(const boost::filesystem::path& source, const std::string& params);
	void checkProgrmType(const std::string &source);

	Tools::Logger& logger;
	float* cachedResults;

	cl_device_type deviceType;
	streamsdk::SDKDeviceInfo deviceInfo;

	cl::Buffer automatBuffer;
	cl::Buffer clConstSizesBuffer;
	cl::Buffer clResultCacheBuffer;
	cl::Buffer clTestSizesBuffer, clTestsBuffer;

	cl::Kernel kernelGen;
	cl::Context context;
	cl::CommandQueue queue;
	std::vector<cl::Device> devices;
	cl::NDRange globalRange;
	cl::NDRange localRange;
};