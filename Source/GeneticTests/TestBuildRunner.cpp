#include "TestBuildRunner.h"
#include "CL/cl.hpp"
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/errorMsg.hpp"
#include "Tools/helper.hpp"
#include "GeneticCommon/Test.hpp"
#include <boost/spirit/include/qi.hpp>
#include "Tools/StringProcessor.h"
#include <boost/lexical_cast.hpp>
#include <boost/thread.hpp>

TestBuildRunner::~TestBuildRunner()
{
	delete cachedResults;
}

TestBuildRunner::TestBuildRunner(const std::vector< std::string >& strings, Tools::Logger& log)
:logger(log), m_size(128), cachedResults(0)
{
	logger << "[INIT] Initializing TestBuildRunner.\n";
	setFromStrings(strings);
	//bufSize = sizeof(AUTOMAT)* m_size;
	globalRange = cl::NDRange(m_size);
	localRange = cl::NDRange(cl::NullRange);
	//automatSize = commonDataSize + pAntCommon->statesCount() * stateSize;
	//stateSize = 16;//1 << statesCount;
	initKernel(clFilePath, deviceType, &log);

	cachedResults = new float[m_size];
}

void TestBuildRunner::setFromStrings(const std::vector< std::string >& strings)
{
	using namespace boost::spirit::qi;
	std::string deviceTypeStr;
	char c = 'C';
	for (size_t i = 0; i < strings.size(); ++i)
	{
		const std::string& str = strings[i];
		if (phrase_parse(str.begin(), str.end(), "GENERATION_SIZE=" >> int_ >> ";", space, m_size))
			continue;
		if (phrase_parse(str.begin(), str.end(), "DEVICE_TYPE=" >> char_ >> ";", space, c))
			continue;
	}
	deviceType = CL_DEVICE_TYPE_CPU;
	if (c == 'G')
		deviceType = CL_DEVICE_TYPE_GPU;
}

std::string TestBuildRunner::getInfo() const
{
	std::string res;
	if (deviceType == CL_DEVICE_TYPE_CPU)
		res = "OpenCL on CPU, ";
	else
		res = "OpenCL on GPU, ";

	res.append(deviceInfo.vendorName);
	res.append(": ");
	res.append(deviceInfo.name);
	return res;
}

std::string TestBuildRunner::getOptions() const
{
	std::string params = "";
	size_t localMem = static_cast<size_t>(deviceInfo.localMemSize);
	if (bufSize < localMem)
	{
		params.append(" -D __check_space=__local");
	}
	else
	{
		params.append(" -D __check_space=__global");
	}

	params.append(KernelRunner::getOptions());
	return params;
}

void TestBuildRunner::initCLBuffers()
{
	std::string lastCreated = "no one created)";
	try
	{
		//statesBufCL1 = cl::Buffer(context, CL_MEM_READ_ONLY, m_size*sizeof(AUTOMAT));
		//lastCreated = "statesBufCL1)";
		clConstSizesBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, 5 * sizeof(int));
		lastCreated = "sizesBuf)";
		clResultCacheBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, m_size * sizeof(float));
		lastCreated = "resultCache)";
	}
	catch (cl::Error& err)
	{
		std::string msg = "Failed to init buffers(last was ";
		msg.append(lastCreated);
		throwDetailedFailed(msg, streamsdk::getOpenCLErrorCodeStr(err.err()), &logger);
	}
}

/*void TestBuildRunner::setMaps(std::vector<CMapPtr> maps)
{
	CCleverAnt3Fitnes::setMaps(maps);

	mapSize = getMap(0)->getSizeInts();
	mapCL = cl::Buffer(context, CL_MEM_READ_ONLY, mapSize * 4);
	mapsBuffer = new int[mapSize * m_size];
	int* oneMap = new int[mapSize];
	getMap(0)->toIntBuffer(oneMap);

	mapBufCL = cl::Buffer(context, CL_MEM_READ_WRITE, m_size * mapSize * sizeof(int));
	mapBufConst = cl::Buffer(context, CL_MEM_READ_ONLY, m_size * mapSize * sizeof(int));
	for (size_t i = 0; i < m_size; ++i)
	{
		memcpy(mapsBuffer + i*mapSize, oneMap, mapSize*sizeof(int));
	}

	AUTOMAT aut;

	//__kernel void genetic_2d(__global uint* individs, __global const uint* constSizes,
	//__global int* mapBuffer, __constant int* maps, __global float* resultCache)
	try
	{
		queue.enqueueWriteBuffer(mapBufConst, CL_FALSE, 0, m_size * mapSize * sizeof(int), mapsBuffer);
		kernelGen.setArg(0, statesBufCL1);
		kernelGen.setArg(1, sizesBuf);
		kernelGen.setArg(2, mapBufCL);
		kernelGen.setArg(3, mapCL);
		kernelGen.setArg(4, resultCache);

		//not used now, just compatibility
		size_t sizes[5];
		sizes[0] = STATES_COUNT;
		sizes[1] = 1 << STATES_COUNT;
		if (sizeof(AUTOMAT) % 4 != 0)
			Tools::throwFailed("sizeof(AUTOMAT) % 4 != 0", 0);
		sizes[2] = sizeof(AUTOMAT) / 4;

		sizes[3] = aut.getBufferOffset();
		sizes[4] = mapSize;

		queue.enqueueWriteBuffer(sizesBuf, CL_FALSE, 0, 5 * 4, sizes);
		queue.finish();
	}
	catch (cl::Error& error)
	{
		delete[] oneMap;
		Tools::throwDetailedFailed("[FAILED] to set arguments", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	}
	delete[] oneMap;
}

void TestBuildRunner::fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result)
{
	/*CCleverAnt3FitnesCPU fitCPU(m_steps);
	fitCPU.setMaps(maps);
	fitCPU.fitnes(&individs[0]);*/

	//m_size = individs.size();
/*	std::string counterName = perfFitnesFunctionCL_CPU;
	if (deviceType == CL_DEVICE_TYPE_GPU)
		counterName = perfFitnesFunctionCL_GPU;
	CTimeCounter counter(counterName);
	if (m_size != individs.size())
		Tools::throwFailed("Different sizes!!!", &logger);
	prepareData(individs.data());
	run();
	getData(result.data());
}

void TestBuildRunner::fitnes(const AUTOMAT* individs, ANT_FITNES_TYPE* result)
{
	//if (m_size != individs.size())
	//	Tools::throwFailed("Different sizes!!!", &logger);
	std::string counterName = perfFitnesFunctionCL_CPU;
	if (deviceType == CL_DEVICE_TYPE_GPU)
		counterName = perfFitnesFunctionCL_GPU;
	CTimeCounter counter(counterName);
	prepareData(individs);
	run();
	getData(result);
}*/

void TestBuildRunner::prepareData()
{
	cl_uint sizes[5];
	try
	{
		queue.enqueueWriteBuffer(clTestSizesBuffer, CL_FALSE, 0, getTestSizesSize(), getTestSizesPtr());
		queue.enqueueWriteBuffer(clTestsBuffer, CL_FALSE, 0, getTestsBufferSize(), getTestsBufferPtr());
		queue.enqueueWriteBuffer(clConstSizesBuffer, CL_FALSE, 0, 4 * 5, sizes);
	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("[FAILED] to set arguments", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	}
	//logger << "[SUCCESS] Buffers was wrtitten to the device\n";
}

void TestBuildRunner::run()
{
	try
	{
		//queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NullRange );
		boost::this_thread::disable_interruption di;
		queue.finish();

		queue.enqueueNDRangeKernel(kernelGen, cl::NullRange, globalRange, localRange);
		queue.finish();
		queue.enqueueReadBuffer(clResultCacheBuffer, CL_TRUE, 0, m_size*sizeof(float), cachedResults);
	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("[FAILED] Failed to get next generation", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	}
	catch (std::exception& ex)
	{
		int k = 0;
		throw ex;
	}
}

void TestBuildRunner::getData(FITNES_TYPE* result) const
{
	for (size_t i = 0; i < m_size; ++i)
	{
		result[i] = cachedResults[i];
	}
}
