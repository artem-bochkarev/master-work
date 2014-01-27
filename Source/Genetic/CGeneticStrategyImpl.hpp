#pragma once
#include "stdafx.h"
#define NOMINMAX
#include "CGeneticStrategyImpl.h"
#include "CTest.h"
#include "GeneticCommon/CleverAntMapImpl.h"
#include <boost/thread/barrier.hpp>
#include <boost/thread/thread_time.hpp>
#include "CRandomImpl.h"
#include <boost/algorithm/string/predicate.hpp>
#include "CAntFitnes.h"

template<class AUTOMAT_TYPE>
class CLocalInvoker
{
	CGeneticStrategyImpl<AUTOMAT_TYPE>* strategy;
    size_t start, finish;
    AUTOMAT_TYPE*** newIndivids;
    boost::barrier& barrier;
    mutable CRandomImpl rand;
public:
	CLocalInvoker(CGeneticStrategyImpl<AUTOMAT_TYPE>* strg, size_t start,
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

template<class AUTOMAT_TYPE>
class CMainInvoker : public CInvoker
{
    AUTOMAT_TYPE*** newIndivids;
    size_t cnt;
public:
	CMainInvoker(CGeneticStrategyImpl<AUTOMAT_TYPE>* strg, size_t threadCount, CRandomPtr rand, boost::exception_ptr & error)
		:CInvoker(strg, error), cnt(threadCount) {}

    void operator ()() const
    {
        try
        {
            CGeneticStrategyImpl<AUTOMAT_TYPE>* strg = dynamic_cast<CGeneticStrategyImpl<AUTOMAT_TYPE>* >(m_pTask);

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
				CLocalInvoker<AUTOMAT_TYPE> invoker(static_cast<CGeneticStrategyImpl<AUTOMAT_TYPE>*>(m_pTask), prev, next,
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


template<class AUTOMAT_TYPE>
CGeneticStrategyImpl<AUTOMAT_TYPE>::CGeneticStrategyImpl(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, CLabResultMulti<COUNTERS_TYPE, INPUT_TYPE>* res, CAntFitnesPtr fitnes,
	const std::vector< std::string >& strings, Tools::Logger& logger )
:CGeneticStrategyCommon(pAntCommon, res, fitnes, strings, logger), isCacheValid(false), cnt(0), cachedResult(0)
{
    m_pRandom = CRandomPtr( new CRandomImpl() );
    setFromStrings( strings, m_pRandom );
	individs = new AUTOMAT_TYPE**[N];
	newIndivids = new AUTOMAT_TYPE**[N];
    cachedResult = new double*[N];
    for ( int i=0; i< N; ++i)
    {
		individs[i] = new AUTOMAT_TYPE*[M];
		newIndivids[i] = new AUTOMAT_TYPE*[M];
        cachedResult[i] = new double[M];
        for ( int j=0; j < M; ++j )
        {
			individs[i][j] = new AUTOMAT_TYPE(pAntCommon.get());
            individs[i][j]->generateRandom( m_pRandom.get() );
            newIndivids[i][j] = 0;
            cachedResult[i][j] = 0;
        }
    }
    //invoker = new CInvoker( this, rand );
}

template <class AUTOMAT_TYPE>
void CGeneticStrategyImpl<AUTOMAT_TYPE>::setFromStrings(const std::vector< std::string >& strings, CRandomPtr rand)
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
        invoker = new CMainInvoker<AUTOMAT_TYPE>( this, flowsCnt, rand, error );
    }

    if (( M == 0 ) || ( N == 0 ))
    {
        throw std::runtime_error( "bad config!" );
    }
}

template<class AUTOMAT_TYPE>
double CGeneticStrategyImpl<AUTOMAT_TYPE>::getAvgFitness() const
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

template<class AUTOMAT_TYPE>
CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE> CGeneticStrategyImpl<AUTOMAT_TYPE>::getBestIndivid() const
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
		return CAutomatPtr<COUNTERS_TYPE, INPUT_TYPE>(new AUTOMAT_TYPE(*(individs[x][y])));
}

template <class AUTOMAT_TYPE>
double CGeneticStrategyImpl<AUTOMAT_TYPE>::getMaxFitness() const
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

template < class AUTOMAT_TYPE >
void CGeneticStrategyImpl<AUTOMAT_TYPE>::fillCache() const
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

template < class AUTOMAT_TYPE >
void CGeneticStrategyImpl<AUTOMAT_TYPE>::run()
{
    nextGeneration( m_pRandom.get() );
}
template < class AUTOMAT_TYPE >
void CGeneticStrategyImpl<AUTOMAT_TYPE>::nextGeneration(size_t start, size_t finish, CRandom* rand)
{
    for ( size_t i=start; i < finish; ++i)
    {
        for ( int j=0; j < M; ++j )
        {
			AUTOMAT_TYPE a[5] = { AUTOMAT_TYPE(pAntCommon.get()), AUTOMAT_TYPE(pAntCommon.get()),
				AUTOMAT_TYPE(pAntCommon.get()), AUTOMAT_TYPE(pAntCommon.get()), AUTOMAT_TYPE(*individs[i][j]) };
			size_t k = rand->nextUINT() & 255;

			AUTOMAT_TYPE curr(*individs[i][j]);
			//if (k <= 128)
				//curr.mutate(rand);

            a[0].crossover( &curr, individs[ i ][ (j + 1)%M ], rand );
            a[2].crossover( &curr, individs[ (i + 1)%N ][ j ], rand );
            a[1].crossover( &curr, individs[ i ][ (j + N - 1)%M ], rand );
            a[3].crossover( &curr, individs[ (i + N - 1)%N ][ j ], rand );
			for (size_t z = 0; z < 4; ++z)
				a[z].mutate(rand);
            double r[8] = { 0., 0., 0., 0., 0., 0., 0., 0. };
            for ( int z=0; z<5; ++z )
            {
                r[z] = fitnesFunctor->fitnes( &a[z] );
            }

            k = 0;
            for ( size_t z=1; z<4; ++z )
                if ( r[z] > r[k] )
                    k = z;
            cachedResult[i][j] = r[k];
            if ( newIndivids[i][j] != 0 )
                delete newIndivids[i][j];
			newIndivids[i][j] = new AUTOMAT_TYPE(a[k]);
        }
    }
}

template < class AUTOMAT_TYPE >
void CGeneticStrategyImpl<AUTOMAT_TYPE>::preGeneration()
{
    if ( cnt % 10 == 0 )
    {
        std::srand( (size_t)std::time(0) );
    }
    isCacheValid = false;
}

template < class AUTOMAT_TYPE >
void CGeneticStrategyImpl<AUTOMAT_TYPE>::postGeneration()
{
	AUTOMAT_TYPE*** tmp = newIndivids;
    newIndivids = individs;
    individs = tmp;

    isCacheValid = true;
    pushResults();
    ++cnt;
}

template < class AUTOMAT_TYPE >
void CGeneticStrategyImpl<AUTOMAT_TYPE>::nextGeneration(CRandom* rand)
{
    preGeneration();
    nextGeneration( 0, N, rand );
    postGeneration();
}

template < class AUTOMAT_TYPE >
void CGeneticStrategyImpl<AUTOMAT_TYPE>::freeIndivids(AUTOMAT_TYPE*** ind)
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

template < class AUTOMAT_TYPE >
CGeneticStrategyImpl<AUTOMAT_TYPE>::~CGeneticStrategyImpl()
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

template < class AUTOMAT_TYPE >
std::string CGeneticStrategyImpl<AUTOMAT_TYPE>::getDeviceType() const
{
    return "C++ on CPU";
}