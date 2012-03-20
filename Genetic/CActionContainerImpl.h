#pragma once
#include <map>
#include "../GeneticAPI/CAction.h"

class CActionContainerImpl : public CActionContainer
{
public:
    CActionContainerImpl() {};
    bool addAction( char code, const std::string& name );
    const std::vector<char>& getActions() const;
    char randomAction( CRandom* rand );
    size_t size() const;
private:
    std::vector<char> actions;
    std::map< std::string, char > strToChar;
    std::map< char, std::string > charToStr;
};