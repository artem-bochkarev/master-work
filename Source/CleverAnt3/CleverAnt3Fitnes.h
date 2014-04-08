#pragma once
#include "GeneticParams.h"
#include "GeneticCommon/CleverAntFitnes.h"
//#include "GeneticCommon/CleverAntMap.h"
#include "GeneticCommon/AutomatShortTables.h"
#include "GeneticCommon/AutomatShortTablesStatic.hpp"
typedef CAutomatShortTablesStatic<COUNTERS_TYPE, INPUT_TYPE, 4, 8, 5> AUTOMAT;

class CCleverAnt3Fitnes : public CCleverAntFitnes<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNES_TYPE>
{
public:
	virtual ANT_FITNES_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const = 0;
	virtual void fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result) = 0;

	virtual void setMaps(std::vector<CMapPtr> maps);
	virtual const CMapPtr getMap(size_t i);
	virtual size_t getMapsCount();
	virtual size_t steps() const;
	virtual std::string getInfo() const = 0;
protected:
	std::vector<CMapPtr> maps;
	size_t m_steps;
};

typedef boost::shared_ptr<CCleverAnt3Fitnes> CCleverAnt3FitnesPtr;

class CCleverAnt3FitnesCPU : public CCleverAnt3Fitnes
{
public:
	CCleverAnt3FitnesCPU(const std::vector< std::string >& strings);
	virtual ANT_FITNES_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const;
	virtual void fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result) override;
	virtual std::string getInfo() const;
protected:
};