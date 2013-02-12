#pragma once
#include <vector>
#include <string>
#include "boost/smart_ptr/shared_ptr.hpp"
#include "CRandom.h"

class CStateContainer
{
public:
    CStateContainer() {};
    virtual bool addState( char code, const std::string& name = "" ) = 0;
    virtual const std::vector<char>& getStates() const = 0;
    virtual char randomState( CRandom* rand ) = 0;
    virtual size_t size() const = 0;
};

typedef boost::shared_ptr<CStateContainer> CStateContainerPtr;