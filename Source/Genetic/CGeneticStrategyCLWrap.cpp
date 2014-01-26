#include "stdafx.h"
#include "CGeneticStrategyCLWrap.h"
#include <iostream>
#include <fstream>
#include "GeneticCommon/AutomatImpl.h"
#include "CMapImpl.h"
#include "CTest.h"
#include "CRandomImpl.h"
#include <boost/algorithm/string/predicate.hpp>
#include "COpenCLWrapInvoker.h"
#include "CAntFitnes.h"

void CGeneticStrategyCLWrap::initMemory()
{

    /*uint* buffer, outBuffer, tempBuffer;

    int* mapsBuffer, mapBuffer;
    int* cache;*/

    sizes = (uint*)malloc( 5*4 );
    srands = (uint*)malloc( 5*4 );

	size_t bufSize = ( 2*pAntCommon->statesCount()*stateSize + 4 )*N*M;
    buffer     = (uint*)malloc( bufSize );
    outBuffer  = (uint*)malloc( bufSize );
    tempBuffer = (uint*)malloc( bufSize );


    cache = (int*)malloc( N*M*sizeof(int) );

    CRandomImpl rand;
    for ( int i=0; i < N*M; ++i )
    {
        char * buf = (char*)buffer + i*(2*pAntCommon->statesCount()*stateSize + 4);
		CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::fillRandom(pAntCommon.get(), buf, &rand);
    }
}

CGeneticStrategyCLWrap::CGeneticStrategyCLWrap(CAntCommonPtr<COUNTERS_TYPE> pAntCommon, CLabResultMulti* res, CAntFitnesPtr fitnes,
	const std::vector< std::string >& strings, Tools::Logger& logger )
:CGeneticStrategyCommon(pAntCommon, res, fitnes, strings, logger), mapsBuffer(0), mapBuffer(0), buffer(0)
{
    m_pRandom = CRandomPtr( new CRandomImpl() );
    setFromStrings( strings, m_pRandom );
    stateSize = 16;//1 << statesCount;
    initMemory();
}

void CGeneticStrategyCLWrap::setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand )
{
    gensToCount = 1;
    M = N = 0;
    for ( size_t i=0; i < strings.size(); ++i )
    {
        const std::string& str = strings[i];
        if ( boost::starts_with( str, "flowsCounter" ) )
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            gensToCount = atoi( tmp.c_str() );
            break;
        }

        if ( boost::starts_with( str, "M" ) )
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
    invoker = new CInvoker( this, error );
    
    if (( M <= 0 ) || ( N <= 0 ))
    {
        throw std::runtime_error( "bad config!" );
    }
}

CGeneticStrategyCLWrap::~CGeneticStrategyCLWrap()
{
	if ( buffer != 0 )
		free( buffer );
	if (outBuffer != 0)
		free(outBuffer);
	if (tempBuffer != 0)
		free(tempBuffer);

	free( mapsBuffer );
	free( mapBuffer );
    free( cache );
    free( sizes );
    free( srands );
    if ( invoker != 0 )
        delete invoker;
}

void CGeneticStrategyCLWrap::preStart()
{
    settings.M = M;
	settings.N = N;
	settings.step = 1;
    settings.inBuffer = buffer;
    settings.outBuffer = outBuffer;
    
    settings.tempBuffer = tempBuffer;
    settings.constSizes = sizes;
    settings.varValues = srands;
    settings.cache = cache;
	settings.flowsCnt = gensToCount;

	sizes[0] = pAntCommon->statesCount();
	sizes[1] = stateSize;
	sizes[3] = 1;

	CRandomImpl random;
	srands[0] = random.nextUINT();
	srands[1] = 0;
}

void CGeneticStrategyCLWrap::run()
{
    nextGeneration( m_pRandom.get() );
}

void CGeneticStrategyCLWrap::nextGeneration( CRandom* rand )
{
	srands[0] = rand->nextUINT();
	settings.inBuffer = buffer;
    settings.outBuffer = outBuffer;
    settings.mapsBuffer = mapsBuffer;
    settings.map = mapBuffer;

	CCLWrapInvoker invoker( this, settings );
    threadPtr invokerThread = invoker.getThread();
	try
	{
		invokerThread->join();
	}catch( boost::thread_interrupted& err )
	{
		invokerThread->interrupt();
		invokerThread->join();
		throw err;
	}

    int bestResult = 0;
    double avgResult = 0.0;
    int bestPos = 0;
    for ( int i=0; i<M*N; ++i ) 
    {
        avgResult += cache[i];
        if ( cache[i] > cache[bestPos] )
            bestPos = i;
    }
    avgResult /= M*N;
    bestResult = cache[bestPos];
	size_t autSize = (2 * pAntCommon->statesCount()*stateSize + 4);

    boost::this_thread::disable_interruption di;
	curIndivid = CAutomatImpl<COUNTERS_TYPE, INPUT_TYPE>::createFromBuffer(pAntCommon.get(), (char*)outBuffer + bestPos*autSize);
    boost::mutex& mutex = result->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    result->addGeneration( curIndivid, bestResult, avgResult ); 
    swapBuffers();
	boost::this_thread::interruption_point();
}

void CGeneticStrategyCLWrap::swapBuffers()
{
    uint* tmp = buffer;
    buffer = outBuffer;
    outBuffer = tmp;
}

void CGeneticStrategyCLWrap::setMaps( std::vector<CMapPtr> maps )
{
    fitnesFunctor->setMaps(maps);

    size_t mapSize = 2 + maps[0]->width()*maps[0]->height();
    size_t mapSizes = N*M*( mapSize );
    if ( mapBuffer != 0 )
        delete mapBuffer;
    if ( mapsBuffer != 0 )
        delete mapsBuffer;
    mapBuffer = (int*)malloc( mapSize*4 );
    mapsBuffer = (int*)malloc( mapSizes*4 );
    maps[0]->toIntBuffer( mapBuffer );
    sizes[2] = mapSize;

    preStart();
    //pushResults();
}

std::string CGeneticStrategyCLWrap::getDeviceType() const
{
    return "OpenCL Wrapping on CPU";
}

double CGeneticStrategyCLWrap::getAvgFitness() const
{
    return 0.0;
}
double CGeneticStrategyCLWrap::getMaxFitness() const
{
    return 0.0;
}
CAutomatPtr CGeneticStrategyCLWrap::getBestIndivid() const
{
    return CAutomatPtr();
}

const CMapPtr CGeneticStrategyCLWrap::getMap( size_t i )
{
    return CGeneticStrategyCommon::getMap(i);
}

size_t CGeneticStrategyCLWrap::getMapsCount()
{
    return CGeneticStrategyCommon::getMapsCount();
}

void CGeneticStrategyCLWrap::pushResults()
{
    CGeneticStrategyCommon::pushResults();
}

CInvoker* CGeneticStrategyCLWrap::getInvoker() const
{
    return CGeneticStrategyCommon::getInvoker();
}

size_t CGeneticStrategyCLWrap::getN() const
{
    return CGeneticStrategyCommon::getN();
}

size_t CGeneticStrategyCLWrap::getM() const
{
    return CGeneticStrategyCommon::getM();
}

const boost::exception_ptr& CGeneticStrategyCLWrap::getError() const
{
    return CGeneticStrategyCommon::getError();
}