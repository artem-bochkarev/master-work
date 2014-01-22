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
#include <boost/lexical_cast.hpp>

void CGeneticStrategyCL::initMemory()
{
    cache = (float*)malloc( N*M*sizeof(float) );

    bestResults = (float*)malloc( gensToCount*sizeof(float) );
    sumResults = (float*)malloc( gensToCount*sizeof(float) );
    bestIndivids = (char*)malloc( gensToCount*( 2 * pAntCommon->statesCount() * stateSize + 4 ) );
    sizes = (unsigned int*)malloc( 5*4 );
    srands = (unsigned int*)malloc( 5*4 );
}

void CGeneticStrategyCL::initCLBuffers()
{
	size_t bufSize = (2 * pAntCommon->statesCount() * stateSize + 4)*N*M;
    buffer = (char*)malloc( bufSize );

    CRandomImpl rand;
    for ( int i=0; i < N*M; ++i )
    {
		char * buf = buffer + i*(2 * pAntCommon->statesCount() * stateSize + 4);
        CAutomatImpl::fillRandom( pAntCommon.get(), buf, &rand );
    }

    statesBufCL1 = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
    statesBufCL2 = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
    tempBuffer = cl::Buffer(context, CL_MEM_READ_WRITE, bufSize);
    cacheBuffer = cl::Buffer(context, CL_MEM_WRITE_ONLY, N*M*sizeof(float) );

    srandBuf = cl::Buffer( context, CL_MEM_READ_WRITE, 5*4 );
    sizesBuf = cl::Buffer(context, CL_MEM_READ_ONLY, 5*4);
    bestResult = cl::Buffer(context, CL_MEM_WRITE_ONLY, gensToCount*sizeof(float) );
    sumResult = cl::Buffer(context, CL_MEM_WRITE_ONLY, gensToCount*sizeof(float) );
	bestIndivid = cl::Buffer(context, CL_MEM_WRITE_ONLY, gensToCount*(2 * pAntCommon->statesCount() * stateSize + 4));

    maxCache = cl::Buffer(context, CL_MEM_READ_WRITE, M*N*4);
    bestPos = cl::Buffer(context, CL_MEM_READ_WRITE, M*N*4);
    avgCache = cl::Buffer(context, CL_MEM_READ_WRITE, M*N*4);

   // startStateBufCL1 = cl::Buffer(context, CL_MEM_READ_WRITE, N*N);
   // startStateBufCL2 = cl::Buffer(context, CL_MEM_READ_WRITE, N*N);
}

void CGeneticStrategyCL::processString( std::string& str, const std::vector<size_t>& vals ) const
{
    int i = str.find("#define");
    if ( i >= 0 )
    {
        int j = str.find("max_linear_size");
        if ( j > i )
        {
            if ( vals[0] > 0 )
                str = "#define max_linear_size " + boost::lexical_cast<std::string, size_t>(vals[0]) + "\n";
            return;
        }
        j = str.find("max_squared_size");
        if ( j > i )
        {
            if ( vals[1] > 0 )
                str = "#define max_squared_size " + boost::lexical_cast<std::string, size_t>(vals[1]) + "\n";
            return;
        }
        j = str.find("max_tmp_buffer_size");
        if ( j > i )
        {
            if ( vals[2] > 0 )
                str = "#define max_tmp_buffer_size " + boost::lexical_cast<std::string, size_t>(vals[2]) + "\n";
            return;
        }
    }
}

void CGeneticStrategyCL::createProgram( const std::vector<size_t>& vals )
{
    // build the program from the source in the file
	logger << "[INIT] Start building program.\n";
    using namespace std;
    ifstream file("gen.c");
    if ( !file.is_open() )
    {
        OutputDebugStringW( L"file not found\n" );
		logger << "[FAILED] File not found.\n";
		throw std::runtime_error( "File not found" );
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
        processString( s, vals );
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
        throw std::runtime_error( "Failed to compile Program" );
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
        throw std::runtime_error( "Failed to create Kernel" );
	}
    
}

CGeneticStrategyCL::CGeneticStrategyCL(AntCommonPtr pAntCommon, CLabResultMulti* res, CAntFitnesPtr fitnes,
	const std::vector< std::string >& strings, Tools::Logger& logger )
:CGeneticStrategyCommon(pAntCommon, res, fitnes, strings, logger), mapsBuffer(0), buffer(0)
{
	logger << "[INIT] Initializing CGeneticLaboratryCL.\n";

    m_pRandom = CRandomPtr( new CRandomImpl());
    setFromStrings( strings, m_pRandom );
    stateSize = 16;//1 << statesCount;
    initMemory();
    try
    {
		logger << "[INIT] Trying to get specified device.\n";
        std::vector< cl::Platform > platformList;
        cl::Platform::get(&platformList);

        for ( size_t i=0; i<platformList.size(); ++i )
        {
            //SDKPlatform platform(platformList[i]);
			platformList[i].getInfo(CL_PLATFORM_PROFILE, &sProfile);
			platformList[i].getInfo(CL_PLATFORM_VENDOR, &sVendor);
			platformList[i].getInfo(CL_PLATFORM_VERSION, &sVersion);
			platformList[i].getInfo(CL_PLATFORM_NAME, &sName);

            cl_context_properties cprops[3] = 
            {CL_CONTEXT_PLATFORM, 
            (cl_context_properties)(platformList[i])(), 0};

            cl_uint n = 0;
            if ( deviceType == GPU )
				::clGetDeviceIDs(platformList[i](), CL_DEVICE_TYPE_GPU, 0, NULL, &n);
                //n = platformList[i].getDevices( CL_DEVICE_TYPE_GPU );
            else
				::clGetDeviceIDs(platformList[i](), CL_DEVICE_TYPE_CPU, 0, NULL, &n);
				//n = platformList[i].getDevices(CL_DEVICE_TYPE_CPU);
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

        //cl::vector<cl::Device> devices; 
        //devices[0] = cl::Device( context.getInfo<CL_CONTEXT_DEVICES>() );
        queue = cl::CommandQueue( context, devices[0] );
        uint val;
        devices[0].getInfo( CL_DEVICE_MAX_WORK_GROUP_SIZE, &val );
        cl_ulong lMemSize;
        devices[0].getInfo( CL_DEVICE_LOCAL_MEM_SIZE, &lMemSize );
		cl_ulong autSize = 4 + 2 * 16 * pAntCommon->statesCount();
        cl_ulong nMemSize = val * autSize;
        localRange = cl::NullRange;
        globalRange = cl::NDRange( N, M );
        std::vector<size_t> vals;
        vals.assign(3, 0);
        if ( lMemSize < nMemSize )
        {
            cl_ulong res = lMemSize / autSize;
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
            vals[0] = maxA;
            vals[1] = maxA * maxB;
            vals[2] = maxA*maxB*(size_t)(autSize / 4);
        }

        createProgram( vals );
        initCLBuffers();
		logger << "[SUCCES] CGeneticStrategyCL created.\n";
    }catch ( cl::Error& error ) 
    {
		logger << "[FAILED] " << streamsdk::getOpenCLErrorCodeStr( error.err() ) << "\n";
        OutputDebugStringA( error.what() );
        OutputDebugStringA( streamsdk::getOpenCLErrorCodeStr( error.err() ) );
        throw std::runtime_error( std::string("Failed to create GeneticStrategy: ").append( 
            streamsdk::getOpenCLErrorCodeStr( error.err() )) );
    }catch ( std::runtime_error& err )
    {
        throw std::runtime_error( std::string( "Failed to create GeneticStrategy: " ).append(
            err.what() ));
    }
}

void CGeneticStrategyCL::setFromStrings( const std::vector< std::string >& strings, CRandomPtr rand )
{
    gensToCount = 1;
    M = N = 0;
    deviceType = GPU;
    mode = NEW;
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
                deviceType = CPU;
            continue;
        }
        if ( boost::starts_with( str, "compute" ) )
        {
            int b = str.find( "=" );
            ++b;
            int e = str.find( ";" );
            const std::string tmp( str.substr( b, e ) );
            if ( tmp.find("old") != -1 || tmp.find("one") != -1 )
                mode = OLD;
            continue;
        }
    }
    
    invoker = new CInvoker( this, error );
    
    if (( M <= 0 ) || ( N <= 0 ))
    {
		logger << "[FAILED] Bad config!\n";
        throw std::runtime_error( "bad config!" );
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
	size_t bufSize = (2 * pAntCommon->statesCount() * stateSize + 4)*N*M;
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
        //kernelGen.setArg( 5, bufSize, 0 );
        if ( mode == NEW )
        {
            kernelGen.setArg( 5, maxCache );
            kernelGen.setArg( 6, avgCache );
            kernelGen.setArg( 7, bestPos );
            kernelGen.setArg( 8, mapCL );
            kernelGen.setArg( 9, bestResult );
            kernelGen.setArg( 10, sumResult );
            kernelGen.setArg( 11, bestIndivid );
        }else
        {
            kernelGen.setArg( 5, bufSize, 0 );
            kernelGen.setArg( 6, mapCL );
            kernelGen.setArg( 7, bestResult );
            kernelGen.setArg( 8, sumResult );
            kernelGen.setArg( 9, bestIndivid );
        }

		sizes[0] = pAntCommon->statesCount();
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
        logger << "[FAILED] to set arguments: " << streamsdk::getOpenCLErrorCodeStr( error.err() ) << "\n";
        OutputDebugStringA( error.what() );
        OutputDebugStringA( streamsdk::getOpenCLErrorCodeStr( error.err() ) );
        throw std::runtime_error( std::string( "Failed to set arguments: " ).append (
            streamsdk::getOpenCLErrorCodeStr( error.err() ) ));
    }
}

void CGeneticStrategyCL::run()
{
    nextGeneration( m_pRandom.get() );
}

void CGeneticStrategyCL::nextGeneration( CRandom* rand )
{
    try
    {
        //queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NullRange );
        boost::this_thread::disable_interruption di;
        if ( mode == NEW )
            queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, globalRange, localRange );
        else
            queue.enqueueNDRangeKernel( kernelGen, cl::NullRange, cl::NDRange(N, M), cl::NDRange(N, M) );
        queue.finish();
       
        queue.enqueueReadBuffer( bestResult, CL_TRUE, 0, gensToCount*sizeof(float), bestResults );
        queue.enqueueReadBuffer( sumResult, CL_TRUE, 0, gensToCount*sizeof(float), sumResults );
		queue.enqueueReadBuffer(bestIndivid, CL_TRUE, 0, (2 * pAntCommon->statesCount() * stateSize + 4)*gensToCount, bestIndivids);
    }catch( cl::Error& error )
    {
		logger << "[FAILED] " << streamsdk::getOpenCLErrorCodeStr( error.err() ) << "\n";
        OutputDebugStringA( error.what() );
        OutputDebugStringA( streamsdk::getOpenCLErrorCodeStr( error.err() ) );
        throw std::runtime_error( std::string( "Failed to get next generation: " ).append ( 
            streamsdk::getOpenCLErrorCodeStr( error.err() ) ));
    }catch( std::exception& ex )
    {
        int k = 0;
        throw ex;
    }

    boost::mutex& mutex = result->getMutex();
    boost::mutex::scoped_lock lock(mutex);
	size_t autSize = (2 * pAntCommon->statesCount() * stateSize + 4);
    for ( size_t i=0; i<gensToCount; ++i)
    {
        curIndivid = CAutomatImpl::createFromBuffer(pAntCommon.get(), bestIndivids + i*autSize );
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

std::string CGeneticStrategyCL::getDeviceType() const
{
    if ( deviceType == CPU )
    {
        return "OpenCL on CPU, " + sName;
    }
    return "OpenCL on GPU, " + sName;
}

double CGeneticStrategyCL::getAvgFitness() const
{
    return 0.0;
}
double CGeneticStrategyCL::getMaxFitness() const
{
    return 0.0;
}
CAutomatPtr CGeneticStrategyCL::getBestIndivid() const
{
    return CAutomatPtr();
}

const CMapPtr CGeneticStrategyCL::getMap( size_t i )
{
    return CGeneticStrategyCommon::getMap(i);
}

size_t CGeneticStrategyCL::getMapsCount()
{
    return CGeneticStrategyCommon::getMapsCount();
}

void CGeneticStrategyCL::pushResults()
{
    CGeneticStrategyCommon::pushResults();
}

CInvoker* CGeneticStrategyCL::getInvoker() const
{
    return CGeneticStrategyCommon::getInvoker();
}

size_t CGeneticStrategyCL::getN() const
{
    return CGeneticStrategyCommon::getN();
}

size_t CGeneticStrategyCL::getM() const
{
    return CGeneticStrategyCommon::getM();
}

const boost::exception_ptr& CGeneticStrategyCL::getError() const
{
    return CGeneticStrategyCommon::getError();
}