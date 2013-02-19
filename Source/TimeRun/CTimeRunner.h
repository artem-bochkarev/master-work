#pragma once
#include "CTask.h"
#include "CInvoker.h"
#include "CTimeResult.h"

class CTimeRunner
{
public:
    virtual void start();
    virtual void runForTime( int milisec );
    virtual void pause();
    virtual size_t getRunCount() const;

    virtual bool isRunning() const;
protected:
    CTaskPtr m_pTask;
    CTimeResultPtr m_pTimeResult;
    bool running;
    CInvoker* m_pInvoker;
    threadPtr m_pThread;
};

typedef boost::shared_ptr<CTimeRunner> CTimeRunnerPtr;