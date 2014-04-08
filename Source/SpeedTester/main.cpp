#include <iostream>
#include <conio.h>
#include "Tools/Logger.h"
#include "Tester.h"
#include "CleverAnt3\CleverAnt3LaboratoryFactory.h"
#include "TimeRun/CSimpleTask.h"

int main(int argc, char *argv[])
{
	Tools::Logger logger;
	const char* testFile = "../Release/tests/cleverAnt3ShortTables/testIt.txt";
	if (argc > 1)
		testFile = argv[1];

	const char* outputFile = "speed_results.txt";
	if (argc > 2)
		outputFile = argv[2];

	const char* clFile = "../CleverAnt3/genShortTables.cl";
	if (argc > 3)
		clFile = argv[3];

	const char* configFile = "config.txt";
	if (argc > 4)
		configFile = argv[4];

	if (argc <= 1)
	{
		Tester tester(testFile, outputFile, clFile, configFile, logger);
		tester.run();
	}
	else
	{
		Tester tester(argv[1], "speed_results.txt", "config.txt", "../CleverAnt3/genShortTables.cl", logger);
		tester.run();
	}
	/*class TestTask : public CSimpleTask
    {
    protected:
        virtual void work()
        {
            int k = 0;
            for (int i=0; i<1000000; ++i)
                k ^= k*i;
            ++k;
        }
    };
    TestTask task;
    task.run();
    task.writeResult( std::cout );*/
	/*std::vector<std::string> nulVec;
	nulVec.push_back("config.txt");
	CTimeRunnerPtr laboratory = CleverAnt3LaboratoryFactory::createLaboratory(logger, nulVec);

	laboratory->writeInfo(std::cout);
	//laboratory->writeInfo(*pOut);

	size_t timeSec = 10;

	try
	{
		laboratory->runForTime(timeSec * 1000);

		laboratory->writeResult(std::cout);
		//laboratory->writeResult(*pOut);

		logger << "[SUCCESS] " << "\n";
	}
	catch (std::runtime_error& err)
	{
		//*pOut << "FAILED\n";
		//throw err;
	}*/
    _getch();
}