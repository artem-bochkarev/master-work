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
    size_t counter;
public:
	CInvoker(CTask* task, boost::exception_ptr & error);
	CInvoker& operator= (const CInvoker& inv);
	virtual void operator ()();
	virtual threadPtr getThread();
	virtual size_t getLoopCounter() const;
};