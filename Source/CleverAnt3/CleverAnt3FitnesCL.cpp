#include "CleverAnt3FitnesCL.h"
#include "CL/cl.hpp"
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/errorMsg.hpp"
#include "Tools/helper.hpp"
#include "CleverAnt3Map.h"
#include "GeneticCommon/Test.hpp"
#include <boost/spirit/include/qi.hpp>
#include "Tools/StringProcessor.h"
#include <boost/lexical_cast.hpp>
#include "performanceInfo.h"

CCleverAnt3FitnesCL::~CCleverAnt3FitnesCL()
{
	delete cachedResults;
	delete mapsBuffer;
}

CCleverAnt3FitnesCL::CCleverAnt3FitnesCL(const std::vector< std::string >& strings, Tools::Logger& log)
:logger(log), m_size(128), cachedResults(0), mapsBuffer(0)
{
	m_steps = 100;
	logger << "[INIT] Initializing CCleverAnt3FitnesCL.\n";
	setFromStrings(strings);
	bufSize = sizeof(AUTOMAT)* m_size;
	globalRange = cl::NDRange(m_size);
	localRange = cl::NDRange(256);
	//automatSize = commonDataSize + pAntCommon->statesCount() * stateSize;
	//stateSize = 16;//1 << statesCount;
	try
	{
		logger << "[INIT] Trying to get specified device.\n";

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
				//Tools::throwDetailedFailed("Failed to create CCleverAnt3FitnesCL", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
			}
			if (isCreated)
				break;
		}
		if (!isCreated)
			Tools::throwDetailedFailed("Failed to create CCleverAnt3FitnesCL", "No platform found for this request", &logger);

		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		queue = cl::CommandQueue(context, devices[0]);
		deviceInfo.setDeviceInfo(devices[0]());
		std::string options = getOptions();//"-cl-opt-disable -g -s \"../CleverAnt3/genShortTables.cl\"";
		//countRanges(options);
		boost::filesystem::path source(clFilePath);

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
	using namespace boost::spirit::qi;
	std::string deviceTypeStr;
	char c = 'C';
	for (size_t i = 0; i < strings.size(); ++i)
	{
		const std::string& str = strings[i];
		if (phrase_parse(str.begin(), str.end(), "GENERATION_SIZE=" >> int_ >> ";", space, m_size))
			continue;
		if (phrase_parse(str.begin(), str.end(), "STEPS_COUNT=" >> int_ >> ";", space, m_steps))
			continue;
		if (phrase_parse(str.begin(), str.end(), "DEVICE_TYPE=" >> char_ >> ";", space, c))
			continue;
	}
	deviceType = CL_DEVICE_TYPE_CPU;
	if (c == 'G')
		deviceType = CL_DEVICE_TYPE_GPU;
}

std::string CCleverAnt3FitnesCL::getInfo() const
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

std::string CCleverAnt3FitnesCL::getOptions() const
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

	using namespace boost::filesystem;
	path curPath = current_path();
	if (exists(curPath / "CleverAnt3"))
	{
		params.append(" -I ");
		params.append(curPath.string());
	}
	else
	{
		curPath = curPath / "../";
		if (exists(curPath / "CleverAnt3"))
		{
			params.append(" -I ");
			params.append(curPath.string());
		}
		else Tools::throwDetailedFailed("Can't compile openCL file", "Can't find folder CleverAnt3 with headers", &logger);
	}

	return params;
}

void CCleverAnt3FitnesCL::createProgram(const boost::filesystem::path& sourceFile, const std::string& params)
{
	// build the program from the source in the file
	std::string input;
	//Tools::StringProcessorSimple strProc(vals);
	Tools::readFileToString(input, sourceFile, &logger);

	Tools::changeDefine(input, Tools::Define("STEPS_COUNT", boost::lexical_cast<std::string, size_t>(m_steps)));

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

	mapSize = (2 + getMap(0)->width()*getMap(0)->height());
	mapCL = cl::Buffer(context, CL_MEM_READ_ONLY, mapSize * 4);
	mapsBuffer = new int[mapSize * m_size];
	int* oneMap = new int[mapSize];
	getMap(0)->toIntBuffer(oneMap);

	mapBufCL = cl::Buffer(context, CL_MEM_READ_WRITE, m_size * mapSize * sizeof(int) );
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

void CCleverAnt3FitnesCL::fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result)
{
	/*CCleverAnt3FitnesCPU fitCPU(m_steps);
	fitCPU.setMaps(maps);
	fitCPU.fitnes(&individs[0]);*/

	//m_size = individs.size();
	CTimeCounter counter(perfFitnesFunction);
	if (m_size != individs.size())
		Tools::throwFailed("Different sizes!!!", &logger);
	prepareData(individs.data());
	run();
	getData(result.data());
}

void CCleverAnt3FitnesCL::fitnes(const AUTOMAT* individs, ANT_FITNES_TYPE* result)
{
	//if (m_size != individs.size())
	//	Tools::throwFailed("Different sizes!!!", &logger);
	prepareData(individs);
	run();
	getData(result);
}

void CCleverAnt3FitnesCL::prepareData(const AUTOMAT* individs)
{
	try
	{
		queue.enqueueWriteBuffer(statesBufCL1, CL_FALSE, 0, bufSize, individs);
		queue.enqueueCopyBuffer(mapBufConst, mapBufCL, 0, 0, m_size * mapSize * sizeof(int));
	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("[FAILED] to set arguments", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	}
	//logger << "[SUCCESS] Buffers was wrtitten to the device\n";
}

void CCleverAnt3FitnesCL::run() const
{
	try
	{
		//queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NullRange );
		boost::this_thread::disable_interruption di;
		queue.finish();
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

void CCleverAnt3FitnesCL::getData(ANT_FITNES_TYPE* result) const
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
