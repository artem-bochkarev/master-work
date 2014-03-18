#include "CTimeRunner.h"
#include <boost/thread/condition.hpp>
//#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/chrono.hpp>

const long sleep_time = 100; //millisec

CTimeRunner::CTimeRunner()
:running(false), m_pInvoker(0) {}

size_t CTimeRunner::getRunCount() const
{
    if ( m_pTask->getError() )
        boost::rethrow_exception(m_pTask->getError());
    boost::mutex& mutex = m_pTimeResult->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    return m_pTimeResult->getRunCount();
}

void CTimeRunner::start()
{
    if ( !running  )
    {
        running = true;
        m_pInvoker = m_pTask->getInvoker( );
        m_pThread = m_pInvoker->getThread();
    }
}

void CTimeRunner::pause()
{
    if ( m_pThread.get() != 0 )
    {
        m_pThread->interrupt();
        m_pThread->join();
        running = false;
        m_pThread->detach();
        m_pThread.reset();
    }
}

void CTimeRunner::runForTime( int milisec )
{
    typedef boost::chrono::milliseconds ms;
    start();
    boost::chrono::system_clock::time_point start = boost::chrono::system_clock::now();
    for ( ; ; )
    {
        boost::this_thread::sleep( boost::posix_time::millisec( sleep_time ) );
        ms millis = boost::chrono::duration_cast<ms>(boost::chrono::system_clock::now() - start);
        if ( milisec - millis.count() <= 0 || milisec - millis.count() < sleep_time/2  )
            break;
        if ( m_pTask->getError() )
            boost::rethrow_exception( m_pTask->getError() );
    }
    pause();
    boost::this_thread::sleep( boost::posix_time::millisec( sleep_time ) );
}

bool CTimeRunner::isRunning() const
{
    return running;
}

void CTimeRunner::writeInfo( std::ostream& out ) const
{
    out << "Standard TimeRunner\n";
}

void CTimeRunner::writeResult( std::ostream& out ) const
{
    if ( m_pTimeResult.get() != 0 )
        out << "Result: " << m_pTimeResult->getRunCount() << std::endl;
    else
    if ( m_pInvoker != 0 )
        out << "Result: " << m_pInvoker->getLoopCounter() << std::endl;
    else
        out << "No Invoker - No Result: " << std::endl;
}