#include "stdafx.h"
#include "CAntFitnes.h"
#include "CMapImpl.h"

void CAntFitnes::setMaps(std::vector<CMapPtr> maps)
{
    this->maps = maps;
}

const CMapPtr CAntFitnes::getMap( size_t i )
{
    CMapImpl* map = new CMapImpl( maps.at(i).get() );
    return CMapPtr( map );
}

size_t CAntFitnes::getMapsCount()
{
    return maps.size();
}