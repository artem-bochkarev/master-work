#pragma once
#include "CTask.h"
#include <boost/thread.hpp>
#include <boost/exception_ptr.hpp>

typedef boost::shared_ptr< boost::thread > threadPtr;

class CInvoker
{
protected:
    CTask* m_pTask;
    boost::exception_ptr & m_error;
public:
    CInvoker( CTask* task, boost::exception_ptr & error )
        :m_pTask( task ), m_error(error) {}

    virtual void operator ()()
    {
        try
        {
            while ( true )
            {
                boost::this_thread::interruption_point();
                m_pTask->run();
            }
        }catch( boost::thread_interrupted& ) 
        {
            int k = 0;
        }
        catch( ... )
        {
            m_error = boost::current_exception();
        }
    }

    virtual threadPtr getThread() const
    {
        return threadPtr( new boost::thread( *this ) );
    }
};