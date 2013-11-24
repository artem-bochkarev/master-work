#include "stdafx.h"
#include "CStateContainerImpl.h"

bool CStateContainerImpl::addState(COUNTERS_TYPE code, const std::string& name)
{
    if ( charToStr.find( code ) == charToStr.end() )
    {
        states.push_back( code );
        if ( name.compare("") != 0 )
        {
            strToChar[name] = code;
            charToStr[code] = name;
        }
        return true;
    }
    return false;
}

const std::vector<COUNTERS_TYPE>& CStateContainerImpl::getStates() const
{
    return states;
}

COUNTERS_TYPE CStateContainerImpl::randomState(CRandom* rand)
{
    size_t i = rand->nextUINT() % size();
    //return states[i];
	return static_cast<COUNTERS_TYPE>(i);
}

size_t CStateContainerImpl::size() const
{
    return states.size();
}