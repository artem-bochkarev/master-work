#include "Tester.h"
#include <iostream>

#include "CleverAnt3/CleverAnt3LaboratoryFactory.h"

#include <boost/tokenizer.hpp>
#include <boost/algorithm/string/compare.hpp>
#include <boost/filesystem.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>
#include "TimeRun/CTimeCounter.h"

typedef boost::tokenizer<boost::char_separator<char> > 
    tokenizer;


Tester::Tester(const char *inFileName, const char *outFileName, const char* clFileName, const char* configFileName, Tools::Logger& logger)
:in(inFileName), pOut(new std::ofstream(outFileName)), logger(logger), timeSec(1), m_configFileName(configFileName), m_clFileName(clFileName)
{
	m_configFileNameTmp = m_configFileName;
	m_configFileNameTmp.append(".tmp");

	m_clFileNameTmp = m_clFileName;
	m_clFileNameTmp.append(".tmp");
}

void Tester::run()
{
    using namespace boost::filesystem;
	path config_path( m_configFileName );
	path config_tmp_path( m_configFileNameTmp );
    remove( config_tmp_path );
    bool wasFile = exists( config_path );
    if ( wasFile )
        copy_file( config_path, config_tmp_path);

	path gen_path( m_clFileName );
	path gen_tmp_path( m_clFileNameTmp );
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
	GetTimeManager().clean();
	CTimeRunnerPtr laboratory = CleverAnt3LaboratoryFactory::createLaboratory(logger, args);

    laboratory->writeInfo( std::cout );
    laboratory->writeInfo( *pOut );

    try
    {
        laboratory->runForTime( timeSec * 1000 );

        laboratory->writeResult( std::cout );
		std::cout << std::endl;
        laboratory->writeResult( *pOut );
		*pOut << std::endl;

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
	std::map<std::string, double> speedSum;
	
    std::cout << "Started mean test on " << args[0] << std::endl;
    *pOut << "mean test on \"" << args[0] << "\"" << std::endl;
	
	std::vector<std::string> newArgs(args.size() - 1);
	newArgs[0] = args[0];
	for (size_t i = 2; i < args.size(); ++i)
		newArgs[i - 1] = args[i];

    for ( int i=0; i<cnt; ++i )
    {
		CTimeRunnerPtr laboratory = CleverAnt3LaboratoryFactory::createLaboratory(logger, newArgs);
		GetTimeManager().clean();

        laboratory->runForTime( timeSec * 1000 );
        values[i] = laboratory->getRunCount();
        sum += values[i];
        std::cout << i+1 << " ";

		for (std::map<std::string, TimerData>::value_type val : GetTimeManager().getTimers())
		{
			boost::chrono::microseconds mcs = boost::chrono::duration_cast<boost::chrono::microseconds>(val.second.duration);
			double speed = (double)val.second.counter / mcs.count();
			speed *= 1000000;
			if (speedSum.find(val.first) == speedSum.end())
				speedSum[val.first] = speed;
			else
				speedSum[val.first] += speed;
		}
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

	for (std::map<std::string, double>::value_type val : speedSum)
	{
		double speed = val.second / cnt;
		int a = speed;
		int b = speed * 1000 - a * 1000;
		std::cout << val.first << " avg. speed=" << a << "." << b << std::endl;
		*pOut << val.first << " avg. speed=" << a << "." << b << std::endl;
	}
	std::cout << std::endl;
	*pOut << std::endl;
    return true;
}

bool Tester::setCmd( std::vector<std::string> &args )
{
    using namespace boost::filesystem;
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
		path oldPath(m_clFileName);
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
