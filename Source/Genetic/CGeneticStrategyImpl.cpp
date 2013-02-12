#include "stdafx.h"
#define NOMINMAX
#include "CGeneticStrategyImpl.h"
#include "CTest.h"
#include "CMapImpl.h"
#include <boost/thread/barrier.hpp>
#include <boost/thread/thread_time.hpp>
#include "CRandomImpl.h"
#include <boost/algorithm/string/predicate.hpp>

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
        :CInvoker(strg, rand, error), cnt(threadCount) {}

    void operator ()() const
    {
        try
        {
            boost::thread_group group;
            size_t N = strategy->getN();
            size_t step = std::max( (size_t)1, N / cnt );
            size_t counter = N / step;
            if ( N%step != 0 )
                ++counter;
            size_t prev = 0;
            size_t next = prev + step;
            

            CGeneticStrategyImpl* strg = static_cast<CGeneticStrategyImpl*>(strategy);
            strg->preGeneration();

            boost::barrier firstBarrier( counter + 1 );
            for ( size_t i=0; i<counter; ++i )
            {
                if ( i == counter - 1 )
                    next = N;
                CLocalInvoker invoker( static_cast<CGeneticStrategyImpl*>(strategy), prev, next, 
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
            error = boost::current_exception();
        }
    }

    virtual threadPtr getThread() const
    {
        return threadPtr( new boost::thread( *this ) );
    }

};


CGeneticStrategyImpl::CGeneticStrategyImpl(CStateContainer* states, CActionContainer* actions, 
                                           CLabResultMulti* res, const std::vector< std::string >& strings, Tools::Logger& logger )
:CGeneticStrategyCommon(states, actions, res, strings, logger), isCacheValid(false), cnt(0), cachedResult(0)
{
    CRandomPtr rand( new CRandomImpl() );
    setFromStrings( strings, rand );
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
            individs[i][j]->generateRandom( rand.get() );
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
        invoker = new CInvoker( this, rand, error );
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

void CGeneticStrategyImpl::nextGeneration( size_t start, size_t finish, CRandom* rand )
{
    for ( size_t i=start; i < finish; ++i)
    {
        for ( int j=0; j < M; ++j )
        {
            
            //CAutomatImpl b[8] = { a[0], a[0], a[0], a[0], a[0], a[0], a[0], a[0] };
           // if ( rand()%100 < 10 )
           //     individs[i][j]->mutate();
            CAutomatImpl a[5] = { CAutomatImpl(states, actions), CAutomatImpl(states, actions),
                        CAutomatImpl(states, actions), CAutomatImpl(states, actions), CAutomatImpl(*individs[i][j]) };
          /*  std::vector<CAutomatImplPtr> vec( CAutomatImpl::cross( individs[i][j], individs[ i ][ (j + 1)%N ] ) );
            b[0] = *vec[0];
            b[1] = *vec[1];
            vec = CAutomatImpl::cross( individs[i][j], individs[ (i + 1)%N ][ j ] );
            b[2] = *vec[0];
            b[3] = *vec[1];
            vec = CAutomatImpl::cross( individs[i][j], individs[ i ][ (j + N - 1)%N ] );
            b[4] = *vec[0];
            b[5] = *vec[1];
            vec = CAutomatImpl::cross( individs[i][j], individs[ (i + N - 1)%N ][ j ] );
            b[6] = *vec[0];
            b[7] = *vec[1];*/
            a[0].crossover( individs[i][j], individs[ i ][ (j + 1)%M ], rand );
            a[2].crossover( individs[i][j], individs[ (i + 1)%N ][ j ], rand );
            a[1].crossover( individs[i][j], individs[ i ][ (j + N - 1)%M ], rand );
            a[3].crossover( individs[i][j], individs[ (i + N - 1)%N ][ j ], rand );
            double r[8] = { 0., 0., 0., 0., 0., 0., 0., 0. };
            for ( std::vector<CMapPtr>::const_iterator it = maps.begin(); it != maps.end(); ++it )
                for ( int z=0; z<5; ++z )
                {
                    CMapImpl tmpMap( it->get() );
                    r[z] +=  CTest::run( &a[z],  &tmpMap );
                }
            int k = 0;
            for ( int z=1; z<4; ++z )
                if ( r[z] > r[k] )
                    k = z;
            cachedResult[i][j] = r[k];
            if ( newIndivids[i][j] != 0 )
                delete newIndivids[i][j];
            newIndivids[i][j] = new CAutomatImpl( a[k] );
           // if ( rand()%100 < 10 )
            //    newIndivids[i][j]->mutate( rand );
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


void CGeneticStrategyImpl::setMaps( std::vector<CMapPtr> maps )
{
    this->maps = maps;
    pushResults();
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

const CMapPtr CGeneticStrategyImpl::getMap( size_t i )
{
    return CGeneticStrategyCommon::getMap(i);
}

size_t CGeneticStrategyImpl::getMapsCount()
{
    return CGeneticStrategyCommon::getMapsCount();
}

void CGeneticStrategyImpl::pushResults()
{
    CGeneticStrategyCommon::pushResults();
}

CInvoker* CGeneticStrategyImpl::getInvoker() const
{
    return CGeneticStrategyCommon::getInvoker();
}

size_t CGeneticStrategyImpl::getN() const
{
    return CGeneticStrategyCommon::getN();
}

size_t CGeneticStrategyImpl::getM() const
{
    return CGeneticStrategyCommon::getM();
}

const boost::exception_ptr& CGeneticStrategyImpl::getError() const
{
    return CGeneticStrategyCommon::getError();
}