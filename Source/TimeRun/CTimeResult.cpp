#include "CTimeResult.h"

boost::mutex& CTimeResult::getMutex()
{
    return m_mutex;
}