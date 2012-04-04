#include "stdafx.h"
#include "CGeneticStrategyCLWrap.h"
#include <iostream>
#include <fstream>
#include "CAutomatImpl.h"
#include "CMapImpl.h"
#include "CTest.h"
#include "CRandomImpl.h"
#include <boost/algorithm/string/predicate.hpp>
#include "COpenCLWrapInvoker.h"


void CGeneticStrategyCLWrap::pushResults()
{
    boost::mutex& mutex = result->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    result->addGeneration( getBestIndivid(), getMaxFitness(), getAvgFitness() );
}

void CGeneticStrategyCLWrap::initMemory()
{
    cache = (float*)malloc( N*M*sizeof(float) );

    bestResults = (float*)malloc( gensToCount*sizeof(float) );
    sumResults = (float*)malloc( gensToCount*sizeof(float) );
    bestIndivids = (char*)malloc( gensToCount*( 2*statesCount*stateSize + 4 ) );
    sizes = (unsigned int*)malloc( 5*4 );
    srands = (unsigned int*)malloc( 5*4 );

	size_t bufSize = ( 2*statesCount*stateSize + 4 )*N*M;
    buffer = (char*)malloc( bufSize );

    CRandomImpl rand;
    for ( int i=0; i < N*M; ++i )
    {
        char * buf = buffer + i*(2*statesCount*stateSize + 4);
        CAutomatImpl::fillRandom( states, actions, buf, stateSize, &rand );
    }
}

CGeneticStrategyCLWrap::CGeneticStrategyCLWrap(CStateContainer* states, CActionContainer* actions, 
                                       CLabResultMulti* res, const std::vector< std::string >& strings )
:states(states), actions(actions), result(res), mapsBuffer(0), invoker(0), buffer(0)
{
    CRandomPtr rand( new CRandomImpl() );
    setFromStrings( strings, rand );
    statesCount = states->size();
    stateSize = 16;//1 << statesCount;
    initMemory();
}

void CGeneticStrategyCLWrap::setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand )
{
    gensToCount = 1;
    M = N = 0;
    deviceType = GPU;
    for ( size_t i=0; i < strings.size(); ++i )
    {
        const std::string& str = strings[i];
        if ( boost::starts_with( str, "gensCounter" ) )
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
    invoker = new CInvoker( this, rand );
    
    if (( M <= 0 ) || ( N <= 0 ))
    {
        throw std::exception( "bad config!" );
    }
}

CGeneticStrategyCLWrap::~CGeneticStrategyCLWrap()
{
	if ( buffer != 0 )
		free( buffer );
    free( mapsBuffer );
    free( cache );
    free( bestResults );
    free( sumResults );
    free( bestIndivids );
    free( sizes );
    free( srands );
    if ( invoker != 0 )
        delete invoker;
}

void CGeneticStrategyCLWrap::preStart()
{
    
	size_t mapSize = (2 + maps[0]->width()*maps[0]->height() );
	size_t bufSize = ( 2*statesCount*stateSize + 4 )*N*M;
	sizes[0] = statesCount;
	sizes[1] = stateSize;
	sizes[3] = gensToCount;

	CRandomImpl random;
	srands[0] = random.nextUINT();
	srands[1] = 0;
}

void CGeneticStrategyCLWrap::nextGeneration( CRandom* rand )
{
	CLWrapSettings settings;
	settings.M = M;
	settings.M = N;
	settings.step = 1;
	settings.rand = rand;
	settings.flowsCnt = gensToCount;
	CCLWrapInvoker invoker( this, settings );
	invoker.getThread()->join();
    
	//queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NDRange( N, M ) );
    //queue.finish();

    boost::mutex& mutex = result->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    size_t autSize = ( 2*statesCount*stateSize + 4);
    for ( size_t i=0; i<gensToCount; ++i)
    {
        curIndivid = CAutomatPtr( 
                new CAutomatImpl( CAutomatImpl::createFromBuffer(states, actions, bestIndivids + i*autSize ) ) );
        result->addGeneration( curIndivid, bestResults[i], sumResults[i] );
    }
}

void CGeneticStrategyCLWrap::swapBuffers()
{
    cl::Buffer tmp = statesBufCL2;
    statesBufCL2 = statesBufCL1;
    statesBufCL1 = tmp;
}

void CGeneticStrategyCLWrap::setMaps( std::vector<CMapPtr> maps )
{
    this->maps = maps;

    size_t mapSize = 2 + maps[0]->width()*maps[0]->height();
    size_t mapSizes = N*M*( mapSize );
    mapsBuffer = (int*)malloc( mapSize*4 );
    maps[0]->toIntBuffer( mapsBuffer );
    sizes[2] = mapSize;

    preStart();
    //pushResults();
}

const CMapPtr CGeneticStrategyCLWrap::getMap( size_t i )
{
    CMapImpl* map = new CMapImpl( maps.at(i).get() );
    return CMapPtr( map );
}

size_t CGeneticStrategyCLWrap::getMapsCount()
{
    return maps.size();
}

double CGeneticStrategyCLWrap::getAvgFitness() const
{
    return 0;
}

CAutomatPtr CGeneticStrategyCLWrap::getBestIndivid() const
{   
    return curIndivid;
}

double CGeneticStrategyCL::getMaxFitness() const
{
    return 0.0;
}

CInvoker* CGeneticStrategyCL::getInvoker() const
{
    return invoker;
}

size_t CGeneticStrategyCL::getN() const
{
    return N;
}

size_t CGeneticStrategyCL::getM() const
{
    return M;
}

std::string CGeneticStrategyCL::getDeviceType() const
{
    if ( deviceType == CPU )
    {
        return "OpenCL on CPU, " + sName;
    }
    return "OpenCL on GPU, " + sName;
}