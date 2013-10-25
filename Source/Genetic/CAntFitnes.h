#pragma once
#include "GeneticAPI/CFitnessCounter.h"
#include "CMap.h"

typedef double ANT_FITNES_TYPE;

class CAntFitnes : public CFitnesCounter<ANT_FITNES_TYPE>
{
public:
    virtual ANT_FITNES_TYPE fitnes( const CAutomat* automat ) const = 0;

    virtual void setMaps( std::vector<CMapPtr> maps );
    virtual const CMapPtr getMap( size_t i );
    virtual size_t getMapsCount();
protected:
    std::vector<CMapPtr> maps;
};

typedef boost::shared_ptr<CAntFitnes> CAntFitnesPtr;

class CAntFitnesFullTablesCPU : public CAntFitnes
{
public:
    virtual ANT_FITNES_TYPE fitnes( const CAutomat* automat ) const;
};
typedef boost::shared_ptr<CAntFitnesFullTablesCPU> CAntFitnesFullTablesCPUPtr;


class CAntFitnesFullTablesNone : public CAntFitnes
{
public:
    virtual ANT_FITNES_TYPE fitnes( const CAutomat* automat ) const;
};
typedef boost::shared_ptr<CAntFitnesFullTablesNone> CAntFitnesFullTablesNonePtr;