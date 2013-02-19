#pragma once
#include "boost/smart_ptr/shared_ptr.hpp"
#include <boost/thread/mutex.hpp>

class CTimeResult
{
public:
    virtual size_t getRunCount() const = 0;
    virtual boost::mutex& getMutex();
protected:
    boost::mutex m_mutex;
};

typedef boost::shared_ptr< CTimeResult > CTimeResultPtr;