#pragma once 
#include <string>
#include <boost/filesystem.hpp>
#include "Tools/Logger.h"

#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include "CL/cl.hpp"
#include "SDKUtil/include/SDKCommon.hpp"

class KernelRunner
{
public:
	virtual ~KernelRunner();

	virtual void run() = 0;

protected:
	void initKernel(const std::string& filename, cl_device_type devType, Tools::Logger* logger = 0);
	void createProgramFromString(const std::string& input, const std::string& params, Tools::Logger* log);

	virtual std::string getOptions() const;
	virtual void initCLBuffers() = 0;
	virtual void createProgram(const boost::filesystem::path& source, const std::string& params, Tools::Logger* log);

	cl::Kernel kernelGen;
	cl::Context context;

	cl::CommandQueue queue;
	std::vector<cl::Device> devices;
	streamsdk::SDKDeviceInfo deviceInfo;
};
