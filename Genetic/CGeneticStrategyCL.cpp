#include "stdafx.h"
#include "CGeneticStrategyCL.h"
#include "CL/cl.h"
#include <iostream>
#include <fstream>
#include "SDKUtil/include/SDKCommon.hpp"
#include "CAutomatImpl.h"
#include "CMapImpl.h"
#include "CTest.h"
#include "CRandomImpl.h"
#include <boost/algorithm/string/predicate.hpp>
#include "SDKUtil/SDKPlatform.hpp"


void CGeneticStrategyCL::pushResults()
{
    boost::mutex& mutex = result->getMutex();
    boost::mutex::scoped_lock lock(mutex);
    result->addGeneration( getBestIndivid(), getMaxFitness(), getAvgFitness() );
}

void CGeneticStrategyCL::initMemory()
{
    cache = (float*)malloc( N*M*sizeof(float) );

    bestResults = (float*)malloc( gensToCount*sizeof(float) );
    sumResults = (float*)malloc( gensToCount*sizeof(float) );
    bestIndivids = (char*)malloc( gensToCount*( 2*statesCount*stateSize + 4 ) );
    sizes = (unsigned int*)malloc( 5*4 );
    srands = (unsigned int*)malloc( 5*4 );
}

void CGeneticStrategyCL::initCLBuffers()
{
    size_t bufSize = ( 2*statesCount*stateSize + 4 )*N*M;
    buffer = (char*)malloc( bufSize );

    CRandomImpl rand;
    for ( int i=0; i < N*M; ++i )
    {
        char * buf = buffer + i*(2*statesCount*stateSize + 4);
        CAutomatImpl::fillRandom( states, actions, buf, stateSize, &rand );
    }

    statesBufCL1 = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
    statesBufCL2 = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
    tempBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
    cacheBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, N*M*sizeof(float) );

    srandBuf = cl::Buffer( context, CL_MEM_READ_WRITE, 5*4 );
    sizesBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 5*4);
    bestResult = cl::Buffer(context, CL_MEM_WRITE_ONLY, gensToCount*sizeof(float) );
    sumResult = cl::Buffer(context, CL_MEM_WRITE_ONLY, gensToCount*sizeof(float) );
    bestIndivid = cl::Buffer(context, CL_MEM_WRITE_ONLY, gensToCount*( 2*statesCount*stateSize + 4 ) );

   // startStateBufCL1 = cl::Buffer(context, CL_MEM_READ_WRITE, N*N);
   // startStateBufCL2 = cl::Buffer(context, CL_MEM_READ_WRITE, N*N);
}

void CGeneticStrategyCL::createProgram()
{
    // build the program from the source in the file
	logger << "[INIT] Start building program.\n";
    using namespace std;
    ifstream file("gen.c");
    if ( !file.is_open() )
    {
        OutputDebugStringW( L"file not found\n" );
		logger << "[FAILED] File not found.\n";
		return;
    }
   /* string prog(istreambuf_iterator<char>(file),
        (istreambuf_iterator<char>()));
    cl::Program::Sources source( 1, make_pair(prog.c_str(),
        prog.length()+1));*/
    cl::Program::Sources source;
    std::vector< std::string > strs;
    std::string input;
    while ( file.good() )
    {
        char str[300];
        file.getline( str, 290 );
        size_t len = strlen( str );
        
        std::string s( str );
        input.append( s );
        input.append("\n");
        //strs.push_back( s );
        //source.push_back( make_pair( strs[strs.size()-1].c_str(), s.size()-1 ) );
    }
    source.push_back( make_pair( input.c_str(), input.size() ) );
    cl::Program program(context, source);
    file.close();

    try {
        cl_int res = program.build(devices);
		logger << "[SUCCESS] Program compiled.\n";
    } catch(cl::Error& err) {
        OutputDebugStringW( L"file: gen.txt couldn't be compiled\n" );
		logger << "[FAILED] Can't compile file.\n";
        OutputDebugStringA( streamsdk::getOpenCLErrorCodeStr( err.err() ) );
        OutputDebugStringA( "\n" );
        stringstream ss;
        ss << program.getBuildInfo< CL_PROGRAM_BUILD_LOG >( devices[0] );
        string s = ss.str();
		logger << "[DESCRIPTION] " << s.c_str() << "\n";
        OutputDebugStringA( s.c_str() );
    }
	try {
		kernelGen = cl::Kernel( program, "genetic_2d" );
		logger << "[SUCCESS] Kernel created.\n";
	} catch(cl::Error& err) {
        OutputDebugStringW( L"file: can't create context\n" );
		logger << "[FAILED] Can't create kernel.\n";
		logger << "[DESCRIPTION] " << streamsdk::getOpenCLErrorCodeStr( err.err() ) << "\n";
        OutputDebugStringA( streamsdk::getOpenCLErrorCodeStr( err.err() ) );
        OutputDebugStringA( "\n" );
	}
    
}

CGeneticStrategyCL::CGeneticStrategyCL(CStateContainer* states, CActionContainer* actions, 
                                       CLabResultMulti* res, const std::vector< std::string >& strings, Tools::Logger& logger )
:states(states), actions(actions), result(res), mapsBuffer(0), invoker(0), buffer(0), logger(logger)
{
	logger << "[INIT] Initializing CGeneticLaboratryCL.\n";
    CRandomPtr rand( new CRandomImpl() );
    setFromStrings( strings, rand );
    statesCount = states->size();
    stateSize = 16;//1 << statesCount;
    initMemory();
    try
    {
		logger << "[INIT] Trying to get specified device.\n";
        cl::vector< cl::Platform > platformList;
        cl::Platform::get(&platformList);

        for ( size_t i=0; i<platformList.size(); ++i )
        {
            SDKPlatform platform(platformList[i]);
            platform.getInfo( CL_PLATFORM_PROFILE, &sProfile );
            platform.getInfo( CL_PLATFORM_VENDOR, &sVendor );
            platform.getInfo( CL_PLATFORM_VERSION, &sVersion );
            platform.getInfo( CL_PLATFORM_NAME, &sName );

            cl_context_properties cprops[3] = 
            {CL_CONTEXT_PLATFORM, 
            (cl_context_properties)(platformList[i])(), 0};

            int n = 0;
            if ( deviceType == GPU )
                n = platform.getDevicesCnt( CL_DEVICE_TYPE_GPU );
            else
                n = platform.getDevicesCnt( CL_DEVICE_TYPE_CPU );
            if ( n == 0 )
                continue;

            if ( deviceType == GPU )
                context = cl::Context(CL_DEVICE_TYPE_GPU, cprops);
            else
                context = cl::Context(CL_DEVICE_TYPE_CPU, cprops);
			logger << "[SUCCESS] Creating Context.\n";
            break;
        }

        devices = context.getInfo<CL_CONTEXT_DEVICES>();
        createProgram();

        //cl::vector<cl::Device> devices; 
        //devices[0] = cl::Device( context.getInfo<CL_CONTEXT_DEVICES>() );
        queue = cl::CommandQueue( context, devices[0] );
        
        initCLBuffers();
		logger << "[SUCCES] CGeneticStrategyCL created.\n";
    }catch ( cl::Error& error ) 
    {
		logger << "[FAILED] " << streamsdk::getOpenCLErrorCodeStr( error.err() ) << "\n";
        OutputDebugStringA( error.what() );
        OutputDebugStringA( streamsdk::getOpenCLErrorCodeStr( error.err() ) );
    }
}

void CGeneticStrategyCL::setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand )
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
        if ( boost::starts_with( str, "device" ) )
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            if ( tmp[0] == 'C' )
                deviceType = CPU;
            continue;
        }
    }
    
    invoker = new CInvoker( this, rand );
    
    if (( M <= 0 ) || ( N <= 0 ))
    {
		logger << "[FAILED] Bad config!\n";
        throw std::exception( "bad config!" );
    }
}

CGeneticStrategyCL::~CGeneticStrategyCL()
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

void CGeneticStrategyCL::preStart()
{
    
    size_t mapSize = (2 + maps[0]->width()*maps[0]->height() );
    size_t bufSize = ( 2*statesCount*stateSize + 4 )*N*M;
    //char * ptr = (char*) malloc( N*N*mapSize );//mapsBuffer ;
    
    /*for ( int i=0; i<N*N; ++i )
    {
        memcpy( ptr+i*mapSize, mapsBuffer, mapSize );
    }*/
    
    try
    {
        //queue.enqueueWriteBuffer( mapBufCL, CL_TRUE, 0, N*N*mapSize, ptr );
        //queue.enqueueWriteBuffer( startStateBufCL1, CL_TRUE, 0, N*N, startBuf );
        //queue.enqueueWriteBuffer( mapCL, CL_TRUE, 0, mapSize, mapsBuffer );

        kernelGen.setArg( 0, statesBufCL1 );
        kernelGen.setArg( 1, statesBufCL2 );
        kernelGen.setArg( 2, sizesBuf );
        kernelGen.setArg( 3, srandBuf );
        
        kernelGen.setArg( 4, mapBufCL );
        //kernelGen.setArg( 4, mapSize, 0 );
        //kernelGen.setArg( 5, tempBuffer );
        kernelGen.setArg( 5, bufSize, 0 );
        kernelGen.setArg( 6, mapCL );
        kernelGen.setArg( 7, bestResult );
        kernelGen.setArg( 8, sumResult );
        kernelGen.setArg( 9, bestIndivid );

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
		logger << "[FAILED] " << streamsdk::getOpenCLErrorCodeStr( error.err() ) << "\n";
        OutputDebugStringA( error.what() );
        OutputDebugStringA( streamsdk::getOpenCLErrorCodeStr( error.err() ) );
    }
}

void CGeneticStrategyCL::nextGeneration( CRandom* rand )
{
    try
    {
        queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NDRange( N, M ) );
        queue.finish();
       
        queue.enqueueReadBuffer( bestResult, CL_TRUE, 0, gensToCount*sizeof(float), bestResults );
        queue.enqueueReadBuffer( sumResult, CL_TRUE, 0, gensToCount*sizeof(float), sumResults );
        queue.enqueueReadBuffer( bestIndivid, CL_TRUE, 0, ( 2*statesCount*stateSize + 4)*gensToCount, bestIndivids );
    }catch( cl::Error& error )
    {
		logger << "[FAILED] " << streamsdk::getOpenCLErrorCodeStr( error.err() ) << "\n";
        OutputDebugStringA( error.what() );
        OutputDebugStringA( streamsdk::getOpenCLErrorCodeStr( error.err() ) );
    }

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

void CGeneticStrategyCL::swapBuffers()
{
    cl::Buffer tmp = statesBufCL2;
    statesBufCL2 = statesBufCL1;
    statesBufCL1 = tmp;
}

void CGeneticStrategyCL::setMaps( std::vector<CMapPtr> maps )
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
    //pushResults();
}

const CMapPtr CGeneticStrategyCL::getMap( size_t i )
{
    CMapImpl* map = new CMapImpl( maps.at(i).get() );
    return CMapPtr( map );
}

size_t CGeneticStrategyCL::getMapsCount()
{
    return maps.size();
}

double CGeneticStrategyCL::getAvgFitness() const
{
    /*double sum = 0.0;
    for ( int i=0; i<N*M; ++i )
        sum += cache[i];
    return sum/(N*N);*/
    return 0;
}

CAutomatPtr CGeneticStrategyCL::getBestIndivid() const
{
    /*size_t max_coord = 0;
    for ( int i=1; i<N*M; ++i )
        if ( cache[i] > cache[max_coord] )
            max_coord = i;

    /*double res3 = 0.0;
    for ( size_t i=1; i<N*N; ++i )
    {
        CMapImpl tmpMap( maps[0].get() );
        double r = CTest::run( automats[i].get(),  &tmpMap );
        if ( r > res3 )
            res3 = r;
    }

    CMapImpl tmpMap( maps[0].get() );
    double res = CTest::run( automats[max_coord].get(),  &tmpMap );//, 100, steps2 );
 /*   for ( int i=0; i<100; ++i )
    {
        int a1 = steps[max_coord*200 + i];
        int a2 = steps2[i];
        if ( a1 != a2 )
        {
            int k = 1;
            ++k;
        }
    }*/
    /*double res2 = getMaxFitness();
    if (( std::abs(res - res2) > 1.0 ) || ( std::abs(res - res3) > 1.0 ))
    {
        int k = 1;
        ++k;
    }*/
    
    return curIndivid;
}

double CGeneticStrategyCL::getMaxFitness() const
{
    /*size_t max_coord = 0;
    for ( size_t i=1; i<N*M; ++i )
        if ( cache[i] > cache[max_coord] )
            max_coord = i;
    return cache[max_coord];    */
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