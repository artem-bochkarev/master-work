#include "CSimpleTask.h"

void CSimpleTask::startTimer()
{
    startTime = boost::chrono::system_clock::now();
}

void CSimpleTask::stopTimer()
{
    stopTime = boost::chrono::system_clock::now();
}



boost::chrono::nanoseconds CSimpleTask::getNanos() const
{
    return boost::chrono::duration_cast<boost::chrono::nanoseconds>(stopTime - startTime);
}

boost::chrono::milliseconds CSimpleTask::getMillis() const
{
    return boost::chrono::duration_cast<boost::chrono::milliseconds>(stopTime - startTime);
}

void CSimpleTask::run()
{
    startTimer();
    work();
    stopTimer();
}

void CSimpleTask::writeResult( std::ostream& out ) const
{
    if ( getMillis() > boost::chrono::milliseconds(10) )
        out << "Result: " << getMillis() << std::endl;
    else
        out << "Result: " << getNanos() << std::endl;
}