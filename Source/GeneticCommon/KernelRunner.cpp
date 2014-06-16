#include "KernelRunner.h"
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/errorMsg.hpp"

#include "Tools/helper.hpp"

void KernelRunner::initKernel(const std::string& filename, cl_device_type deviceType, Tools::Logger* logger)
{
	try
	{
		cl_uint numPlatforms;
		clGetPlatformIDs(0, 0, &numPlatforms);
		cl_platform_id platforms[4];

		clGetPlatformIDs(4, platforms, 0);
		bool isCreated = false;
		for (size_t i = 0; i < numPlatforms; ++i)
		{
			cl_context_properties contextProperties[] =
			{
				CL_CONTEXT_PLATFORM,
				(cl_context_properties)platforms[i],
				0
			};
			try
			{
				context = cl::Context(deviceType, contextProperties);
				isCreated = true;
			}
			catch (cl::Error& error)
			{
				//Tools::throwDetailedFailed("Failed to create GeneticChecks", streamsdk::getOpenCLErrorCodeStr(error.err()), logger);
			}
			if (isCreated)
				break;
		}
		if (!isCreated)
			Tools::throwDetailedFailed("Failed to create GeneticChecks", "No platform found for this request", logger);

		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		queue = cl::CommandQueue(context, devices[0]);
		deviceInfo.setDeviceInfo(devices[0]());
		std::string options = getOptions();//"-cl-opt-disable -g -s \"../CleverAnt3/genShortTables.cl\"";
		//countRanges(options);
		boost::filesystem::path source(filename);

		createProgram(source, options);
		initCLBuffers();
	}
	catch (cl::Error& error)
	{
		std::string msg = "Failed to create Kernel";
		if (deviceType == CL_DEVICE_TYPE_CPU)
			msg.append("(CPU): ");
		else
			msg.append("(GPU): ");
		Tools::throwDetailedFailed(msg, streamsdk::getOpenCLErrorCodeStr(error.err()), logger);
	}
	catch (std::runtime_error& err)
	{
		std::string msg = "Failed to create Kernel";
		if (deviceType == CL_DEVICE_TYPE_CPU)
			msg.append("(CPU): ");
		else
			msg.append("(GPU): ");

		Tools::throwDetailedFailed(msg, err.what(), logger);
	}
}

std::string KernelRunner::getOptions() const
{
	std::string params = "";
	size_t localMem = static_cast<size_t>(deviceInfo.localMemSize);
	/*if (bufSize < localMem)
	{
	params.append(" -D __check_space=__local");
	}
	else
	{
	params.append(" -D __check_space=__global");
	}*/

	using namespace boost::filesystem;
	path curPath = current_path();
	if (exists(curPath / "GeneticTests"))
	{
		params.append(" -I ");
		params.append(curPath.string());
	}
	else
	{
		curPath = curPath / "../";
		if (exists(curPath / "GeneticTests"))
		{
			params.append(" -I ");
			params.append(curPath.string());
		}
	}

	return params;
}

void KernelRunner::createProgram(const boost::filesystem::path& source, const std::string& params)
{
	// build the program from the source in the file
	std::string input;
	//Tools::StringProcessorSimple strProc(vals);
	Tools::readFileToString(input, source);

	createProgramFromString(input, params);
}

void KernelRunner::createProgramFromString(const std::string& input, const std::string& params)
{
	cl::Program::Sources source;
	source.push_back(std::make_pair(input.c_str(), input.size()));
	cl::Program program(context, source);

	try { //compiling OpenCL source
		cl_int res = program.build(devices, params.c_str());
	}
	catch (cl::Error) {
		std::stringstream ss;
		ss << program.getBuildInfo< CL_PROGRAM_BUILD_LOG >(devices[0]);
		std::cerr << ss.str() << std::endl;
		Tools::throwDetailedFailed("File couldn't be compiled", ss.str().c_str(), 0);
	}
	try { //Trying to create OpenCL kernel
		kernelGen = cl::Kernel(program, "genetic_1d");
	}
	catch (cl::Error& err) {
		Tools::throwDetailedFailed("Failed to create kernel", streamsdk::getOpenCLErrorCodeStr(err.err()), 0);
	}
}

KernelRunner::~KernelRunner()
{}