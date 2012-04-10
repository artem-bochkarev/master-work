#include <iostream>
#include "Tools/Logger.h"
#include "Tester.h"

int main()
{
    Tools::Logger logger;
    Tester tester("tests/testIt.txt", "speed_results.txt", logger);
    tester.run();
}