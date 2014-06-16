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
#include <boost/format.hpp>
#include <algorithm>

#include "GeneticCommon/RandomImpl.h"

TestBuildRunner::~TestBuildRunner()
{
}

TestBuildRunner::TestBuildRunner(const std::string& clFileName, const std::string& xmlFileName, Tools::Logger& log)
:logger(log), m_size(128)
{
	logger << "[INIT] Initializing TestBuildRunner.\n";
	//setFromStrings(strings);


	deviceType = CL_DEVICE_TYPE_GPU;
	m_testReader.processFile(xmlFileName);
	m_size = m_testReader.getGeneticSettings().populationSize;
	
	m_automatBuffer.resize(m_size);
	m_cachedResultBuffer.resize(m_size);
	m_srandsBuffer.resize(m_size);

	globalRange = cl::NDRange(m_size);
	localRange = cl::NDRange(cl::NullRange);

	initKernel(clFileName, deviceType, &log);
}

void TestBuildRunner::createProgram(const boost::filesystem::path& sourceFile, const std::string& params)
{
	// build the program from the source in the file
	std::string input;
	//Tools::StringProcessorSimple strProc(vals);
	Tools::readFileToString(input, sourceFile);

	Tools::changeDefine(input, Tools::Define("MAX_TEST_INPUT", 
		boost::lexical_cast<std::string, size_t>(m_testReader.getMaxTestInputLength())));

	Tools::changeDefine(input, Tools::Define("MAX_TEST_OUTPUT",
		boost::lexical_cast<std::string, size_t>(m_testReader.getMaxTestOutputLength())));

	Tools::changeDefine(input, Tools::Define("TESTS_NUMBER",
		boost::lexical_cast<std::string, size_t>(m_testReader.getTestsCount())));


	Tools::changeDefine(input, Tools::Define("MAX_OUTPUTS",
		boost::lexical_cast<std::string, size_t>(m_testReader.getGeneticSettings().maxStateOutputCount)));

	Tools::changeDefine(input, Tools::Define("STATES_NUMBER",
		boost::lexical_cast<std::string, size_t>(m_testReader.getGeneticSettings().stateNumber)));

	Tools::changeDefine(input, Tools::Define("MAX_TRANSITIONS",
		boost::lexical_cast<std::string, size_t>(m_testReader.getGeneticSettings().maxStateTransitions)));

	Tools::changeDefine(input, Tools::Define("INPUTS_NUMBER",
		boost::lexical_cast<std::string, size_t>(m_testReader.getInputsCount())));

	Tools::changeDefine(input, Tools::Define("OUTPUTS_NUMBER",
		boost::lexical_cast<std::string, size_t>(m_testReader.getOutputsCount())));


	size_t mutationProb256 = m_testReader.getGeneticSettings().mutationProbability * 256;
	Tools::changeDefine(input, Tools::Define("MUTATION_THRESHOLD_256",
		boost::lexical_cast<std::string, size_t>(mutationProb256)));

	size_t mutationProb1024 = m_testReader.getGeneticSettings().mutationProbability * 1024;
	Tools::changeDefine(input, Tools::Define("MUTATION_THRESHOLD_1024",
		boost::lexical_cast<std::string, size_t>(mutationProb1024)));

	Tools::changeDefine(input, Tools::Define("MAX_LOCAL_SIZE",
		boost::lexical_cast<std::string, size_t>(256)));

	Tools::changeDefine(input, Tools::Define("GLOBAL_SIZE",
		boost::lexical_cast<std::string, size_t>(m_testReader.getGeneticSettings().populationSize)));

	size_t gv = m_testReader.getGeneticSettings().partStay * m_testReader.getGeneticSettings().populationSize;
	Tools::changeDefine(input, Tools::Define("GO_VALUE",
		boost::lexical_cast<std::string, size_t>(gv)));

	createProgramFromString(input, params);
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
#ifdef _DEBUG
	params.append(" -cl-opt-disable");
	// " -g -s \"../CleverAnt3/genShortTables.cl\"
#endif

	params.append(KernelRunner::getOptions());
	return params;
}

void TestBuildRunner::initCLBuffers()
{
	try
	{
		clSrandsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, m_size * sizeof(cl_uint));
		clResultCacheBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, m_size * sizeof(cl_float));

		clTestInfoBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, m_testReader.getTestInfosSize() );
		clTestsBuffer = cl::Buffer(context, CL_MEM_READ_ONLY, m_testReader.getTestsBufferSize());

		clAutomatBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, sizeof(TransitionListAutomat) * m_size);

		std::cout << boost::format("C++ :: sizeof(TransitionListAutomat)=%i") % sizeof(TransitionListAutomat) << std::endl;
		std::cout << boost::format("C++ :: sizeof(TestInfo)=%i") % sizeof(TestInfo) << std::endl;
	}
	catch (cl::Error& err)
	{
		std::string msg = "Failed to init buffers(last was ";
		throwDetailedFailed(msg, streamsdk::getOpenCLErrorCodeStr(err.err()), &logger);
	}
}

void TestBuildRunner::prepareData()
{
	CRandomImpl random(boost::chrono::system_clock::now().time_since_epoch().count());
	//CRandomImpl random(100500);
	for (size_t i = 0; i < m_size; ++i)
	{
		uint sr = random.nextUINT() * random.nextUINT() - i * random.nextUINT();
		m_srandsBuffer[i] = sr;
	}
	prepareFirstGeneration();

	try
	{
		kernelGen.setArg(0, clAutomatBuffer);
		kernelGen.setArg(1, clSrandsBuffer);
		kernelGen.setArg(2, clTestInfoBuffer);
		kernelGen.setArg(3, clTestsBuffer);
		kernelGen.setArg(4, clResultCacheBuffer);

		queue.enqueueWriteBuffer(clTestInfoBuffer, CL_TRUE, 0, m_testReader.getTestInfosSize(), m_testReader.getTestInfosPtr());
		queue.enqueueWriteBuffer(clTestsBuffer, CL_TRUE, 0, m_testReader.getTestsBufferSize(), m_testReader.getTestsBufferPtr());
		queue.enqueueWriteBuffer(clSrandsBuffer, CL_TRUE, 0, sizeof(cl_uint) * m_size, m_srandsBuffer.data());
		queue.enqueueWriteBuffer(clAutomatBuffer, CL_TRUE, 0, sizeof(TransitionListAutomat) * m_size, m_automatInitialBuffer.data());
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

void TestBuildRunner::getData(FITNES_TYPE* result)
{
	queue.enqueueReadBuffer(clResultCacheBuffer, CL_FALSE, 0, m_size*sizeof(cl_float), m_cachedResultBuffer.data());
	queue.enqueueReadBuffer(clAutomatBuffer, CL_FALSE, 0, sizeof(TransitionListAutomat)* m_size, m_automatBuffer.data());
	queue.finish();
	for (size_t i = 0; i < m_size; ++i)
	{
		result[i] = m_cachedResultBuffer[i];
	}
}

const GeneticSettings& TestBuildRunner::getGeneticSettings() const
{
	return m_testReader.getGeneticSettings();
}

const TestsReader& TestBuildRunner::getTestReader() const
{
	return m_testReader;
}

int TestBuildRunner::compareLabelling(const TransitionListAutomat& a, const TransitionListAutomat& b, const TestsReader& testReader)
{
	bool sameAutomats = true;
	sameAutomats = (a.firstState != b.firstState) ? false : sameAutomats;

/*#ifdef _DEBUG
	BOOST_ASSERT(a.firstState == b.firstState);
#endif*/

	int different = 0;
	for (size_t state = 0; state < testReader.getGeneticSettings().stateNumber; ++state)
	{
		sameAutomats = (a.states[state].count != b.states[state].count) ? false : sameAutomats;

/*#ifdef _DEBUG
		BOOST_ASSERT(a.states[state].count == b.states[state].count);
#endif*/

		for (size_t t = 0; t < a.states[state].count; ++t)
		{
			sameAutomats = (a.states[state].list[t].input != b.states[state].list[t].input) ? false : sameAutomats;
			sameAutomats = (a.states[state].list[t].outputsCount != b.states[state].list[t].outputsCount) ? false : sameAutomats;
			sameAutomats = (a.states[state].list[t].nextState != b.states[state].list[t].nextState) ? false : sameAutomats;

/*#ifdef _DEBUG
			BOOST_ASSERT(a.states[state].list[t].input == b.states[state].list[t].input);
			BOOST_ASSERT(a.states[state].list[t].outputsCount == b.states[state].list[t].outputsCount);
			BOOST_ASSERT(a.states[state].list[t].nextState == b.states[state].list[t].nextState);
#endif*/
			
			if ((a.states[state].list[t].obviousBest == 0) && (b.states[state].list[t].obviousBest == 0))
			{
				for (size_t k = 0; k < a.states[state].list[t].outputsCount; ++k)
				{
					if (a.states[state].list[t].outputs[k] != b.states[state].list[t].outputs[k])
						++different;
				}
			}
		}
	}
	if (!sameAutomats)
		different = -different - 1;
	return different;
}

void TestBuildRunner::transform(const TransitionListAutomat& automat, std::vector<int>& result, size_t testNumber, 
	const TestsReader& testReader)
{
	result.clear();
	int currentState = automat.firstState;
	for (size_t i = 0; i < testReader.getTestInfos()[testNumber].inputLength; ++i)
	{
		int s = testReader.getTestsBuffer()[testReader.getTestInfos()[testNumber].offset + i];
		bool found = false;
		for (size_t t = 0; t < automat.states[currentState].count; ++t) 
		{
			if (automat.states[currentState].list[t].input == s) 
			{
				for (size_t j = 0; j < automat.states[currentState].list[t].outputsCount; ++j)
				{
					result.push_back(automat.states[currentState].list[t].outputs[j]);
				}
				currentState = automat.states[currentState].list[t].nextState;
				found = true;
				break;
			}
		}
		if (!found) 
		{
			//result.clear();
			return;
		}
		size_t k = 0;
		++k;
	}
}

float TestBuildRunner::hammDist(const std::vector<int>& a, const std::vector<int>& b)
{
	float dist = 0.0;
	for (size_t i = 0; i < (std::min)(a.size(), b.size()); ++i)
	{
		if (a[i] != b[i])
			dist += 1.0;
	}
	dist += (std::max)(a.size(), b.size()) - (std::min)(a.size(), b.size());
	dist /= (std::max)(a.size(), b.size());
	return dist;
}

float TestBuildRunner::calcFitness(const TransitionListAutomat& automat, const TestsReader& testReader)
{
	float pSum = 0.0f;
	for (size_t test = 0; test < testReader.getTestsCount(); ++test) 
	{
		std::vector<int> output;
		transform(automat, output, test, testReader);
		std::vector<int> answer(testReader.getTestInfos()[test].outputLength);
		for (size_t i = 0; i < testReader.getTestInfos()[test].outputLength; ++i)
			answer[i] = testReader.getTestsBuffer()[testReader.getTestInfos()[test].offset +
				testReader.getTestInfos()[test].inputLength + i];
		
		double t;
		if (output.size() == 0) 
		{
			t = 1;
		}
		else 
		{
			t = hammDist(output, answer);
		}
		pSum += 1 - t;
	}
	return pSum/testReader.getTestsCount();
}

void TestBuildRunner::clearLabels(TransitionListAutomat& aut, const TestsReader& testReader)
{
	for (size_t state = 0; state < testReader.getGeneticSettings().stateNumber; ++state)
	{
		for (size_t t = 0; t < testReader.getGeneticSettings().maxStateTransitions; ++t)
		{
			for (size_t i = 0; i < testReader.getGeneticSettings().maxStateOutputCount; ++i)
			{
				aut.states[state].list[t].outputs[i] = 0;
			}
		}
	}
}

void TestBuildRunner::doLabelling(std::vector<TransitionListAutomat>& automats, const TestsReader& testReader)
{
	for (TransitionListAutomat& aut : automats)
	{
		clearLabels(aut, testReader);
		std::map<int, std::map<int, std::map<Sequence, int, seq_less> > > stateTransitionSequnceCounter;
		for (const TestInfo& test : testReader.getTestInfos())
		{
			int currentState = aut.firstState;

			int pOutput = 0;
			for (size_t i = 0; i < test.inputLength; ++i) 
			{
				int s = testReader.getTestsBuffer()[test.offset + i];
				boolean found = false;
				for (size_t t = 0; t < aut.states[currentState].count; ++t)
				{
					if (aut.states[currentState].list[t].input == s)
					{
						Sequence sequence;
						for (int j = 0; j < aut.states[currentState].list[t].outputsCount; j++) 
						{
							if (pOutput < test.outputLength) 
							{
								sequence.values[j] = testReader.getTestsBuffer()[test.offset + test.inputLength + pOutput];
								++pOutput;
							}
							else 
							{
								//wtf!?
								sequence.values[j] = testReader.getOutputsCount();
							}
						}
						
						stateTransitionSequnceCounter[currentState][t][sequence]++;
						//t.addOutputSequence(new Sequence(sequence));
						currentState = aut.states[currentState].list[t].nextState;
						found = true;
						break;
					}
				}
				if (!found) {
					break;
				}
			}
		}
		for (size_t state = 0; state < testReader.getGeneticSettings().stateNumber; ++state)
		{
			for (size_t t = 0; t < aut.states[state].count; ++t)
			{
				int c = 0;
				Sequence best;
				int same = 0;
				for (auto v : stateTransitionSequnceCounter[state][t])
				{
					if (v.second == c)
					{
						++same;
					}
					if (v.second > c)
					{
						best = v.first;
						c = v.second;
						same = 0;
					}
				}
				for (size_t k = 0; k < aut.states[state].list[t].outputsCount; ++k)
				{
					if (c > 0)
						aut.states[state].list[t].outputs[k] = best.values[k];
					else
						aut.states[state].list[t].outputs[k] = 0;
				}
				aut.states[state].list[t].obviousBest = 0;
				if ((same > 0) || (c==0))
					aut.states[state].list[t].obviousBest = 1;
			}
		}
	}
}

void TestBuildRunner::prepareFirstGeneration()
{
	//CRandomImpl random(boost::chrono::system_clock::now().time_since_epoch().count());
	CRandomImpl random(100500);
	int inputsCnt = m_testReader.getInputsCount();
	int outputsCnt = m_testReader.getOutputsCount();
	for (size_t ind = 0; ind < m_size; ++ind)
	{
		for (int i = 0; i < getGeneticSettings().stateNumber; i++)
		{
			std::vector<int> p(inputsCnt);
			for (int j = 0; j < inputsCnt; j++)
			{
				p[j] = j;
			}
			for (int j = inputsCnt - 1; j >= 1; j--)
			{
				int k = random.nextINT(j);
				int t = p[j];
				p[j] = p[k];
				p[k] = t;
			}

			int degree = random.nextINT(outputsCnt + 1);
			m_automatBuffer[ind].states[i].count = degree;

			for (int j = 0; j < degree; j++)
			{
				m_automatBuffer[ind].states[i].list[j].input = p[j];
				m_automatBuffer[ind].states[i].list[j].outputsCount = 1;
				m_automatBuffer[ind].states[i].list[j].nextState = random.nextINT(getGeneticSettings().stateNumber);
				m_automatBuffer[ind].states[i].list[j].obviousBest = 0;
			}
		}
		m_automatBuffer[ind].firstState = random.nextINT(getGeneticSettings().stateNumber);
	}
	m_automatInitialBuffer = m_automatBuffer;
}

const std::vector<TransitionListAutomat> TestBuildRunner::getCurrentAutomats() const
{
	return m_automatBuffer;
}

const std::vector<TransitionListAutomat> TestBuildRunner::getStartAutomats() const
{
	return m_automatInitialBuffer;
}