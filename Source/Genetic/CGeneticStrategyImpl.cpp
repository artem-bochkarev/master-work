#include "stdafx.h"
#define NOMINMAX
#include "CGeneticStrategyImpl.h"
#include "CTest.h"
#include "CMapImpl.h"
#include <boost/thread/barrier.hpp>
#include <boost/thread/thread_time.hpp>
#include "CRandomImpl.h"
#include <boost/algorithm/string/predicate.hpp>
#include "CAntFitnes.h"

class CLocalInvoker
{
    CGeneticStrategyImpl* strategy;
    size_t start, finish;
    CAutomatImpl*** newIndivids;
    boost::barrier& barrier;
    mutable CRandomImpl rand;
public:
    CLocalInvoker( CGeneticStrategyImpl* strg, size_t start, 
            size_t finish, boost::barrier& barrier )
        :strategy( strg ), start(start), finish(finish),
        barrier(barrier), rand( std::clock() ) {}

    void operator ()() const
    {
        while (true)
        {
            strategy->nextGeneration(start, finish, &rand );
            barrier.wait();
            boost::this_thread::interruption_point();
            barrier.wait();
            boost::this_thread::interruption_point();
        }
    }

    virtual threadPtr getThread() const
    {
        return threadPtr( new boost::thread( *this ) );
    }
    private:
        CLocalInvoker();
        CLocalInvoker& operator=( const CLocalInvoker& );
};

class CMainInvoker : public CInvoker
{
    CAutomatImpl*** newIndivids;
    size_t cnt;
public:
    CMainInvoker( CGeneticStrategyImpl* strg, size_t threadCount, CRandomPtr rand, boost::exception_ptr & error )
        :CInvoker(strg, error), cnt(threadCount) {}

    void operator ()() const
    {
        try
        {
            CGeneticStrategyImpl* strg = dynamic_cast<CGeneticStrategyImpl*>(m_pTask);

            boost::thread_group group;
            size_t N = strg->getN();
            size_t step = std::max( (size_t)1, N / cnt );
            size_t counter = N / step;
            if ( N%step != 0 )
                ++counter;
            size_t prev = 0;
            size_t next = prev + step;
            
            strg->preGeneration();

            boost::barrier firstBarrier( counter + 1 );
            for ( size_t i=0; i<counter; ++i )
            {
                if ( i == counter - 1 )
                    next = N;
                CLocalInvoker invoker( static_cast<CGeneticStrategyImpl*>(m_pTask), prev, next, 
                    firstBarrier );
                group.create_thread( invoker );
                prev = next;
                next += step;
            }

            try
            {
                while (true)
                {
                    firstBarrier.wait();
                    strg->postGeneration();
                    boost::this_thread::interruption_point();
                    firstBarrier.wait();
                    strg->preGeneration();
                }
            }catch( boost::thread_interrupted& )
            {
                group.interrupt_all();
            }

            group.interrupt_all();
            group.join_all();
        }
        catch( boost::thread_interrupted& ) {}
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


CGeneticStrategyImpl::CGeneticStrategyImpl(CStateContainer* states, CActionContainer* actions, 
                                           CLabResultMulti* res, CAntFitnesPtr fitnes, const std::vector< std::string >& strings, Tools::Logger& logger )
:CGeneticStrategyCommon(states, actions, res, fitnes, strings, logger), isCacheValid(false), cnt(0), cachedResult(0)
{
    m_pRandom = CRandomPtr( new CRandomImpl() );
    setFromStrings( strings, m_pRandom );
    individs = new CAutomatImpl**[N];
    newIndivids = new CAutomatImpl**[N];
    cachedResult = new double*[N];
    for ( int i=0; i< N; ++i)
    {
        individs[i] = new CAutomatImpl*[M];
        newIndivids[i] = new CAutomatImpl*[M];
        cachedResult[i] = new double[M];
        for ( int j=0; j < M; ++j )
        {
            individs[i][j] = new CAutomatImpl( states, actions );
            individs[i][j]->generateRandom( m_pRandom.get() );
            newIndivids[i][j] = 0;
            cachedResult[i][j] = 0;
        }
    }
    //invoker = new CInvoker( this, rand );
}

void CGeneticStrategyImpl::setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand )
{
    size_t flowsCnt = 5;
    M = N = 0;
    for ( size_t i=0; i < strings.size(); ++i )
    {
        const std::string& str = strings[i];
        if ( boost::starts_with( str, "flowsCounter" ))
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            flowsCnt = atoi( tmp.c_str() );
            continue;
        }
        if ( boost::starts_with( str, "M" ))
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            M = atoi( tmp.c_str() );
            continue;
        }
        if ( boost::starts_with( str, "N" ) )
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            N = atoi( tmp.c_str() );
            continue;
        }
    }

    if ( flowsCnt == 1 )
    {
        invoker = new CInvoker( this, error );
    }else
    {
        invoker = new CMainInvoker( this, flowsCnt, rand, error );
    }

    if (( M == 0 ) || ( N == 0 ))
    {
        throw std::runtime_error( "bad config!" );
    }
}

double CGeneticStrategyImpl::getAvgFitness() const
{
    if ( !isCacheValid )
        fillCache();
    double sum = 0.0;
    for ( int i=0; i< N; ++i)
        for ( int j=0; j < M; ++j )
            sum += cachedResult[i][j];
    sum /= N*M;
    return sum;
}

CAutomatPtr CGeneticStrategyImpl::getBestIndivid() const
{
    if ( !isCacheValid )
        fillCache();

    double max = 0.0;
    int x = 0, y = 0;
    for ( int i=0; i< N; ++i)
        for ( int j=0; j < M; ++j )
        {
            if ( cachedResult[i][j] > max )
            {
                max = cachedResult[i][j];
                x = i;
                y = j;
            }
        }
    return CAutomatPtr( new CAutomatImpl( *(individs[x][y]) ) ); 
}

double CGeneticStrategyImpl::getMaxFitness() const
{
    if ( !isCacheValid )
        fillCache();       

    double max = 0.0;
    for ( int i=0; i< N; ++i)
        for ( int j=0; j < M; ++j )
            if ( cachedResult[i][j] > max )
                max = cachedResult[i][j];
    return max;    
}

void CGeneticStrategyImpl::fillCache() const
{
    if ( isCacheValid )
        return;
    for ( int i=0; i< N; ++i)
        for ( int j=0; j < M; ++j )
        {
            double tmp = 0.0;
            for ( std::vector<CMapPtr>::const_iterator it = maps.begin(); it != maps.end(); ++it )
            {
                CMapImpl tmpMap( it->get() );
                tmp += CTest::run( individs[i][j],  &tmpMap );
            }
            cachedResult[i][j] = tmp;
        }
    isCacheValid = true;
}

void CGeneticStrategyImpl::run()
{
    nextGeneration( m_pRandom.get() );
}

void CGeneticStrategyImpl::nextGeneration( size_t start, size_t finish, CRandom* rand )
{
    for ( size_t i=start; i < finish; ++i)
    {
        for ( int j=0; j < M; ++j )
        {
            CAutomatImpl a[5] = { CAutomatImpl(states, actions), CAutomatImpl(states, actions),
                        CAutomatImpl(states, actions), CAutomatImpl(states, actions), CAutomatImpl(*individs[i][j]) };

            a[0].crossover( individs[i][j], individs[ i ][ (j + 1)%M ], rand );
            a[2].crossover( individs[i][j], individs[ (i + 1)%N ][ j ], rand );
            a[1].crossover( individs[i][j], individs[ i ][ (j + N - 1)%M ], rand );
            a[3].crossover( individs[i][j], individs[ (i + N - 1)%N ][ j ], rand );
            double r[8] = { 0., 0., 0., 0., 0., 0., 0., 0. };
            for ( int z=0; z<5; ++z )
            {
                r[z] = fitnesFunctor->fitnes( &a[z] );
            }

            int k = 0;
            for ( int z=1; z<4; ++z )
                if ( r[z] > r[k] )
                    k = z;
            cachedResult[i][j] = r[k];
            if ( newIndivids[i][j] != 0 )
                delete newIndivids[i][j];
            newIndivids[i][j] = new CAutomatImpl( a[k] );
        }
    }
}

void CGeneticStrategyImpl::preGeneration()
{
    if ( cnt % 10 == 0 )
    {
        std::srand( (size_t)std::time(0) );
    }
    isCacheValid = false;
}

void CGeneticStrategyImpl::postGeneration()
{
    CAutomatImpl*** tmp = newIndivids;
    newIndivids = individs;
    individs = tmp;

    isCacheValid = true;
    pushResults();
    ++cnt;
}

void CGeneticStrategyImpl::nextGeneration( CRandom* rand )
{
    preGeneration();
    nextGeneration( 0, N, rand );
    postGeneration();
}

void CGeneticStrategyImpl::freeIndivids( CAutomatImpl*** ind )
{
    if ( ind != 0 )
    {
        for ( int i=0; i < N; ++i)
        {
            if ( ind[i] != 0 )
            {
                for ( int j=0; j < M; ++j )
                    if ( ind[i][j] != 0 )
                        delete ind[i][j];
                delete[] ind[i];
            }
        }
        delete[] ind;
        ind = 0;
    }
}

CGeneticStrategyImpl::~CGeneticStrategyImpl()
{
    freeIndivids( individs );
    freeIndivids( newIndivids );
    if ( cachedResult != 0 )
    {
        for ( int i=0; i < N; ++i)
            delete[] cachedResult[i];
        delete[] cachedResult;
    }
    if ( invoker != 0 )
        delete invoker;
}

std::string CGeneticStrategyImpl::getDeviceType() const
{
    return "C++ on CPU";
}