#pragma once
#include <vector>
#include <string>
#include "boost/smart_ptr/shared_ptr.hpp"
#include "CRandom.h"

template < typename NUMBERS_TYPE > class CStateContainer
{
public:
    virtual ~CStateContainer() {};
	virtual bool addState(NUMBERS_TYPE code, const std::string& name = "") = 0;
	virtual const std::vector<NUMBERS_TYPE>& getStates() const = 0;
	virtual NUMBERS_TYPE randomState(CRandom* rand) = 0;
    virtual size_t size() const = 0;
};