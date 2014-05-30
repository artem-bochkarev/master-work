#pragma once
#include "TestGroup.hpp"
#include "testCommon.h"
#include "TestAutomat.h"
#include <vector>

class TestFitnes
{
	const double TESTS_COST = 1;
	const double FORMULAS_COST = TESTS_COST;
public:
	FITNES_TYPE calcFitnessForTest(TestAutomat fst, TEST test);

	FITNES_TYPE calcFitness(TestAutomat fst);
	void calcFitness(std::vector<TestAutomat>& fst, std::vector<FITNES_TYPE>& results);

protected:
	bool same(std::vector<OUTPUT_TYPE> output, std::vector<OUTPUT_TYPE> answer);

	FITNES_TYPE editDistance(std::vector<OUTPUT_TYPE> a, std::vector<OUTPUT_TYPE> b);

	std::vector<TestGroup<TEST_TYPE, FORMULA_TYPE>> groups;
};