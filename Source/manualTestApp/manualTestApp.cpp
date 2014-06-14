// manualTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GeneticTests/GeneticChecks.h"
#include "TimeRun/CTimeCounter.h"
#include <conio.h>
#include "GeneticCommon/RandomImpl.h"
#include <boost/format.hpp>

static const char* genCheckFileName = "../GeneticTests/elitismTest.cl";
static const char* genCheckFileNameLocal = "../GeneticTests/elitismTestLocal.cl";
static const char* genCheckFileNameRandomized = "../GeneticTests/elitismTestRandomized.cl";

static const std::string counterElitismGlobal = "TrueElitismGlobal";
static const std::string counterElitismLocal = "TrueElitismLocal";
static const std::string sortTime = "SortTimeCPU";

void checkIt(const char* fileName, size_t size, size_t count, const cl_float* data, cl_uint* out)
{
	GeneticChecks genCheck(fileName, size, count);
	genCheck.prepareData(data);

	std::string str = "Counter(" + std::string(fileName) + ")";
	CTimeCounter counter1(str);
	genCheck.run();
	counter1.stop();

	genCheck.getData(out);
}

void compareToTrue(size_t size, size_t count, const cl_float* data, const cl_uint* out, const cl_uint* trueOut)
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

static const size_t genSize = 1024;
static const size_t eliteCount = 16;

int _tmain(int argc, _TCHAR* argv[])
{
	GetTimeManager().clean();

	cl_float data[genSize];
	cl_uint out[genSize];
	cl_uint outTrue[genSize];

	CRandomImpl random;

	for (size_t i = 0; i < genSize; ++i)
	{
		data[i] = random.nextUINT(60);
	}

	checkIt(genCheckFileName, genSize, eliteCount, data, outTrue);
	
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
	counter2.stop();

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

