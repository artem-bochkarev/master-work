#pragma once
#include <vector>
#include <string>
#include "boost/smart_ptr/shared_ptr.hpp"
#include "CRandom.h"
//#include "vld.h"

template < typename NUMBERS_TYPE > class CActionContainer
{
public:
    virtual ~CActionContainer() {};
	virtual bool addAction(NUMBERS_TYPE code, const std::string& name) = 0;
	virtual const std::vector<NUMBERS_TYPE>& getActions() const = 0;
	virtual NUMBERS_TYPE randomAction(CRandom* rand) = 0;
    virtual size_t size() const = 0;
};

template<typename NUMBERS_TYPE, typename C = CActionContainer<NUMBERS_TYPE> >
using CActionContainerPtr = boost::shared_ptr< C >;
//typedef boost::shared_ptr<CActionContainer> CActionContainerPtr;