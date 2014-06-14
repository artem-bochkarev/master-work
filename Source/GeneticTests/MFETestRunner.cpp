#include "MFETestRunner.h"

#include "Tools/errorMsg.hpp"
#include "Tools/helper.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>



MFETestRunner::~MFETestRunner()
{
	delete cachedResults;
}

MFETestRunner::MFETestRunner(const char* genCheckFileName, size_t size, size_t checkElements, size_t mfeSize)
: m_size(size), cachedResults(0), m_checkElements(checkElements), m_mfeSize(mfeSize)
{
	globalRange = cl::NDRange(m_size);
	localRange = cl::NDRange(cl::NullRange);
	deviceType = CL_DEVICE_TYPE_CPU;

	initKernel(genCheckFileName, deviceType);

	cachedResults = new cl_float[m_size];
}

void MFETestRunner::createProgram(const boost::filesystem::path& sourceFile, const std::string& params)
{
	// build the program from the source in the file
	std::string input;
	//Tools::StringProcessorSimple strProc(vals);
	Tools::readFileToString(input, sourceFile);

	Tools::changeDefine(input, Tools::Define("GLOBAL_SIZE", boost::lexical_cast<std::string, size_t>(m_size)));
	//Tools::changeDefine(input, Tools::Define("CHECK_COUNT", boost::lexical_cast<std::string, size_t>(m_checkElements)));
	//Tools::changeDefine(input, Tools::Define("MFE_COUNTERS_SIZE", boost::lexical_cast<std::string, size_t>(m_mfeSize)));

	createProgramFromString(input, params);
}

void MFETestRunner::initCLBuffers()
{
	try
	{
		clFLoatBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, m_size * sizeof(cl_float));
		clResultBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, m_size * sizeof(cl_float));
	}
	catch (cl::Error& err)
	{
		std::string msg = "Failed to init buffers";
		Tools::throwDetailedFailed(msg, streamsdk::getOpenCLErrorCodeStr(err.err()), 0);
	}
}

void MFETestRunner::prepareData(const cl_float* data)
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

void MFETestRunner::run()
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

void MFETestRunner::getData(cl_float* result) const
{
	queue.enqueueReadBuffer(clResultBuffer, CL_TRUE, 0, m_size*sizeof(cl_float), result);
}
