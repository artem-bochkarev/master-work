#pragma once
#include "LabResultMulti.h"

template<typename CT, typename IT>
void CLabResultMulti<CT, IT>::addGeneration(const CAutomatPtr<CT, IT> ind, double maxF, double avgF)
{
    individs.push_back( ind );
    avgFitnes.push_back( avgF );
    maxFitnes.push_back( maxF );
    ++genCnt;
}

template<typename CT, typename IT>
double CLabResultMulti<CT, IT>::getAvgFitnes(size_t i) const
{
    return avgFitnes[i];
}

template<typename CT, typename IT>
CAutomat<CT, IT>* CLabResultMulti<CT, IT>::getBestInd(size_t i) const
{
    return individs[i].get();
}

template<typename CT, typename IT>
size_t CLabResultMulti<CT, IT>::getGenerationsNumber() const
{
    return genCnt;
}

template<typename CT, typename IT>
size_t CLabResultMulti<CT, IT>::getRunCount() const
{
    return genCnt;
}

template<typename CT, typename IT>
CAutomat<CT, IT>* CLabResultMulti<CT, IT>::getLastInd() const
{
    return individs[genCnt - 1].get();
}

template<typename CT, typename IT>
double CLabResultMulti<CT, IT>::getMaxFitnes(size_t i) const
{
    return maxFitnes[i];
}

/*boost::mutex& CLabResultMulti::getMutex()
{
    return mutex;
}*/

template<typename CT, typename IT>
CLabResultMulti<CT, IT>::CLabResultMulti()
:genCnt(0) {}