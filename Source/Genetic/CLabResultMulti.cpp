#include "stdafx.h"
#include "CLabResultMulti.h"

void CLabResultMulti::addGeneration( const CAutomatPtr ind, double maxF, double avgF )
{
    individs.push_back( ind );
    avgFitnes.push_back( avgF );
    maxFitnes.push_back( maxF );
    ++genCnt;
}

double CLabResultMulti::getAvgFitnes(size_t i) const
{
    return avgFitnes[i];
}

CAutomat<COUNTERS_TYPE, INPUT_TYPE>* CLabResultMulti::getBestInd(size_t i) const
{
    return individs[i].get();
}

size_t CLabResultMulti::getGenerationsNumber() const
{
    return genCnt;
}

size_t CLabResultMulti::getRunCount() const
{
    return genCnt;
}

CAutomat<COUNTERS_TYPE, INPUT_TYPE>* CLabResultMulti::getLastInd() const
{
    return individs[genCnt - 1].get();
}

double CLabResultMulti::getMaxFitnes(size_t i) const
{
    return maxFitnes[i];
}

/*boost::mutex& CLabResultMulti::getMutex()
{
    return mutex;
}*/

CLabResultMulti::CLabResultMulti()
:genCnt(0) {}