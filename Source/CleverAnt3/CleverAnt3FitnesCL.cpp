#include "CleverAnt3FitnesCL.h"
#include "CL/cl.hpp"
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/errorMsg.hpp"
#include "Tools/helper.hpp"
#include "CleverAnt3Map.h"
#include "GeneticCommon/Test.hpp"

CCleverAnt3FitnesCL::~CCleverAnt3FitnesCL()
{
	delete cachedResults;
	delete mapsBuffer;
}

CCleverAnt3FitnesCL::CCleverAnt3FitnesCL(size_t stepsCount, const std::vector< std::string >& strings, Tools::Logger& log)
:logger(log), m_size(100), cachedResults(0), mapsBuffer(0)
{
	logger << "[INIT] Initializing CCleverAnt3FitnesCL.\n";
	setFromStrings(strings);
	globalRange = cl::NDRange(m_size);
	localRange = cl::NDRange(m_size);
	//automatSize = commonDataSize + pAntCommon->statesCount() * stateSize;
	//stateSize = 16;//1 << statesCount;
	try
	{
		logger << "[INIT] Trying to get specified device.\n";

		cl_uint numPlatforms;
		cl_platform_id firstPlatformId;

		clGetPlatformIDs(1, &firstPlatformId, &numPlatforms);
		cl_context_properties contextProperties[] =
		{
			CL_CONTEXT_PLATFORM,
			(cl_context_properties)firstPlatformId,
			0
		};
		context = cl::Context(deviceType, contextProperties);
		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		queue = cl::CommandQueue(context, devices[0]);
		deviceInfo.setDeviceInfo(devices[0]());
		std::string options = "-cl-opt-disable -g -s \"../CleverAnt3/genShortTables.cl\"";
		//countRanges(options);
		boost::filesystem::path source("../CleverAnt3/genShortTables.cl");

		createProgram(source, options);
		initCLBuffers();
		logger << "[SUCCES] CCleverAnt3FitnesCL created.\n";
	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("Failed to create CCleverAnt3FitnesCL", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	} 
	catch (std::runtime_error& err)
	{
		throw std::runtime_error(std::string("Failed to create GeneticStrategy: ").append(
			err.what()));
	}
	cachedResults = new float[m_size];
}

void CCleverAnt3FitnesCL::setFromStrings(const std::vector< std::string >& strings)
{
	deviceType = CL_DEVICE_TYPE_CPU;
}

void CCleverAnt3FitnesCL::checkProgrmType(const std::string &source)
{
	int pos = source.find("#type=");
	if (pos == -1)
	{
		//if (automatType != FULL_TABLES)
			Tools::throwFailed("Type hint doesn't specified", &logger);
	}
	else
	{
		
	}
}

void CCleverAnt3FitnesCL::createProgram(const boost::filesystem::path& sourceFile, const std::string& params)
{
	// build the program from the source in the file
	std::string input;
	//Tools::StringProcessorSimple strProc(vals);
	Tools::readFileToString(input, sourceFile, &logger);
	checkProgrmType(input);

	cl::Program::Sources source;
	source.push_back(std::make_pair(input.c_str(), input.size()));
	cl::Program program(context, source);

	try { //compiling OpenCL source
		cl_int res = program.build(devices, params.c_str());
		logger << "[SUCCESS] Program compiled.\n";
	}
	catch (cl::Error) {
		std::stringstream ss;
		ss << program.getBuildInfo< CL_PROGRAM_BUILD_LOG >(devices[0]);
		throwDetailedFailed("File couldn't be compiled", ss.str().c_str(), &logger);
	}
	try { //Trying to create OpenCL kernel
		kernelGen = cl::Kernel(program, "genetic_1d");
		logger << "[SUCCESS] Kernel created.\n";
	}
	catch (cl::Error& err) {
		throwDetailedFailed("Failed to create kernel", streamsdk::getOpenCLErrorCodeStr(err.err()), &logger);
	}
}

void CCleverAnt3FitnesCL::initCLBuffers()
{
	statesBufCL1 = cl::Buffer(context, CL_MEM_READ_ONLY, m_size*sizeof(AUTOMAT) );

	sizesBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 5 * sizeof(int));
	resultCache = cl::Buffer(context, CL_MEM_READ_WRITE, m_size * sizeof(float));
}

void CCleverAnt3FitnesCL::setMaps(std::vector<CMapPtr> maps)
{
	CCleverAnt3Fitnes::setMaps( maps );

	size_t mapSize = (2 + getMap(0)->width()*getMap(0)->height());
	mapCL = cl::Buffer(context, CL_MEM_READ_ONLY, mapSize * 4);
	mapsBuffer = new int[mapSize];
	getMap(0)->toIntBuffer(mapsBuffer);

	mapBufCL = cl::Buffer(context, CL_MEM_READ_WRITE, m_size * mapSize * sizeof(int) );
}

void CCleverAnt3FitnesCL::fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result)
{
	m_size = individs.size();
	prepareData(individs);
	run();
	getData(result);
}

void CCleverAnt3FitnesCL::prepareData(const std::vector<AUTOMAT>& individs)
{
	//logger << "[INIT] Trying write buffer to the device\n";
	size_t mapSize = (2 + getMap(0)->width()*getMap(0)->height());
	size_t bufSize = sizeof(AUTOMAT)* individs.size();

	//__kernel void genetic_2d(__global uint* individs, __global const uint* constSizes,
		//__global int* mapBuffer, __constant int* maps, __global float* resultCache)
	try
	{
		kernelGen.setArg(0, statesBufCL1);
		kernelGen.setArg(1, sizesBuf);
		kernelGen.setArg(2, mapBufCL);
		kernelGen.setArg(3, mapCL);
		kernelGen.setArg(4, resultCache);

		//not used now, just compatibility
		size_t sizes[5];
		sizes[0] = individs[0].getAntCommon()->statesCount();
		sizes[1] = 1 << individs[0].getAntCommon()->statesCount();
		sizes[2] = sizeof(AUTOMAT)/4;
		sizes[3] = individs[0].getBufferOffset();
		sizes[4] = mapSize;

		const int* buf = ((const int*)&individs[0]) + individs[0].getBufferOffset();
		std::cout << "expected:" << buf[0] << " " << buf[0] << " " << buf[1] << " " << buf[2] << " " << buf[3]
			<< " " << buf[4] << " " << buf[5] << " " << buf[6] << " " << buf[7] << " " << std::endl;

		queue.enqueueWriteBuffer(statesBufCL1, CL_TRUE, 0, bufSize, &individs[0]);
		queue.enqueueWriteBuffer(mapCL, CL_TRUE, 0, mapSize * 4, mapsBuffer);

		queue.enqueueWriteBuffer(sizesBuf, CL_TRUE, 0, 5 * 4, sizes);

	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("[FAILED] to set arguments", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	}
	logger << "[SUCCESS] Buffers was wrtitten to the device\n";
}

void CCleverAnt3FitnesCL::run() const
{
	try
	{
		//queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NullRange );
		boost::this_thread::disable_interruption di;
		queue.enqueueNDRangeKernel(kernelGen, cl::NullRange, globalRange, localRange);
		queue.finish();
		queue.enqueueReadBuffer(resultCache, CL_TRUE, 0, m_size*sizeof(float), cachedResults);
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

void CCleverAnt3FitnesCL::getData(std::vector<ANT_FITNES_TYPE>& result) const
{
	for (size_t i = 0; i < m_size; ++i)
	{
		result[i] = cachedResults[i];
	}
}

ANT_FITNES_TYPE CCleverAnt3FitnesCL::fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const
{
	CMapPtr pMap(new CleverAnt3Map(maps[0]));
	return CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>::run(automat, pMap.get(), DEFAULT_STEPS_COUNT);
}
