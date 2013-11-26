#include "stdafx.h"
#include "CActionContainerImpl.h"

bool CActionContainerImpl::addAction(COUNTERS_TYPE code, const std::string& name)
{
    if ( charToStr.find( code ) == charToStr.end() )
    {
        actions.push_back(code);
        strToChar[name] = code;
        charToStr[code] = name;
        return true;
    }
    return false;
}

const std::vector<COUNTERS_TYPE>& CActionContainerImpl::getActions() const
{
    return actions;
}

size_t CActionContainerImpl::size() const
{
    return actions.size();
}

COUNTERS_TYPE CActionContainerImpl::randomAction(CRandom* rand)
{
    int i = rand->nextUINT() % size();
    return actions[i];
}