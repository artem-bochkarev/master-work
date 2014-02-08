#pragma once
#include <map>
#include "GeneticAPI/CAction.h"

template<typename COUNTERS_TYPE>
class CActionContainerImpl : public CActionContainer<COUNTERS_TYPE>
{
public:
    CActionContainerImpl() {};
	virtual bool addAction(COUNTERS_TYPE code, const std::string& name) override;
	virtual const std::vector<COUNTERS_TYPE>& getActions() const override;
	virtual COUNTERS_TYPE randomAction(CRandom* rand) override;
	virtual size_t size() const override;
private:
	std::vector<COUNTERS_TYPE> actions;
	std::map< std::string, COUNTERS_TYPE > strToChar;
	std::map< COUNTERS_TYPE, std::string > charToStr;
};