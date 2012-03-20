#include "stdafx.h"
#include "CStateContainerImpl.h"

bool CStateContainerImpl::addState( char code, const std::string& name )
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

const std::vector<char>& CStateContainerImpl::getStates() const
{
    return states;
}

char CStateContainerImpl::randomState( CRandom* rand )
{
    int i = rand->nextUINT() % size();
    return states[i];
}

size_t CStateContainerImpl::size() const
{
    return states.size();
}