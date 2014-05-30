#pragma once
#include <vector>

template <typename TEST_TYPE, typename FORMULA_TYPE>
class TestGroup {
private:
	std::vector<TEST_TYPE> tests;
	std::vector<TEST_TYPE> negativeTests;
	std::vector<FORMULA_TYPE> formulas;

public:
	TestGroup(const std::vector<TEST_TYPE>& tests,
		const std::vector<TEST_TYPE>& negativeTests,
		const std::vector<FORMULA_TYPE>& formulas)
		:tests(tests), negativeTests(negativeTests), formulas(formulas)
	{}

	const std::vector<TEST_TYPE>& getTests() {
		return tests;
	}

	const std::vector<TEST_TYPE>& getNegativeTests() {
		return negativeTests;
	}

	const std::vector<FORMULA_TYPE>& getFormulas() {
		return formulas;
	}
};