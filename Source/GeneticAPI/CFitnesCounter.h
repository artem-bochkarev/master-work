#pragma once
#include "CAutomat.h"

template< typename NUMBERS_TYPE, typename INPUT_TYPE, typename FITNESS_TYPE > class CFitnesCounter
{
public:
    virtual FITNESS_TYPE fitnes( const CAutomat<NUMBERS_TYPE, INPUT_TYPE>* automat ) const = 0;
};

template< typename NT, typename IT, typename FT, typename C = CFitnesCounter<NT, IT, FT> >
using CFitnesCounterPtr = boost::shared_ptr< C >;
