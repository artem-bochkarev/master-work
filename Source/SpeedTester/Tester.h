#pragma once
#include <fstream>
#include <Tools/Logger.h>
#include <vector>
#include <string>
#include <boost/filesystem.hpp>

class Tester
{
    enum Commands
    {
        RUN,
        SET,
        AR_MEAN,
        ERROR
    };
public:
	Tester(const char* inFileName, const char* outFileName, const char* clFileName, const char* configFileName, Tools::Logger& logger);
    void run();
private:
    Commands parseStr( const std::string& in, std::vector<std::string>& args );
    bool runCmd( std::vector<std::string>& args );
    bool setCmd( std::vector<std::string>& args );
    bool meanCmd( std::vector<std::string>& args );
    bool process( const std::string& in );
	void createConfigFile(const boost::filesystem::path& tmp_config);
    std::ifstream in;
    typedef boost::shared_ptr<std::ofstream> ofstreamPtr;
    ofstreamPtr pOut;
    std::string version;
    Tools::Logger& logger;
	std::string m_configFileName, m_clFileName;
	std::string m_configFileNameTmp, m_clFileNameTmp;

    int timeSec;
	std::string m_genSize, m_stepsCount, m_useOpencl, m_deviceType, m_automatType;
	bool m_bCreateConfig;
};