#pragma once
#include "boost/smart_ptr/shared_ptr.hpp"
#include <boost/exception_ptr.hpp>

class CInvoker;

class CTask
{
public:
    virtual void run() = 0;
    virtual const boost::exception_ptr& getError() const = 0;
    virtual CInvoker* getInvoker() const = 0;
};

typedef boost::shared_ptr<CTask> CTaskPtr;