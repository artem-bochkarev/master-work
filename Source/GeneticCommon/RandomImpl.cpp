#include "RandomImpl.h"
#include <ctime>

// rand like in MSVC
const uint rand_m = 0x80000000;
const uint rand_a = 214013;
const uint rand_c = 2531011;

CRandomImpl::CRandomImpl()
:value( std::clock() ) {}

CRandomImpl::CRandomImpl(uint value)
:value(value) {}

CRandomImpl::~CRandomImpl()
{}

void CRandomImpl::setValue(uint value)
{
    this->value = value;
}

uint CRandomImpl::nextUINT()
{
    value = ( rand_a*value + rand_c ) % rand_m;
    return value;
    //return std::rand();
}