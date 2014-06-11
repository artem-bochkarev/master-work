#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include <boost/filesystem.hpp>
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/Logger.h"
#include <string>
#include "TypeDefines.h"

class GeneticChecks
{
public:
	GeneticChecks(const char*, size_t size, size_t elitismVal);
	virtual ~GeneticChecks();

	void run();

	void prepareData(cl_float* data);
	void getData(cl_uint* result) const;

protected:
	size_t m_size, eliteCount;
private:

	std::string getOptions() const;
	void initCLBuffers();
	void createProgram(const boost::filesystem::path& source, const std::string& params);

	float* cachedResults;

	cl_device_type deviceType;
	streamsdk::SDKDeviceInfo deviceInfo;

	cl::Buffer clFLoatBuffer;
	cl::Buffer clResultBuffer;

	cl::Kernel kernelGen;
	cl::Context context;
	cl::CommandQueue queue;
	std::vector<cl::Device> devices;
	cl::NDRange globalRange;
	cl::NDRange localRange;
};