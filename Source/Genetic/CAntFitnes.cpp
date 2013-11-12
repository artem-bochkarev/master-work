#include "stdafx.h"
#include "CAntFitnes.h"
#include "CMapImpl.h"
#include "CTest.h"

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

 
ANT_FITNESS_TYPE CAntFitnesFullTablesCPU::fitnes( const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat ) const
{
    ANT_FITNESS_TYPE result = 0.0;
    for ( std::vector<CMapPtr>::const_iterator it = maps.begin(); it != maps.end(); ++it )
    {
        CMapImpl tmpMap( it->get() );
        result +=  CTest::run( automat,  &tmpMap );
    }
    return result;
}

ANT_FITNESS_TYPE CAntFitnesFullTablesNone::fitnes( const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat ) const
{
    BOOST_ASSERT_MSG( 0, "Not allowed. Implement it in your code or use CPU version" );
    return 0.0;
}