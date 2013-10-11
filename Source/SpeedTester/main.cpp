#include <iostream>
#include <conio.h>
#include "Tools/Logger.h"
#include "Tester.h"
#include "TimeRun/CSimpleTask.h"

int main()
{
    Tools::Logger logger;
    /*Tester tester("tests/testIt.txt", "speed_results.txt", logger);
    tester.run();*/
    class TestTask : public CSimpleTask
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
    task.writeResult( std::cout );
    _getch();
}