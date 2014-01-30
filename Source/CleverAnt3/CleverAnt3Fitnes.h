#include "GeneticParams.h"
#include "GeneticAPI/CFitnesCounter.h"
//#include "GeneticCommon/CleverAntMap.h"
#include "GeneticCommon/AutomatShortTables.h"
typedef CAutomatShortTables<COUNTERS_TYPE, INPUT_TYPE, 4, 8> AUTOMAT;

class CCleverAnt3Fitnes : public CFitnesCounter<COUNTERS_TYPE, INPUT_TYPE, ANT_FITNESS_TYPE>
{
public:
	virtual ANT_FITNESS_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const = 0;
	virtual void fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNESS_TYPE>& result) const = 0;

	/*virtual void setMaps(std::vector<CMapPtr> maps);
	virtual const CMapPtr getMap(size_t i);
	virtual size_t getMapsCount();*/
	virtual size_t steps() const;
protected:
	//std::vector<CMapPtr> maps;
	size_t m_steps;
};

typedef boost::shared_ptr<CCleverAnt3Fitnes> CCleverAnt3FitnesPtr;