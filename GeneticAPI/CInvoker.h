#pragma once
#include "CGeneticStrategy.h"
#include <boost/thread.hpp>

typedef boost::shared_ptr< boost::thread > threadPtr;

class CInvoker
{
protected:
    CGeneticStrategy* strategy;
    CRandomPtr random;
public:
    CInvoker( CGeneticStrategy* strg, CRandomPtr rand )
        :strategy( strg ), random(rand) {}

    virtual void operator ()()
    {
        while ( true )
        {
            boost::this_thread::interruption_point();
            strategy->nextGeneration( random.get() );
        }
    }

    virtual threadPtr getThread() const
    {
        return threadPtr( new boost::thread( *this ) );
    }
};