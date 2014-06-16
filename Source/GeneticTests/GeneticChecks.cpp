#include "GeneticChecks.h"

#include "Tools/errorMsg.hpp"
#include "Tools/helper.hpp"

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

	initKernel(genCheckFileName, deviceType);
	
	cachedResults = new cl_float[m_size];
}

void GeneticChecks::createProgram(const boost::filesystem::path& sourceFile, const std::string& params, Tools::Logger* log)
{
	// build the program from the source in the file
	std::string input;
	//Tools::StringProcessorSimple strProc(vals);
	Tools::readFileToString(input, sourceFile);

	Tools::changeDefine(input, Tools::Define("GLOBAL_SIZE", boost::lexical_cast<std::string, size_t>(m_size)));
	Tools::changeDefine(input, Tools::Define("GO_VALUE", boost::lexical_cast<std::string, size_t>(eliteCount)));
	createProgramFromString(input, params, log);
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

void GeneticChecks::prepareData(const cl_float* data)
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
