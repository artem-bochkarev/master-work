#pragma once

#include "GeneticCommon/CleverAntMapImpl.h"

class CCleverAnt3Map : public CMapImpl
{
public:
	CCleverAnt3Map(size_t width, size_t geight, size_t foodCounter);
	virtual std::vector<int> getInput(size_t x, size_t y, EDirection direct) const override;
	virtual void getInput(size_t x, size_t y, EDirection direct, int* output) const override;
	virtual std::vector< std::pair<int, int> > getVisibleCells(int x, int y, EDirection direct) const override;
	virtual int getVisibleCells(int x, int y, EDirection direct, int* buffer) const override;
};