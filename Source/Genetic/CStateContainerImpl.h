#pragma once
#include "GeneticParams.h"
#include <map>
#include "GeneticAPI/CState.h"

class CStateContainerImpl : public CStateContainer<COUNTERS_TYPE>
{
public:
    CStateContainerImpl() {};
    bool addState( char code, const std::string& name = "" );
    const std::vector<char>& getStates() const;
    char randomState( CRandom* rand = 0 );
    size_t size() const;
private:
    std::vector<char> states;
    std::map< std::string, char > strToChar;
    std::map< char, std::string > charToStr;
};