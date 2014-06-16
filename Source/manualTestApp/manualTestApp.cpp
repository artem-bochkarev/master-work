// manualTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GeneticTests/GeneticChecks.h"
#include "GeneticTests/MFETestRunner.h"
#include "TimeRun/CTimeCounter.h"
#include <conio.h>
#include "GeneticCommon/RandomImpl.h"
#include <boost/format.hpp>
#include "GeneticTests/TestBuildRunner.h"

static const char* genCheckFileName = "../GeneticTests/elitismTest.cl";
static const char* genCheckFileNameLocal = "../GeneticTests/elitismTestLocal.cl";
static const char* genCheckFileNameRandomized = "../GeneticTests/elitismTestRandomized.cl";
static const char* mfeFileName = "../GeneticTests/mfeTest.cl";

static const std::string counterElitismGlobal = "TrueElitismGlobal";
static const std::string counterElitismLocal = "TrueElitismLocal";
static const std::string sortTime = "SortTimeCPU";

static const std::string clockFile = "../GeneticTests/clock.xml";
static const std::string liftFile = "../GeneticTests/lift.xml";
static const std::string scenariosBuildFile = "../GeneticTests/scenariosBuild.cl";


void checkElitism(const char* fileName, size_t size, size_t count, const cl_float* data, cl_uint* out)
{
	GeneticChecks genCheck(fileName, size, count);
	genCheck.prepareData(data);

	std::string str = "Counter(" + std::string(fileName) + ")";
	CTimeCounter counter1(str);
	genCheck.run();
	counter1.stop();

	genCheck.getData(out);
}

void compareElitismToTrue(size_t size, size_t count, const cl_float* data, const cl_uint* out, const cl_uint* trueOut)
{
	size_t cntTrue = 0;
	cl_float worstTakenValue = 100500;
	for (size_t i = 0; i < size; ++i)
	{
		if (trueOut[i])
		{
			cntTrue++;
			if (data[i] < worstTakenValue)
				worstTakenValue = data[i];
		}
	}
	BOOST_ASSERT(cntTrue == count);

	size_t cntTaken = 0;
	size_t cntBadTaken = 0;
	size_t diff = 0;
	for (size_t i = 0; i < size; ++i)
	{
		if (out[i])
		{
			cntTaken++;
			if (data[i] < worstTakenValue)
				cntBadTaken++;
		}
		if (out[i] != trueOut[i])
		{
			diff++;
		}
	}
	double badPercent = cntBadTaken*100.0 / cntTaken;
	size_t a = badPercent;
	size_t b = (badPercent - a) * 100;
	std::cout << boost::format("\tBad percent: %.2f") % badPercent << "%" << std::endl;
	std::cout << boost::format("\tTaken count: %i") % cntTaken << std::endl;
	std::cout << boost::format("\tDifference: %i") % diff << std::endl;
}

void compareMFE(size_t genSize, size_t checkCount, const cl_float* data, const cl_float* out, const cl_float* trueOut)
{
	size_t same = 0;
	for (size_t i = 0; i<genSize; ++i)
	{
		std::map<float, int> hmap;
		for (size_t j = 0; j < checkCount; ++j)
		{
			size_t index = (i + j) % genSize;
			if (hmap.find(data[index]) != hmap.end()) {
				hmap[data[index]]++;
			}
			else
			{
				hmap[data[index]] = 1;
			}
		}
		float max = -100500.0f;
		int maxCounter = 0;
		for (auto t : hmap)
		{
			if (t.second > maxCounter)
			{
				maxCounter = t.second;
				max = t.first;
			}
		}
		if (hmap.find(out[i]) != hmap.end())
		{
			if (maxCounter == hmap[out[i]])
				++same;
		}
	}

	double percent = same*100.0 / genSize;
	std::cout << boost::format("\tEquality percent: %.2f") % percent << "%" << std::endl;
}

void checkMFE(const char* fileName, size_t size, size_t checkCount, size_t mfeSize, const cl_float* data, cl_float* out)
{
	MFETestRunner runner(fileName, size, checkCount, mfeSize);
	runner.prepareData(data);

	std::stringstream ss;
	ss << "Counter(" << fileName << boost::format("[%i, %i, %i]") % size % checkCount % mfeSize << ")";
	CTimeCounter counter1(ss.str());
	runner.run();
	counter1.stop();

	runner.getData(out);
}

static const size_t genSize = 1024;
static const size_t eliteCount = 16;

int _tmain(int argc, _TCHAR* argv[])
{
	GetTimeManager().clean();

	cl_float data[genSize];
	cl_uint outInt[genSize];
	cl_float outFloat[genSize];
	cl_float outFloatTrue[genSize];
	cl_uint outTrue[genSize];

	CRandomImpl random;

	for (size_t i = 0; i < genSize; ++i)
	{
		data[i] = random.nextUINT(10);
	}

	/*checkIt(genCheckFileName, genSize, eliteCount, data, outTrue);

	checkIt(genCheckFileNameLocal, genSize, eliteCount, data, out);
	std::cout << "Result for " << genCheckFileNameLocal << std::endl;
	compareToTrue(genSize, eliteCount, data, out, outTrue);

	checkIt(genCheckFileNameRandomized, genSize, 1, data, out);
	std::cout << "Result for " << genCheckFileNameRandomized << std::endl;
	compareToTrue(genSize, eliteCount, data, out, outTrue);



	std::vector<float> floats(genSize);
	for (int i = 0; i < genSize; ++i)
	{
	floats[i] = data[i];
	}

	CTimeCounter counter2(sortTime);
	std::sort(floats.begin(), floats.end());
	counter2.stop();*/
	/*size_t checkCount = 16;
	size_t mfeSize = 4;

	CTimeCounter counterMFETrue(sortTime);
	for (size_t i = 0; i<genSize; ++i)
	{
	std::map<float, int> hmap;
	for (size_t j = 0; j < checkCount; ++j)
	{
	size_t index = (i + j) % genSize;
	if (hmap.find(data[index]) != hmap.end()) {
	hmap[data[index]]++;
	}
	else
	{
	hmap[data[index]] = 1;
	}
	}
	float max = -100500.0f;
	int maxCounter = 0;
	for (auto t : hmap)
	{
	if (t.second > maxCounter)
	{
	maxCounter = t.second;
	max = t.first;
	}
	}
	outFloatTrue[i] = max;
	}
	counterMFETrue.stop();

	checkMFE(mfeFileName, genSize, checkCount, mfeSize, data, outFloat);
	std::cout << "Result for " << genCheckFileNameRandomized << std::endl;
	compareMFE(genSize, checkCount, data, outFloat, outFloatTrue);*/

	Tools::Logger logger;

	/*TestBuildRunner testBuildRunner(scenariosBuildFile, clockFile, logger);
	std::vector<cl_float> resultsCL(testBuildRunner.getGeneticSettings().populationSize);
	std::vector<cl_float> resultsCPP(testBuildRunner.getGeneticSettings().populationSize);
	testBuildRunner.prepareData();
	std::vector<TransitionListAutomat> tmp = testBuildRunner.getStartAutomats();
	TestBuildRunner::compareLabelling(tmp[0], testBuildRunner.getCurrentAutomats()[0], testBuildRunner.getTestReader());

	CTimeCounter counterCLRun("OpenCL: labelling + calcFitness + newGen(calc, fitness)");
	testBuildRunner.run();
	counterCLRun.stop();
	testBuildRunner.getData(resultsCL.data());

	CTimeCounter counterCPPLabelling("C++: labelling + calcFitness");
	TestBuildRunner::doLabelling(tmp, testBuildRunner.getTestReader());
	for (size_t i = 0; i < testBuildRunner.getGeneticSettings().populationSize; ++i)
	{
		resultsCPP[i] = TestBuildRunner::calcFitness(tmp[i], testBuildRunner.getTestReader());
	}
	counterCPPLabelling.stop();

	int sum = 0;
	int sum2 = 0;
	int diff = 0;
	int diffFitn = 0;
	int notNullres = 0;
	int differentAutomats = 0;

	for (size_t i = 0; i < testBuildRunner.getGeneticSettings().populationSize; ++i)
	{
		int k = TestBuildRunner::compareLabelling(tmp[i], testBuildRunner.getCurrentAutomats()[i], testBuildRunner.getTestReader());
		if (k < 0)
		{
			++differentAutomats;
			sum += std::abs(k) - 1;
			if (k<-1)
				diff++;
		}
		else if (k > 0)
		{
			sum += k;
			diff++;
		}

		if (std::abs(resultsCL[i] - resultsCPP[i]) > 0.0001f)
			diffFitn++;
		if (resultsCPP[i] > 0.0001f)
			notNullres++;
	}
	double c1 = sum;
	c1 /= testBuildRunner.getGeneticSettings().populationSize;
	double c2 = sum;
	c2 /= diff;

	std::cout << boost::format("\tNot the same by Transitions=%i") % differentAutomats << std::endl;
	std::cout << boost::format("\tNot the same by Labels=%i \n avgDiff=%.2f \n avgDiff2=%.2f") % diff % c1 % c2 << std::endl;
	std::cout << boost::format("\tNot the same by Fitness=%i") % diffFitn << std::endl;
	std::cout << boost::format("\tNot null results count=%i") % notNullres << std::endl;
	*/

	TestBuildRunner testBuildRunner(scenariosBuildFile, clockFile, logger);
	std::vector<cl_float> resultsCL(testBuildRunner.getGeneticSettings().populationSize);
	std::vector<cl_float> resultsCPP(testBuildRunner.getGeneticSettings().populationSize);
	testBuildRunner.prepareData();

	size_t steps = 10;
	CTimeCounter counterCLRun("ManyGenerations");
	for (size_t i = 0; i<steps; ++i)
	{
		testBuildRunner.run();
		//TestBuildRunner::compareLabelling(tmp[0], testBuildRunner.getCurrentAutomats()[0], testBuildRunner.getTestReader());
		/*testBuildRunner.getData(resultsCL.data());
		std::vector<TransitionListAutomat> tmp = testBuildRunner.getCurrentAutomats();

		TestBuildRunner::doLabelling(tmp, testBuildRunner.getTestReader());
		for (size_t i = 0; i < testBuildRunner.getGeneticSettings().populationSize; ++i)
		{
			resultsCPP[i] = TestBuildRunner::calcFitness(tmp[i], testBuildRunner.getTestReader());
		}

		int sum = 0;
		int sum2 = 0;
		int diff = 0;
		int diffFitn = 0;
		int notNullres = 0;
		int differentAutomats = 0;

		int bestFitnessCPP = 0;
		double sumFitnessCPP = 0.0;
		int bestFitnessCL = 0;
		double sumFitnessCL = 0.0;
		double fitnessDiff = 0.0;

		for (size_t i = 0; i < testBuildRunner.getGeneticSettings().populationSize; ++i)
		{*/
			/*int k = TestBuildRunner::compareLabelling(tmp[i], testBuildRunner.getCurrentAutomats()[i], testBuildRunner.getTestReader());
			if (k < 0)
			{
				++differentAutomats;
				sum += std::abs(k) - 1;
				if (k<-1)
					diff++;
			}
			else if (k > 0)
			{
				sum += k;
				diff++;
			}*/

			/*if (std::abs(resultsCL[i] - resultsCPP[i]) > 0.0001f)
			{
				diffFitn++;
				fitnessDiff += std::abs(resultsCL[i] - resultsCPP[i]);
			}
				
			sumFitnessCPP += resultsCPP[i];
			if (resultsCPP[i] > resultsCPP[bestFitnessCPP])
				bestFitnessCPP = i;

			sumFitnessCL += resultsCL[i];
			if (resultsCL[i] > resultsCL[bestFitnessCL])
				bestFitnessCL = i;

			if (resultsCPP[i] > 0.0001f)
				notNullres++;
		}

		std::cout << "Step: " << i << std::endl;;
		std::cout << boost::format("\tCPP   : best=%.4f   avg=%.4f") % resultsCPP[bestFitnessCPP] % (sumFitnessCPP / testBuildRunner.getGeneticSettings().populationSize) << std::endl;
		std::cout << boost::format("\tOpenCL: best=%.4f   avg=%.4f") % resultsCL[bestFitnessCL] % (sumFitnessCL / testBuildRunner.getGeneticSettings().populationSize) << std::endl;
		//std::cout << boost::format("\tNot the same by Transitions=%i") % differentAutomats << std::endl;
		//std::cout << boost::format("\tNot the same by Labels=%i \n avgDiff=%.2f \n avgDiff2=%.2f") % diff % c1 % c2 << std::endl;
		std::cout << boost::format("\tNot the same by Fitness=%i") % diffFitn << std::endl;
		std::cout << boost::format("\tFitness absolute difference=%.4f") % fitnessDiff << std::endl;
		std::cout << boost::format("\tNot null results count=%i") % notNullres << std::endl;*/
		std::cout << "Step: " << 10*i << std::endl;
		//std::cout << "Step: " << 2*i + 1 << std::endl;
	}
	counterCLRun.stop();

	testBuildRunner.getData(resultsCL.data());
	int bestFitnessCL = 0;
	double sumFitnessCL = 0.0;
	for (size_t i = 0; i < testBuildRunner.getGeneticSettings().populationSize; ++i)
	{
		/*if (std::abs(resultsCL[i] - resultsCPP[i]) > 0.0001f)
		{
		diffFitn++;
		fitnessDiff += std::abs(resultsCL[i] - resultsCPP[i]);
		}

		sumFitnessCPP += resultsCPP[i];
		if (resultsCPP[i] > resultsCPP[bestFitnessCPP])
		bestFitnessCPP = i;
		*/
		sumFitnessCL += resultsCL[i];
		if (resultsCL[i] > resultsCL[bestFitnessCL])
			bestFitnessCL = i;

		/*if (resultsCPP[i] > 0.0001f)
		notNullres++;*/
	}

	std::cout << "Step: " << steps << " Generations:" << 2*steps << std::endl;;
	std::cout << boost::format("\tOpenCL: best=%.4f   avg=%.4f") % resultsCL[bestFitnessCL] % (sumFitnessCL / testBuildRunner.getGeneticSettings().populationSize) << std::endl;

	for (std::map<std::string, TimerData>::value_type val : GetTimeManager().getTimers())
	{
		boost::chrono::nanoseconds nano = val.second.duration;
		if (nano.count() > 10000)
		{
			boost::chrono::microseconds mcs = boost::chrono::duration_cast<boost::chrono::microseconds>(val.second.duration);
			if (mcs.count() > 10000)
			{
				boost::chrono::milliseconds mls = boost::chrono::duration_cast<boost::chrono::milliseconds>(val.second.duration);
				std::cout << val.first << " Time: " << mls << std::endl;
			}
			else
			{
				std::cout << val.first << " Time: " << mcs << std::endl;
			}
		}
		else
		{
			std::cout << val.first << "Time: " << nano << std::endl;
		}
	}
	
	
	_getch();
	return 0;
}

