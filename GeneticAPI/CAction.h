#pragma once
#include <vector>
#include <string>
#include "boost/smart_ptr/shared_ptr.hpp"
#include "CRandom.h"
//#include "vld.h"

class CActionContainer
{
public:
    CActionContainer() {};
    virtual bool addAction( char code, const std::string& name ) = 0;
    virtual const std::vector<char>& getActions() const = 0;
    virtual char randomAction( CRandom* rand ) = 0;
    virtual size_t size() const = 0;
};

typedef boost::shared_ptr<CActionContainer> CActionContainerPtr;