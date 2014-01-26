#include "stdafx.h"
#include "CGeneticStrategyCLv3.h"
#include <iostream>
#include <sstream>
#include "SDKUtil/include/SDKCommon.hpp"
#include "CAutomatImpl.h"
#include "GeneticCommon/AutomatShortTables.h"
#include "CTest.h"
#include "CRandomImpl.h"
#include <boost/algorithm/string/predicate.hpp>
#include "Tools/helper.hpp"
#include "Tools/StringProcessorSimple.h"
#include <sstream>

size_t CGeneticStrategyCLv3::commonDataSize = 4;
size_t CGeneticStrategyCLv3::maskSize = INPUT_PARAMS_COUNT;
size_t CGeneticStrategyCLv3::tableSize = 2 * (1 << SHORT_TABLE_COLUMNS);
size_t CGeneticStrategyCLv3::stateSize = tableSize + maskSize;

void CGeneticStrategyCLv3::initMemory()
{
	sizes = (unsigned int*)malloc(5 * sizeof(int));
	srands = (unsigned int*)malloc(5 * sizeof(int));
}

void CGeneticStrategyCLv3::initCLBuffers()
{
	size_t bufSize = automatSize * N * M;
	buffer = (COUNTERS_TYPE*)malloc(bufSize);
	buffer2 = (COUNTERS_TYPE*)malloc(bufSize);

	CRandomImpl rand;
	for (int i = 0; i < N * M; ++i)
	{
		COUNTERS_TYPE* buf = buffer + i * automatSize;
		switch (automatType)
		{
		case(FULL_TABLES):
			CAutomatImpl::fillRandom(pAntCommon.get(), buf, &rand);
			break;
		case(SHORT_TABLES) :
			CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::fillRandom(pAntCommon.get(), buf, &rand);
			break;
		default:
			Tools::throwFailed("Decisin trees not implemented yet", &logger);
		}
		
	}

	statesBufCL1 = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
	statesBufCL2 = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
	globalTmpBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);

	srandBuf = cl::Buffer(context, CL_MEM_READ_WRITE, 5 * sizeof(int));
	sizesBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 5 * sizeof(int));
	resultCache = cl::Buffer(context, CL_MEM_READ_WRITE, 2 * N*M*sizeof(float));
	cachedResults = (float*)malloc(2 * N*M*sizeof(float));
}

void CGeneticStrategyCLv3::checkProgrmType(const std::string &source)
{
	int pos = source.find("#type=");
	if (pos == -1)
	{
		if (automatType != FULL_TABLES)
			Tools::throwFailed("Type hint doesn't specified", &logger);
	}
	else
	{
		char c = source[pos + 6];
		if (c == 's')
			if (automatType != SHORT_TABLES)
				Tools::throwFailed("Type in OpenCL sorce file doesn't match Configure file", &logger);
		if (c == 'f')
			if (automatType != FULL_TABLES)
				Tools::throwFailed("Type in OpenCL sorce file doesn't match Configure file", &logger);
		if (c == 'd')
			if (automatType != DECISION_TREE)
				Tools::throwFailed("Type in OpenCL sorce file doesn't match Configure file", &logger);
	}
}

void CGeneticStrategyCLv3::createProgram(const boost::filesystem::path& sourceFile, const std::string& params)
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
		kernelGen = cl::Kernel(program, "genetic_2d");
		logger << "[SUCCESS] Kernel created.\n";
	}
	catch (cl::Error& err) {
		throwDetailedFailed("Failed to create kernel", streamsdk::getOpenCLErrorCodeStr(err.err()), &logger);
	}
}

void CGeneticStrategyCLv3::countRanges(std::string& options)
{
	uint val;
	devices[0].getInfo(CL_DEVICE_MAX_WORK_GROUP_SIZE, &val);
	cl_ulong lMemSize;
	devices[0].getInfo(CL_DEVICE_LOCAL_MEM_SIZE, &lMemSize);
	cl_ulong autSize = 4 + 2 * 16 * pAntCommon->statesCount();
	cl_ulong nMemSize = val * autSize;
	localRange = cl::NullRange;
	globalRange = cl::NDRange(N, M);
	int v_work_group_size = val;
	cl_ulong v_buffer = val*(autSize / 4);
	std::stringstream ss(std::stringstream::out);
	if (lMemSize < nMemSize)
	{
		/* cl_ulong res = lMemSize / autSize;
		std::vector<int> Ndels, Mdels;
		for ( int i=1; i<=(std::max)( N, M )/2; ++i )
		{
		if ( N % i == 0 )
		Ndels.push_back(i);
		if ( M % i == 0 )
		Mdels.push_back(i);
		}
		int maxA = 1, maxB = 1;
		for ( size_t i=0; i<Ndels.size(); ++i )
		for ( size_t j=0; j<Mdels.size(); ++j )
		{
		int a = Ndels[i], b = Mdels[j];
		if ( (2*a + a*b*(1 + autSize/4))*4 < lMemSize )
		{
		if ( a*b > maxA*maxB )
		{
		maxA = a;
		maxB = b;
		}else if ( a*b == maxA*maxB && std::abs(a - b) < std::abs( maxA - maxB ) )
		{
		maxA = a;
		maxB = b;
		}
		}
		}
		localRange = cl::NDRange( maxA, maxB );
		v_linear = maxA;
		v_squared = maxA * maxB;*/
		//v_buffer = maxA*maxB*(size_t)(autSize / 4);
		ss << " -D __check_space=__global";
	}
	else
	{
		ss << " -D ENOUGH_LOCAL_MEMORY";
		ss << " -D __check_space=__local";
	}

	ss << " -D WORK_GROUP_SIZE=" << v_work_group_size;
	ss << " -D BUFFER_SIZE=" << v_buffer;
	if (deviceType == CL_DEVICE_TYPE_CPU)
		ss << " -D CPU_VERSION";
	options = ss.str();
}

CGeneticStrategyCLv3::CGeneticStrategyCLv3(const boost::filesystem::path& source, CAntCommonPtr<COUNTERS_TYPE> pAntCommon,
	CLabResultMulti* res, CAntFitnesPtr fitnes, const std::vector< std::string >& strings, Tools::Logger& logger)
	:CGeneticStrategyCommon(pAntCommon, res, fitnes, strings, logger), mapsBuffer(0), buffer(0)
{
	logger << "[INIT] Initializing CGeneticStrategyCLv2.\n";
	m_pRandom = CRandomPtr(new CRandomImpl());
	setFromStrings(strings, m_pRandom);
	automatSize = commonDataSize + pAntCommon->statesCount() * stateSize;
	//stateSize = 16;//1 << statesCount;
	initMemory();
	try
	{
		logger << "[INIT] Trying to get specified device.\n";
		context = cl::Context(deviceType, 0);
		devices = context.getInfo<CL_CONTEXT_DEVICES>();
		queue = cl::CommandQueue(context, devices[0]);
		deviceInfo.setDeviceInfo(devices[0]());
		std::string options;
		countRanges(options);

		createProgram(source, options);
		initCLBuffers();
		logger << "[SUCCES] CGeneticStrategyCLv2 created.\n";
	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("Failed to create CGeneticStrategyCLv2", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	}
	catch (std::runtime_error& err)
	{
		throw std::runtime_error(std::string("Failed to create GeneticStrategy: ").append(
			err.what()));
	}
}

void CGeneticStrategyCLv3::setFromStrings(const std::vector< std::string >& strings, CRandomPtr rand)
{
	gensToCount = 1;
	M = N = 0;
	deviceType = CL_DEVICE_TYPE_GPU;
	automatType = FULL_TABLES;
	for (size_t i = 0; i < strings.size(); ++i)
	{
		const std::string& str = strings[i];
		if (boost::starts_with(str, "gensCounter"))
		{
			int b = str.find("=");
			++b;
			int e = str.find(";");
			const std::string tmp(str.substr(b, e));
			gensToCount = atoi(tmp.c_str());
			continue;
		}

		if (boost::starts_with(str, "M"))
		{
			int b = str.find("=");
			++b;
			int e = str.find(";");
			const std::string tmp(str.substr(b, e));
			M = atoi(tmp.c_str());
			continue;
		}
		if (boost::starts_with(str, "N"))
		{
			int b = str.find("=");
			++b;
			int e = str.find(";");
			const std::string tmp(str.substr(b, e));
			N = atoi(tmp.c_str());
			continue;
		}
		if (boost::starts_with(str, "device"))
		{
			int b = str.find("=");
			++b;
			int e = str.find(";");
			const std::string tmp(str.substr(b, e));
			if (tmp[0] == 'C')
				deviceType = CL_DEVICE_TYPE_CPU;
			continue;
		}
		if (boost::starts_with(str, "transition"))
		{
			int b = str.find("=");
			++b;
			int e = str.find(";");
			const std::string type(str.substr(b, e));

			if (boost::starts_with(type, "Short") || boost::starts_with(type, "short"))
			{
				automatType = SHORT_TABLES;
			}
			else
			if (boost::starts_with(type, "Decision") || boost::starts_with(type, "decision"))
			{
				BOOST_ASSERT_MSG(0, "Decision trees not implemented yet, use full tables instead");
				automatType = DECISION_TREE;
			}
			else
			if (boost::starts_with(type, "Full") || boost::starts_with(type, "full"))
			{
				automatType = FULL_TABLES;
			}
		}
	}
	invoker = new CInvoker(this, error);
	if ((M <= 0) || (N <= 0))
		Tools::throwFailed("Failed to create CGeneticStrategyCLv3", &logger);
}

void CGeneticStrategyCLv3::preStart()
{
	logger << "[INIT] Trying write buffer to the device\n";
	size_t mapSize = (2 + getMap(0)->width()*getMap(0)->height());
	size_t bufSize = automatSize * N * M;

	try
	{
		kernelGen.setArg(0, statesBufCL1);
		kernelGen.setArg(1, statesBufCL2);
		kernelGen.setArg(2, sizesBuf);
		kernelGen.setArg(3, srandBuf);
		kernelGen.setArg(4, mapBufCL);
		kernelGen.setArg(5, mapCL);
		kernelGen.setArg(6, resultCache);
		kernelGen.setArg(7, globalTmpBuffer);

		//not used now, just compatibility
		sizes[0] = pAntCommon->statesCount();
		sizes[1] = 1 << pAntCommon->statesCount();
		sizes[3] = gensToCount;

		CRandomImpl random;
		srands[0] = random.nextUINT();
		srands[1] = 0;

		queue.enqueueWriteBuffer(statesBufCL1, CL_TRUE, 0, bufSize, buffer);
		queue.enqueueWriteBuffer(mapCL, CL_TRUE, 0, mapSize * 4, mapsBuffer);
		queue.enqueueWriteBuffer(sizesBuf, CL_TRUE, 0, 5 * 4, sizes);
		queue.enqueueWriteBuffer(srandBuf, CL_TRUE, 0, 5 * 4, srands);

	}
	catch (cl::Error& error)
	{
		Tools::throwDetailedFailed("[FAILED] to set arguments", streamsdk::getOpenCLErrorCodeStr(error.err()), &logger);
	}
	logger << "[SUCCESS] Buffers was wrtitten to the device\n";
}

void CGeneticStrategyCLv3::setMaps(std::vector<CMapPtr> maps)
{
	getFitnesFunctor()->setMaps(maps);

	size_t mapSize = (2 + getMap(0)->width()*getMap(0)->height());
	size_t mapSizes = N * M * mapSize;
	mapBufCL = cl::Buffer(context, CL_MEM_READ_WRITE, mapSizes * 4);
	mapCL = cl::Buffer(context, CL_MEM_READ_ONLY, mapSize * 4);
	mapsBuffer = (int*)malloc(mapSize * 4);
	getMap(0)->toIntBuffer(mapsBuffer);
	sizes[2] = mapSize;

	preStart();
}

void CGeneticStrategyCLv3::run()
{
	nextGeneration(m_pRandom.get());
}

void CGeneticStrategyCLv3::nextGeneration(CRandom* rand)
{
	size_t bufSize = automatSize * N * M;
	size_t cacheSize = N*M*sizeof(float);
	try
	{
		//queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NullRange );
		boost::this_thread::disable_interruption di;
		queue.enqueueNDRangeKernel(kernelGen, cl::NullRange, globalRange, localRange);

		queue.finish();

		queue.enqueueReadBuffer(statesBufCL2, CL_FALSE, 0, bufSize, buffer);
		queue.enqueueReadBuffer(statesBufCL1, CL_FALSE, 0, bufSize, buffer2);
		queue.enqueueReadBuffer(resultCache, CL_FALSE, 0, 2 * cacheSize, cachedResults);
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

	//ToDo: count buffer, buffer2
	boost::mutex& mutex = result->getMutex();
	boost::mutex::scoped_lock lock(mutex);
	queue.finish();
	addGeneration(buffer2, cachedResults);
	addGeneration(buffer, cachedResults + N*M);
}

void CGeneticStrategyCLv3::addGeneration(char* buff, float* results)
{
	double sum = 0.0;
	size_t bestPos = 0;
	for (int i = 0; i < N; ++i)
	for (int j = 0; j < M; ++j)
	{
		sum += results[i*M + j];
		if (results[i*M + j] > results[bestPos])
			bestPos = i*M + j;
	}
	switch (automatType)
	{
	case(FULL_TABLES) :
		result->addGeneration(CAutomatImpl::createFromBuffer(pAntCommon.get(), buff + bestPos*automatSize)
			, results[bestPos], sum / (N*M));
		break;
	case(SHORT_TABLES) :
		result->addGeneration(CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, SHORT_TABLE_COLUMNS, INPUT_PARAMS_COUNT>::createFromBuffer(pAntCommon.get(), buff + bestPos*automatSize)
		, results[bestPos], sum / (N*M));
		break;
	default:
		Tools::throwFailed("Decisin trees not implemented yet", &logger);
	}
	
}

CGeneticStrategyCLv3::~CGeneticStrategyCLv3()
{
	if (buffer != 0)
		free(buffer);
	if (buffer2 != 0)
		free(buffer2);
	free(mapsBuffer);
	free(cachedResults);
	free(sizes);
	free(srands);
	if (invoker != 0)
		delete invoker;
}

std::string CGeneticStrategyCLv3::getDeviceType() const
{
	std::string res;
	if (deviceType == CL_DEVICE_TYPE_CPU)
		res = "OpenCL on CPU, ";
	else
		res = "OpenCL on GPU, ";
	res.append(deviceInfo.name);
	return res;
}
