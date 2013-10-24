#pragma once
#include "CAutomat.h"

template< typename FITNESS_TYPE > class CFitnesCounter
{
public:
    virtual FITNESS_TYPE fitnes( const CAutomat* automat ) const = 0;
};

//typedef boost::shared_ptr<CFitnesCounter> CFitnesCounterPtr;