#include "../Genetic/CTest.h"
#include "../Genetic/CLaboratoryMulti.h"
#include "../Genetic/CActionContainerImpl.h"
#include "../Genetic/CStateContainerImpl.h"
#include "../Genetic/CLaboratoryFactory.h"
#include "../Genetic/CMapFactory.h"
#include <boost/filesystem.hpp>
#include <iostream>

int main()
{
    using namespace boost::filesystem3;

    path config_path("config.txt");
    path config_tmp_path("config_tmp.txt");
    remove( config_tmp_path );
    copy_file( config_path, config_tmp_path);

    std::ifstream in("tests/testIt.txt");
    std::ofstream out("speed_results.txt");
    try 
    {
        while ( in.good() )
        {
            std::string str;
            in >> str;
            std::string name = "tests/" + str;

            path new_file( name );
            if ( !exists( new_file ) )
                continue;

            remove( config_path );
            copy_file( new_file, config_path);

            CLaboratoryPtr laboratory;
            std::vector<CMapPtr> maps;
            laboratory = CLaboratoryFactory::getLaboratory("config.txt");
            CMapPtr map1 = CMapFactory::getMap( "map1.txt" );

            maps.push_back( map1 );

            laboratory->setMaps( maps );
            laboratory->runForTime( 30 * 1000 );
            CGeneticStrategyPtr strategy = laboratory->getStrategy();
            std::cout << strategy->getDeviceType() <<
                ", states count: " << laboratory->getStates()->size() << ",  size: " << 
                strategy->getN() << "x" << strategy->getM() << ",  result: " << 
                laboratory->getGenerationNumber() << std::endl;
            out << strategy->getDeviceType() <<
                ", states count: " << laboratory->getStates()->size() << ",  size: " << 
                strategy->getN() << "x" << strategy->getM() << ",  result: " << 
                laboratory->getGenerationNumber() << std::endl;
        }
    }
    catch ( std::runtime_error& error )
    {
        //do nothing
        std::cout << "bad testIt.txt file" << std::endl;
    }

    remove( config_path );
    copy_file( config_tmp_path, config_path);
    remove( config_tmp_path );

    std::cout << "end";
    char c;
    std::cin >> c;
}