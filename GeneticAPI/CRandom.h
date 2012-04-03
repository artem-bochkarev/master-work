#pragma once
#include "boost/smart_ptr/shared_ptr.hpp"

typedef unsigned int uint;


class CRandom
{
public:
    CRandom() {};
    virtual uint nextUINT() = 0;
    virtual void setValue( uint value ) = 0;
    virtual ~CRandom() {};
};

typedef boost::shared_ptr< CRandom > CRandomPtr;