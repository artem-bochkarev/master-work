#include "stdafx.h"
#include "COpenCLWrapInvoker.h"
#include <boost/thread/barrier.hpp>
#include <boost/thread/thread_time.hpp>
#include <boost/interprocess/detail/atomic.hpp>

class SuperQueue
{
public:
	SuperQueue()
	{
		value = 0;
	}
	boost::uint32_t getNext()
	{
		boost::uint32_t oldVal;
		boost::uint32_t newVal;
		do
		{
			oldVal = value;
			newVal = oldVal + 1;
		}while ( oldVal != boost::interprocess::ipcdetail::atomic_cas32( &value, newVal, oldVal) );
		return newVal;
	}
	boost::uint32_t getNextN( boost::uint32_t n )
	{
		boost::uint32_t oldVal;
		boost::uint32_t newVal;
		do
		{
			oldVal = value;
			newVal = oldVal + n;
		}while ( oldVal != boost::interprocess::ipcdetail::atomic_cas32( &value, newVal, oldVal) );
		return newVal;
	}
	int reset()
	{
		value = 0;
	}
private:
	boost::uint32_t value;
};

class CLocalCLWrapInvoker 
{
protected:
    SuperQueue* queue;
    const CLWrapSettings& settings;
public:
    CLocalCLWrapInvoker( const CLWrapSettings& settings, SuperQueue* queue )
        :queue(queue), settings(settings) {}

    virtual void operator ()()
	{
		size_t N = settings.N;
		size_t M = settings.M;
		size_t step = settings.step;
		size_t end = queue->getNext();
		size_t start = end - step;
		while ( start < N*M )
		{
			size_t end = std::min( N*M, end);
			for ( size_t i = start; i < end; ++i )
			{
				size_t n = i / M;
				size_t m = i - n*M;
				//kernelCall();
			}
			end = queue->getNext();
			start = end - step;
		}
	}

    virtual threadPtr getThread() const
	{
		return threadPtr( new boost::thread( *this ) );
	}
};

void CCLWrapInvoker::operator ()()
{
	boost::thread_group group;
	SuperQueue queue;
	for ( size_t i=0; i<settings.flowsCnt; ++i )
	{
		CLocalCLWrapInvoker invoker( settings, &queue );
		group.create_thread( invoker );
	}
	group.join_all();
}

threadPtr CCLWrapInvoker::getThread() const
{
	return threadPtr( new boost::thread( *this ) );
}