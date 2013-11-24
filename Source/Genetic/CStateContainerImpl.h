#pragma once
#include "GeneticParams.h"
#include <map>
#include "GeneticAPI/CState.h"

class CStateContainerImpl : public CStateContainer<COUNTERS_TYPE>
{
public:
    CStateContainerImpl() {};
	bool addState(COUNTERS_TYPE code, const std::string& name = "") override;
	const std::vector<COUNTERS_TYPE>& getStates() const override;
	COUNTERS_TYPE randomState(CRandom* rand = 0) override;
    size_t size() const override;
private:
	std::vector<COUNTERS_TYPE> states;
	std::map< std::string, COUNTERS_TYPE > strToChar;
	std::map< COUNTERS_TYPE, std::string > charToStr;
};