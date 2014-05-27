#include "TestGroup.hpp"
#include "testCommon.h"
#include <vector>

class TestFitnes
{
public:
	FITNES_TYPE calcFitnessForTest(TEST_AUTOMAT fst, TEST test);

	FITNES_TYPE calcFitness(TEST_AUTOMAT fst);
	void calcFitness(std::vector<TEST_AUTOMAT>& fst, std::vector<FITNES_TYPE>& results);

protected:
	bool same(std::vector<OUTPUT_TYPE> output, std::vector<OUTPUT_TYPE> answer);

	FITNES_TYPE editDistance(std::vector<OUTPUT_TYPE> a, std::vector<OUTPUT_TYPE> b);

	std::vector<TestGroup<TEST_TYPE, FORMULA_TYPE>> groups;
};