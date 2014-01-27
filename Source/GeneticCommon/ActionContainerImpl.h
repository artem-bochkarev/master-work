#pragma once
#include <map>
#include "GeneticAPI/CAction.h"

template<typename COUNTERS_TYPE>
class CActionContainerImpl : public CActionContainer<COUNTERS_TYPE>
{
public:
    CActionContainerImpl() {};
	bool addAction(COUNTERS_TYPE code, const std::string& name);
	const std::vector<COUNTERS_TYPE>& getActions() const;
	COUNTERS_TYPE randomAction(CRandom* rand);
    size_t size() const;
private:
	std::vector<COUNTERS_TYPE> actions;
	std::map< std::string, COUNTERS_TYPE > strToChar;
	std::map< COUNTERS_TYPE, std::string > charToStr;
};