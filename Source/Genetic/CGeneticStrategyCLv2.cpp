#include "stdafx.h"
#include "CGeneticStrategyCLv2.h"
#include <iostream>
#include <sstream>
#include "SDKUtil/include/SDKCommon.hpp"
#include "CAutomatImpl.h"
#include "CTest.h"
#include "CRandomImpl.h"
#include <boost/algorithm/string/predicate.hpp>
#include "Tools/helper.hpp"
#include "Tools/StringProcessorSimple.h"
#include <sstream>

void CGeneticStrategyCLv2::initMemory()
{
    sizes = (unsigned int*)malloc( 5*4 );
    srands = (unsigned int*)malloc( 5*4 );
}

void CGeneticStrategyCLv2::initCLBuffers()
{
    size_t bufSize = ( 2*statesCount*stateSize + 4 )*N*M;
    buffer = (char*)malloc( bufSize );
    buffer2 = (char*)malloc( bufSize );

    CRandomImpl rand;
    for ( int i=0; i < N*M; ++i )
    {
        char * buf = buffer + i*(2*statesCount*stateSize + 4);
        CAutomatImpl::fillRandom( states, actions, buf, stateSize, &rand );
    }

    statesBufCL1 = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
    statesBufCL2 = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
    globalTmpBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);

    srandBuf = cl::Buffer( context, CL_MEM_READ_WRITE, 5*4 );
    sizesBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 5*4);
    resultCache = cl::Buffer( context, CL_MEM_READ_WRITE, 2*N*M*sizeof(float) );
    cachedResults = (float*)malloc( 2*N*M*sizeof(float) );
}

void CGeneticStrategyCLv2::createProgram( const boost::filesystem3::path& sourceFile, const std::string& params )
{
    // build the program from the source in the file
    std::string input;
    //Tools::StringProcessorSimple strProc(vals);
    Tools::readFileToString( input, sourceFile, &logger );
    
    cl::Program::Sources source;
    source.push_back( std::make_pair( input.c_str(), input.size() ) );
    cl::Program program(context, source);

    try { //compiling OpenCL source
        cl_int res = program.build(devices, params.c_str() );
		logger << "[SUCCESS] Program compiled.\n";
    } catch(cl::Error) {
        std::stringstream ss;
        ss << program.getBuildInfo< CL_PROGRAM_BUILD_LOG >( devices[0] );
        throwDetailedFailed( "File couldn't be compiled", ss.str().c_str(), &logger );
    }
	try { //Trying to create OpenCL kernel
		kernelGen = cl::Kernel( program, "genetic_2d" );
		logger << "[SUCCESS] Kernel created.\n";
	} catch(cl::Error& err) {
        throwDetailedFailed( "Failed to create kernel", streamsdk::getOpenCLErrorCodeStr( err.err() ), &logger );
	}
}

void CGeneticStrategyCLv2::countRanges( std::string& options )
{
    uint val;
    devices[0].getInfo( CL_DEVICE_MAX_WORK_GROUP_SIZE, &val );
    cl_ulong lMemSize;
    devices[0].getInfo( CL_DEVICE_LOCAL_MEM_SIZE, &lMemSize );
    cl_ulong autSize = 4 + 2*16*statesCount;
    cl_ulong nMemSize = val * autSize;
    localRange = cl::NullRange;
    globalRange = cl::NDRange( N, M );
    int v_work_group_size = val;
    cl_ulong v_buffer = val*(autSize / 4);
    std::stringstream ss(std::stringstream::out);
    if ( lMemSize < nMemSize )
    {
       /* cl_ulong res = lMemSize / autSize;
        std::vector<int> Ndels, Mdels;
        for ( int i=1; i<=(std::max)( N, M )/2; ++i ) 
        {
            if ( N % i == 0 )
                Ndels.push_back(i);
            if ( M % i == 0 )
                Mdels.push_back(i);
        }
        int maxA = 1, maxB = 1;
        for ( size_t i=0; i<Ndels.size(); ++i )
            for ( size_t j=0; j<Mdels.size(); ++j )
            {
                int a = Ndels[i], b = Mdels[j];
                if ( (2*a + a*b*(1 + autSize/4))*4 < lMemSize )
                {
                    if ( a*b > maxA*maxB )
                    {
                        maxA = a;
                        maxB = b;
                    }else if ( a*b == maxA*maxB && std::abs(a - b) < std::abs( maxA - maxB ) )
                    {
                        maxA = a;
                        maxB = b;
                    }
                }
            }
            localRange = cl::NDRange( maxA, maxB );
            v_linear = maxA;
            v_squared = maxA * maxB;*/
            //v_buffer = maxA*maxB*(size_t)(autSize / 4);
        ss << " -D __check_space=__global";
    }else
    {
        ss << " -D ENOUGH_LOCAL_MEMORY";
        ss << " -D __check_space=__local";
    }
    
    ss << " -D WORK_GROUP_SIZE=" << v_work_group_size;
    ss << " -D BUFFER_SIZE=" << v_buffer;
    if ( deviceType == CL_DEVICE_TYPE_CPU )
        ss << " -D CPU_VERSION";
    options = ss.str();
}

CGeneticStrategyCLv2::CGeneticStrategyCLv2( const boost::filesystem3::path& source, CStateContainer* states, CActionContainer* actions, 
                                       CLabResultMulti* res, CAntFitnesPtr fitnes, const std::vector< std::string >& strings, Tools::Logger& logger )
    :CGeneticStrategyCommon(states, actions, res, fitnes, strings, logger), mapsBuffer(0), buffer(0)
{
	logger << "[INIT] Initializing CGeneticStrategyCLv2.\n";
    m_pRandom = CRandomPtr( new CRandomImpl() );
    setFromStrings( strings, m_pRandom );
    statesCount = states->size();
    stateSize = 16;//1 << statesCount;
    initMemory();
    try
    {
		logger << "[INIT] Trying to get specified device.\n";
        context = streamsdk::getContext( deviceType, platformInfo );
        devices = context.getInfo<CL_CONTEXT_DEVICES>();
        queue = cl::CommandQueue( context, devices[0] );
        std::string options;
        countRanges( options );

        createProgram( source, options );
        initCLBuffers();
		logger << "[SUCCES] CGeneticStrategyCLv2 created.\n";
    }catch ( cl::Error& error ) 
    {
        Tools::throwDetailedFailed( "Failed to create CGeneticStrategyCLv2", streamsdk::getOpenCLErrorCodeStr( error.err() ), &logger );
    }catch ( std::runtime_error& err )
    {
        throw std::runtime_error( std::string( "Failed to create GeneticStrategy: " ).append(
            err.what() ));
    }
}

void CGeneticStrategyCLv2::setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand )
{
    gensToCount = 1;
    M = N = 0;
    deviceType = CL_DEVICE_TYPE_GPU;
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
            continue;
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
        if ( boost::starts_with( str, "device" ) )
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            if ( tmp[0] == 'C' )
                deviceType = CL_DEVICE_TYPE_CPU;
            continue;
        }
    }
    invoker = new CInvoker( this, error );
    if (( M <= 0 ) || ( N <= 0 ))
        Tools::throwFailed( "Failed to create CGeneticStrategyCLv2", &logger );
}

void CGeneticStrategyCLv2::preStart()
{
    
    size_t mapSize = (2 + maps[0]->width()*maps[0]->height() );
    size_t bufSize = ( 2*statesCount*stateSize + 4 )*N*M;
    
    try
    {
        kernelGen.setArg( 0, statesBufCL1 );
        kernelGen.setArg( 1, statesBufCL2 );
        kernelGen.setArg( 2, sizesBuf );
        kernelGen.setArg( 3, srandBuf );
        kernelGen.setArg( 4, mapBufCL );
        kernelGen.setArg( 5, mapCL );
        kernelGen.setArg( 6, resultCache );
        kernelGen.setArg( 7, globalTmpBuffer );

        sizes[0] = statesCount;
        sizes[1] = stateSize;
        sizes[3] = gensToCount;

        CRandomImpl random;
        srands[0] = random.nextUINT();
        srands[1] = 0;

        queue.enqueueWriteBuffer( statesBufCL1, CL_TRUE, 0, bufSize, buffer );
        queue.enqueueWriteBuffer( mapCL, CL_TRUE, 0, mapSize*4, mapsBuffer );
        queue.enqueueWriteBuffer( sizesBuf, CL_TRUE, 0, 5*4, sizes );
        queue.enqueueWriteBuffer( srandBuf, CL_TRUE, 0, 5*4, srands );

    }catch( cl::Error& error )
    {
        Tools::throwDetailedFailed( "[FAILED] to set arguments", streamsdk::getOpenCLErrorCodeStr( error.err() ), &logger );
    }
}

void CGeneticStrategyCLv2::setMaps( std::vector<CMapPtr> maps )
{
    this->maps = maps;

    size_t mapSize = 2 + maps[0]->width()*maps[0]->height();
    size_t mapSizes = N*M*( mapSize );
    mapBufCL = cl::Buffer( context, CL_MEM_READ_WRITE, mapSizes*4 );
    mapCL = cl::Buffer( context, CL_MEM_READ_ONLY, mapSize*4 );
    mapsBuffer = (int*)malloc( mapSize*4 );
    maps[0]->toIntBuffer( mapsBuffer );
    sizes[2] = mapSize;

    preStart();
}

void CGeneticStrategyCLv2::run()
{
    nextGeneration( m_pRandom.get() );
}

void CGeneticStrategyCLv2::nextGeneration( CRandom* rand )
{
    size_t bufSize = ( 2*statesCount*stateSize + 4 )*N*M;
    size_t cacheSize = N*M*sizeof(float);
    try
    {
        //queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NullRange );
        boost::this_thread::disable_interruption di;
        queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, globalRange, localRange );

        queue.finish();
       
        queue.enqueueReadBuffer( statesBufCL2, CL_FALSE, 0, bufSize, buffer );
        queue.enqueueReadBuffer( statesBufCL1, CL_FALSE, 0, bufSize, buffer2 );
        queue.enqueueReadBuffer( resultCache, CL_FALSE, 0, 2*cacheSize, cachedResults );
    }catch( cl::Error& error )
    {
        Tools::throwDetailedFailed( "[FAILED] Failed to get next generation", streamsdk::getOpenCLErrorCodeStr( error.err() ), &logger );
    }catch( std::exception& ex )
    {
        int k = 0;
        throw ex;
    }

    //ToDo: count buffer, buffer2
    boost::mutex& mutex = result->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    queue.finish();
    addGeneration( buffer2, cachedResults );
    addGeneration( buffer, cachedResults + N*M );
}

void CGeneticStrategyCLv2::addGeneration( char* buff, float* results )
{
    double sum = 0.0;
    size_t bestPos = 0;
    for (int i=0; i < N; ++i )
        for (int j=0; j < M; ++j)
        {
            sum += results[i*M + j];
            if (results[i*M + j] > results[bestPos])
                bestPos = i*M + j;
        }
    size_t autSize = ( 2*statesCount*stateSize + 4);
    result->addGeneration( CAutomatPtr( 
                new CAutomatImpl( CAutomatImpl::createFromBuffer(states, actions, buff + bestPos*autSize ) ) )
                , results[bestPos], sum/(N*M) );
}

CGeneticStrategyCLv2::~CGeneticStrategyCLv2()
{
	if ( buffer != 0 )
		free( buffer );
    if ( buffer2 != 0 )
		free( buffer2 );
    free( mapsBuffer );
    free( cachedResults );
    free( sizes );
    free( srands );
    if ( invoker != 0 )
        delete invoker;
}

std::string CGeneticStrategyCLv2::getDeviceType() const
{
    if ( deviceType == CL_DEVICE_TYPE_CPU )
    {
        return "OpenCL on CPU, " + platformInfo.sName;
    }
    return "OpenCL on GPU, " + platformInfo.sName;
}

const CMapPtr CGeneticStrategyCLv2::getMap( size_t i )
{
    return CGeneticStrategyCommon::getMap(i);
}

size_t CGeneticStrategyCLv2::getMapsCount()
{
    return CGeneticStrategyCommon::getMapsCount();
}

void CGeneticStrategyCLv2::pushResults()
{
    CGeneticStrategyCommon::pushResults();
}

CInvoker* CGeneticStrategyCLv2::getInvoker() const
{
    return CGeneticStrategyCommon::getInvoker();
}

size_t CGeneticStrategyCLv2::getN() const
{
    return CGeneticStrategyCommon::getN();
}

size_t CGeneticStrategyCLv2::getM() const
{
    return CGeneticStrategyCommon::getM();
}

const boost::exception_ptr& CGeneticStrategyCLv2::getError() const
{
    return CGeneticStrategyCommon::getError();
}

double CGeneticStrategyCLv2::getAvgFitness() const
{
    return 0.0;
}
double CGeneticStrategyCLv2::getMaxFitness() const
{
    return 0.0;
}
CAutomatPtr CGeneticStrategyCLv2::getBestIndivid() const
{
    return CAutomatPtr();
}