#include "CInvoker.h"

CInvoker::CInvoker(CTask* task, boost::exception_ptr & error)
:m_pTask(task), m_error(error), counter(0) {}

CInvoker& CInvoker::operator= (const CInvoker& inv)
{
	m_pTask = inv.m_pTask;
	m_error = inv.m_error;
	return *this;
}

void CInvoker::operator ()()
{
	try
	{
		for (;;)
		{
			boost::this_thread::interruption_point();
			m_pTask->run();
			++counter;
		}
	}
	catch (boost::thread_interrupted&)
	{
		//normal situation
	}
	catch (...)
	{
		m_error = boost::current_exception();
	}
}

threadPtr CInvoker::getThread()
{
	return threadPtr(new boost::thread(*this));
}

size_t CInvoker::getLoopCounter() const
{
	return counter;
}