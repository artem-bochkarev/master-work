#include <iostream>
#include <conio.h>
#include "Tools/Logger.h"
//#include "Tester.h"
#include "CleverAnt3\CleverAnt3LaboratoryFactory.h"
#include "TimeRun/CSimpleTask.h"

int main()
{
    Tools::Logger logger;
    /*Tester tester("tests/testIt.txt", "speed_results.txt", logger);
    tester.run();*/
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
	std::vector<std::string> nulVec;
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
	}
    _getch();
}