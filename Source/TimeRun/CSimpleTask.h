#pragma once
#include "boost/smart_ptr/shared_ptr.hpp"
#include <boost/chrono.hpp>

//Task to Run and get time, not loops. No threads.

class CSimpleTask
{
public:
	virtual ~CSimpleTask();
    virtual void run();
    boost::chrono::milliseconds getMillis() const;
    boost::chrono::nanoseconds getNanos() const;
    void writeResult( std::ostream& out ) const;
protected:
    void startTimer();
    void stopTimer();
    virtual void work() = 0;

    boost::chrono::system_clock::time_point startTime, stopTime;
};

typedef boost::shared_ptr<CSimpleTask> CSimpleTaskPtr;