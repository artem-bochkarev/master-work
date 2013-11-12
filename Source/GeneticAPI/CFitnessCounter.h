#pragma once
#include "CAutomat.h"

template< typename FITNESS_TYPE, typename NUMBERS_TYPE, typename INPUT_TYPE > class CFitnesCounter
{
public:
    virtual FITNESS_TYPE fitnes( const CAutomat<NUMBERS_TYPE, INPUT_TYPE>* automat ) const = 0;
};

//typedef boost::shared_ptr<CFitnesCounter> CFitnesCounterPtr;