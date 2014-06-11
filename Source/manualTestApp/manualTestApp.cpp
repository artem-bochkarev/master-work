// manualTestApp.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "GeneticTests/GeneticChecks.h"
#include "TimeRun/CTimeCounter.h"
#include <conio.h>

static const char* genCheckFileName = "../GeneticTests/elitismTest.cl";
static const size_t genSize = 1024;
static const size_t eliteCount = 16;

static const std::string counterName = "MainCounter";

int _tmain(int argc, _TCHAR* argv[])
{
	GetTimeManager().clean();

	cl_float data[genSize];
	cl_uint out[genSize];
	GeneticChecks genCheck(genCheckFileName, genSize, eliteCount);

	for (size_t i = 0; i < genSize; ++i)
	{
		data[i] = 1.0;
	}

	genCheck.prepareData(data);

	CTimeCounter counter(counterName);
	genCheck.run();
	counter.stop();

	genCheck.getData(out);

	size_t cnt = 0;
	for (int i = 0; i < genSize; ++i)
	{
		if (out[i])
			cnt++;
	}

	std::cout << cnt << " - Should be equal to " << eliteCount << std::endl;
	for (std::map<std::string, TimerData>::value_type val : GetTimeManager().getTimers())
	{
		boost::chrono::microseconds mcs = boost::chrono::duration_cast<boost::chrono::microseconds>(val.second.duration);
		std::cout << "Time: " << mcs << std::endl;
	}
	
	
	_getch();
	return 0;
}

