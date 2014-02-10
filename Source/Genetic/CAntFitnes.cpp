#include "stdafx.h"
#include "CAntFitnes.h"
#include "GeneticCommon/CleverAntMapImpl.h"
#include "GeneticCommon/Test.hpp"

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

size_t CAntFitnes::steps() const
{
	return m_steps;
}

CAntFitnes::CAntFitnes(size_t steps)
:m_steps(steps) {}

CAntFitnesCPU::CAntFitnesCPU()
:CAntFitnes(DEFAULT_STEPS_COUNT) {}

CAntFitnesCPU::CAntFitnesCPU(size_t steps)
:CAntFitnes(steps) {}

CAntFitnesNone::CAntFitnesNone()
: CAntFitnes(DEFAULT_STEPS_COUNT) {}

CAntFitnesNone::CAntFitnesNone(size_t steps)
: CAntFitnes(steps) {}

 
ANT_FITNES_TYPE CAntFitnesCPU::fitnes( const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat ) const
{
    ANT_FITNES_TYPE result = 0.0;
    for ( std::vector<CMapPtr>::const_iterator it = maps.begin(); it != maps.end(); ++it )
    {
        CMapImpl tmpMap( it->get() );
		result += CTest<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>::run(automat, &tmpMap, m_steps);
    }
    return result;
}

ANT_FITNES_TYPE CAntFitnesNone::fitnes( const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat ) const
{
    BOOST_ASSERT_MSG( 0, "Not allowed. Implement it in your code or use CPU version" );
    return 0.0;
}