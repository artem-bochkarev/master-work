#include "../Genetic/CTest.h"
#include "../Genetic/CLaboratoryMulti.h"
#include "../Genetic/CActionContainerImpl.h"
#include "../Genetic/CStateContainerImpl.h"
#include "../Genetic/CLaboratoryFactory.h"
#include "../Genetic/CMapFactory.h"
#include <boost/filesystem.hpp>
#include <iostream>
#include "Tools/Logger.h"

int main()
{
    using namespace boost::filesystem3;
	Tools::Logger logger;

    path config_path("config.txt");
    path config_tmp_path("config_tmp.txt");
    remove( config_tmp_path );
	bool wasFile = exists( config_path );
	if ( wasFile )
		copy_file( config_path, config_tmp_path);

	if ( !exists( "tests/testIt.txt" ) )
	{
		logger << "[FAILED] No testIt file in tests/\n";
		std::cout << "[FAILED] No testIt file in tests/\n";
		return 1;
	}
    std::ifstream in("tests/testIt.txt");
    std::ofstream out("speed_results.txt");
	logger << "[INFO] Starting tests/\n";
	std::cout << "[INFO] Starting tests/\n";
    try 
    {
        while ( in.good() )
        {
            std::string str;
            in >> str;
            std::string name = "tests/" + str;

            path new_file( name );
            if ( !exists( new_file ) )
			{
				logger << "[WARNING] " << name.c_str() << " does not exists\n";
				std::cout << "[WARNING] " << name.c_str() << " does not exists\n";
                continue;
			}

            remove( config_path );
            copy_file( new_file, config_path);

            CLaboratoryPtr laboratory;
            std::vector<CMapPtr> maps;
            laboratory = CLaboratoryFactory::getLaboratory("config.txt", logger);
            CMapPtr map1 = CMapFactory::getMap( "map1.txt" );

            maps.push_back( map1 );

            laboratory->setMaps( maps );
			CGeneticStrategyPtr strategy = laboratory->getStrategy();
			std::cout << strategy->getDeviceType() <<
                ", states count: " << laboratory->getStates()->size() << ",  size: " << 
                strategy->getN() << "x" << strategy->getM();

            laboratory->runForTime( 30 * 1000 );

            std::cout << ",  result: " << 
                laboratory->getGenerationNumber() << std::endl;
            out << strategy->getDeviceType() <<
                ", states count: " << laboratory->getStates()->size() << ",  size: " << 
                strategy->getN() << "x" << strategy->getM() << ",  result: " << 
                laboratory->getGenerationNumber() << std::endl;
			logger << "[SUCCESS] " << name << "\n";
        }
    }
    catch ( std::runtime_error& error )
    {
        //do nothing
        std::cout << "bad testIt.txt file" << std::endl;
		logger << "[FAILED]" << error.what() << "\n";
    }

    remove( config_path );
	if ( wasFile )
		copy_file( config_tmp_path, config_path);
    remove( config_tmp_path );

    std::cout << "end";
    char c;
    std::cin >> c;
}