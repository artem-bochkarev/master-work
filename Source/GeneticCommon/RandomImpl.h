#pragma once
#include "GeneticAPI/CRandom.h"

class CRandomImpl : public CRandom
{
    uint value;
public:
    CRandomImpl();
    CRandomImpl( uint value );
	virtual uint nextUINT();
	virtual uint nextUINT(uint mod);
	virtual int nextINT();
	virtual int nextINT(int mod);
    virtual void setValue( uint value );
    virtual ~CRandomImpl();
};