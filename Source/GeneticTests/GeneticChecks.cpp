#include "GeneticChecks.h"
#include "CL/cl.hpp"
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/errorMsg.hpp"
#include "Tools/helper.hpp"
#include "GeneticCommon/Test.hpp"
#include <boost/spirit/include/qi.hpp>
#include "Tools/StringProcessor.h"
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

GeneticChecks::~GeneticChecks()
{
	delete cachedResults;
}

GeneticChecks::GeneticChecks(const char* genCheckFileName, size_t size, size_t elitismVal)
: m_size(size), cachedResults(0), eliteCount(elitismVal)
{
	globalRange = cl::NDRange(m_size);
	localRange = cl::NDRange(cl::NullRange);
	deviceType = CL_DEVICE_TYPE_CPU;

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
			catch (cl::Error&)
			{
				//Tools::throwDetailedFailed("Failed to create GeneticChecks", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
			}
			if (isCreated)
				break;
		}
		if (!isCreated)
			Tools::throwDetailedFailed("Failed to create GeneticChecks", "No platform found for this request", 0);

		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		queue = cl::CommandQueue(context, devices[0]);
		deviceInfo.setDeviceInfo(devices[0]());
		std::string options = getOptions();//"-cl-opt-disable -g -s \"../CleverAnt3/genShortTables.cl\"";
		//countRanges(options);
		boost::filesystem::path source(genCheckFileName);

		createProgram(source, options);
		initCLBuffers();
	}
	catch (cl::Error& error)
	{
		std::string msg = "Failed to create GeneticChecks";
		if (deviceType == CL_DEVICE_TYPE_CPU)
			msg.append("(CPU): ");
		else
			msg.append("(GPU): ");
		//Tools::throwDetailedFailed(msg, streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	}
	catch (std::runtime_error& err)
	{
		std::string msg = "Failed to create GeneticChecks";
		if (deviceType == CL_DEVICE_TYPE_CPU)
			msg.append("(CPU): ");
		else
			msg.append("(GPU): ");

		//Tools::throwDetailedFailed(msg, err.what(), &logger);
	}
	cachedResults = new cl_float[m_size];
}

std::string GeneticChecks::getOptions() const
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

void GeneticChecks::createProgram(const boost::filesystem::path& sourceFile, const std::string& params)
{
	// build the program from the source in the file
	std::string input;
	//Tools::StringProcessorSimple strProc(vals);
	Tools::readFileToString(input, sourceFile);

	Tools::changeDefine(input, Tools::Define("GLOBAL_SIZE", boost::lexical_cast<std::string, size_t>(m_size)));
	Tools::changeDefine(input, Tools::Define("GO_VALUE", boost::lexical_cast<std::string, size_t>(eliteCount)));

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

void GeneticChecks::initCLBuffers()
{
	std::string lastCreated = "no one created)";
	try
	{
		clFLoatBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, m_size * sizeof(cl_float));
		clResultBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, m_size * sizeof(cl_uint));
	}
	catch (cl::Error& err)
	{
		std::string msg = "Failed to init buffers(last was ";
		msg.append(lastCreated);
		Tools::throwDetailedFailed(msg, streamsdk::getOpenCLErrorCodeStr(err.err()), 0);
	}
}

void GeneticChecks::prepareData(cl_float* data)
{
	try
	{
		kernelGen.setArg(0, clFLoatBuffer);
		kernelGen.setArg(1, clResultBuffer);
		queue.enqueueWriteBuffer(clFLoatBuffer, CL_FALSE, 0, m_size * sizeof(cl_float), data);
	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("[FAILED] to set arguments", streamsdk::getOpenCLErrorCodeStr(error.err()), 0);
	}
	//logger << "[SUCCESS] Buffers was wrtitten to the device\n";
}

void GeneticChecks::run()
{
	try
	{
		//queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NullRange );
		boost::this_thread::disable_interruption di;
		queue.finish();

		queue.enqueueNDRangeKernel(kernelGen, cl::NullRange, globalRange, localRange);
		queue.finish();
	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("[FAILED] Failed to get next generation", streamsdk::getOpenCLErrorCodeStr(error.err()), 0);
	}
	catch (std::exception& ex)
	{
		int k = 0;
		throw ex;
	}
}

void GeneticChecks::getData(cl_uint* result) const
{
	queue.enqueueReadBuffer(clResultBuffer, CL_TRUE, 0, m_size*sizeof(cl_uint), result);
}
