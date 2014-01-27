#include "ActionContainerImpl.h"

template<typename COUNTERS_TYPE>
bool CActionContainerImpl<COUNTERS_TYPE>::addAction(COUNTERS_TYPE code, const std::string& name)
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

template<typename COUNTERS_TYPE>
const std::vector<COUNTERS_TYPE>& CActionContainerImpl<COUNTERS_TYPE>::getActions() const
{
    return actions;
}

template<typename COUNTERS_TYPE>
size_t CActionContainerImpl<COUNTERS_TYPE>::size() const
{
    return actions.size();
}

template<typename COUNTERS_TYPE>
COUNTERS_TYPE CActionContainerImpl<COUNTERS_TYPE>::randomAction(CRandom* rand)
{
    int i = rand->nextUINT() % size();
    return actions[i];
}