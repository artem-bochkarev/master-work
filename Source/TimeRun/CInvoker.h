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

    CInvoker& operator= ( const CInvoker& inv )
    {
        m_pTask = inv.m_pTask;
        m_error = inv.m_error;
    }

    virtual void operator ()()
    {
        try
        {
            for ( ; ; )
            {
                boost::this_thread::interruption_point();
                m_pTask->run();
            }
        }catch( boost::thread_interrupted& ) 
        {
            //normal situation
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