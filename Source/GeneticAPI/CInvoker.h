#pragma once
#include "CGeneticStrategy.h"
#include <boost/thread.hpp>
#include <boost/exception_ptr.hpp>

typedef boost::shared_ptr< boost::thread > threadPtr;

class CInvoker
{
protected:
    CGeneticStrategy* strategy;
    CRandomPtr random;
    boost::exception_ptr & error;
public:
    CInvoker( CGeneticStrategy* strg, CRandomPtr rand, boost::exception_ptr & error )
        :strategy( strg ), random(rand), error(error) {}

    virtual void operator ()()
    {
        try
        {
            while ( true )
            {
                boost::this_thread::interruption_point();
                strategy->nextGeneration( random.get() );
            }
        }catch( boost::thread_interrupted& ) 
        {
            int k = 0;
        }
        catch( ... )
        {
            error = boost::current_exception();
        }
    }

    virtual threadPtr getThread() const
    {
        return threadPtr( new boost::thread( *this ) );
    }
};