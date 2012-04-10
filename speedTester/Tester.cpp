#include "Tester.h"
#include <iostream>

#include "../Genetic/CTest.h"
#include "../Genetic/CLaboratoryMulti.h"
#include "../Genetic/CActionContainerImpl.h"
#include "../Genetic/CStateContainerImpl.h"
#include "../Genetic/CLaboratoryFactory.h"
#include "../Genetic/CMapFactory.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

typedef boost::tokenizer<boost::char_separator<char> > 
    tokenizer;


Tester::Tester( const char *inFileName, const char *outFileName, Tools::Logger& logger)
:in(inFileName), pOut( new std::ofstream(outFileName)), logger(logger), timeSec(30)
{

}

void Tester::run()
{
    using namespace boost::filesystem3;
    path config_path("config.txt");
    path config_tmp_path("config_tmp.txt");
    remove( config_tmp_path );
    bool wasFile = exists( config_path );
    if ( wasFile )
        copy_file( config_path, config_tmp_path);

    path gen_path("gen.c");
    path gen_tmp_path("gen_tmp.c");
    remove( gen_tmp_path );
    bool wasFile2 = exists( gen_path );
    if ( wasFile2 )
        copy_file( gen_path, gen_tmp_path);

    while ( in.good() )
    {
        try
        {
            char buf[500];
            in.getline( buf, 490 );
            std::string str(buf);
            process( str );
        }catch( std::runtime_error& err )
        {
            std::cout << "[WARNING] " << err.what() << std::endl;
            logger << "[FAILED] " << err.what() << "\n";
        }catch( std::exception& ex )
        {
            std::cout << "[WARNING] " << ex.what() << std::endl;
            logger << "[FAILED] " << ex.what() << "\n";
        }catch( ... )
        {
            std::cout << "[WARNING] " << "unknown exception" << std::endl;
            logger << "[FAILED] " << "unknown exception" << "\n";
        }
    }

    remove( config_path );
    if ( wasFile )
        copy_file( config_tmp_path, config_path);
    remove( config_tmp_path );

    remove( gen_path );
    if ( wasFile2 )
        copy_file( gen_tmp_path, gen_path);
    remove( gen_tmp_path );
}

bool Tester::process( const std::string& in )
{
    std::vector<std::string> args;
    Commands cmd = parseStr( in, args );
    switch (cmd)
    {
    case RUN:
        {
            return runCmd( args );
        }break;
    case SET:
        {
            return setCmd( args );
        }break;
    case AR_MEAN:
        {
            return meanCmd( args );
        }break;
    }
    return false;
}

Tester::Commands Tester::parseStr( const std::string& in, std::vector<std::string>& args )
{
    args.clear();
    boost::char_separator<char> sep(" (,);");
    if ( in != "" )
    {
        tokenizer tokens(in, sep);
        tokenizer::iterator tok_iter = tokens.begin();
        std::string cmdName = *tok_iter;
        ++tok_iter;
        for ( ; tok_iter!= tokens.end(); ++tok_iter )
        {
            args.push_back( *tok_iter );
        }
        boost::algorithm::is_iequal iequals;
        std::string run("run");
        
        if ( cmdName == "RUN" )
            return RUN;
        if ( cmdName == "SET" )
            return SET;
        if ( cmdName == "AR_MEAN" )
            return AR_MEAN;
    }
    return ERROR;
    //throw (std::runtime_error("Illegal command"));
}

bool Tester::runCmd( std::vector<std::string> &args )
{
    CLaboratoryPtr laboratory;
    std::vector<CMapPtr> maps;
    if ( args.size() < 1 )
    {
        laboratory = CLaboratoryFactory::getLaboratory( "config.txt", logger );
    }else
    {
        laboratory = CLaboratoryFactory::getLaboratory( args[0].c_str(), logger );
    }
    CMapPtr map1;
    if ( args.size() < 2 )
    {
        map1 = CMapFactory::getMap( "map1.txt" );
    }else
    {
        map1 = CMapFactory::getMap( args[1].c_str() );
    }

    maps.push_back( map1 );
    laboratory->setMaps( maps );

    CGeneticStrategyPtr strategy = laboratory->getStrategy();
    std::cout << strategy->getDeviceType() << ", version=" << version <<
        ", states count: " << laboratory->getStates()->size() << ",  size: " << 
        strategy->getN() << "x" << strategy->getM();

    *pOut << strategy->getDeviceType() << ", version=" << version <<
        ", states count: " << laboratory->getStates()->size() << ",  size: " << 
        strategy->getN() << "x" << strategy->getM() << ",  result: ";

    try
    {
        laboratory->runForTime( timeSec * 1000 );

        std::cout << ",  result: " << 
            laboratory->getGenerationNumber() << std::endl;
        *pOut << laboratory->getGenerationNumber() << std::endl;
        logger << "[SUCCESS] " << "\n";
    }catch( std::runtime_error& err )
    {
        *pOut << "FAILED\n";
        throw err;
    }
    return true;
}

bool Tester::meanCmd( std::vector<std::string>& args )
{
    int cnt = boost::lexical_cast<int>( args[1] );
    std::vector<size_t> values(cnt);
    size_t sum = 0;
    
    std::cout << "Started mean test on " << args[0] << std::endl;
    *pOut << "mean test on \"" << args[0] << "\"" << std::endl;
    for ( int i=0; i<cnt; ++i )
    {
		CLaboratoryPtr laboratory;
        std::vector<CMapPtr> maps;
        if ( args.size() < 1 )
        {
            laboratory = CLaboratoryFactory::getLaboratory( "config.txt", logger );
        }else
        {
            laboratory = CLaboratoryFactory::getLaboratory( args[0].c_str(), logger );
        }
        CMapPtr map1;
        if ( args.size() > 2 )
        {
            map1 = CMapFactory::getMap( args[2].c_str() );
        }else
        {
            map1 = CMapFactory::getMap( "map1.txt" );
        }

        maps.push_back( map1 );
        laboratory->setMaps( maps );

        laboratory->runForTime( timeSec * 1000 );
        values[i] = laboratory->getGenerationNumber();
        sum += values[i];
        std::cout << i+1 << " ";
    }
    std::cout << std::endl;
    double mean = (double)sum / cnt;
    double dev = 0.0;
    for ( int i=0; i<cnt; ++i )
    {
        dev += (mean - values[i])*(mean - values[i]);
    }
    dev /= cnt;
    dev = sqrt( dev );
    std::cout << boost::format("    mean=%.2f  deviation=%.2f good=%.2f") % mean % dev % (100*(mean-dev)/mean) << "%" << std::endl;
    *pOut << boost::format("    mean=%.2f  deviation=%.2f good=%.2f") % mean % dev % (100*(mean-dev)/mean) << "%" << std::endl;
    return true;
}

bool Tester::setCmd( std::vector<std::string> &args )
{
    using namespace boost::filesystem3;
    boost::algorithm::is_iequal iequals;
    if ( args[0] == "TIME" )
    {
        timeSec = boost::lexical_cast<int>( args[1] );
        *pOut << "TIME=" << timeSec << std::endl;
        return true;
    }
    if ( args[0] == "VERSION" )
    {
        version = args[1];
        *pOut << "VERSION=" << args[1] << std::endl;
        return true;
    }
    if ( args[0] == "CLSOURCE" )
    {
        path oldPath("gen.c");
        path newGenPath = args[1];
        remove( oldPath );
        if ( exists( newGenPath ) )
            copy_file( newGenPath, oldPath );
        else throw std::runtime_error("Can't set new file, file not found");
        return true;
    }
    if ( args[0] == "OUTFILE" )
    {
        pOut->close();
        pOut = ofstreamPtr( new std::ofstream(args[1].c_str()));
        return true;
    }
    throw (std::runtime_error("Tester::setCmd : Illegal argument"));
}
