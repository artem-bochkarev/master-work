#include "TestFitnes.h"
#include <algorithm>

FITNES_TYPE TestFitnes::calcFitnessForTest(TEST_AUTOMAT fst, TEST test) {
	std::vector<OUTPUT_TYPE> output = fst.transform(test.getInput());
	std::vector<OUTPUT_TYPE> answer = test.getOutput();
	FITNES_TYPE t;
	if (output.size() == 0) {
		t = 1;
	}
	else {
		t = editDistance(output, answer) / std::max(output.size(), answer.size());
	}
	return 1 - t;
}

void TestFitnes::calcFitness(std::vector<TEST_AUTOMAT>& fst, std::vector<FITNES_TYPE>& results) {
}

FITNES_TYPE TestFitnes::calcFitness(TEST_AUTOMAT fst) {
	int transitionCount = fst.getUsedTransitionsCount();
	if (transitionCount == 0) {
		return 0.0;
	}

	fst.unmarkAllTransitions();
	fst.doLabelling(this.tests);

	FITNES_TYPE res = 0; //fitness
	int i = 0;      //group number
	for (TestGroup group : groups) {
		FITNES_TYPE pSum = 0;   //positive tests sum
		int nSum = 0;      //negative tests sum
		int cntOk = 0;

		for (TEST p : group.getTests()) {
			std::vector<OUTPUT_TYPE> output = fst.transform(p.getInput());
			std::vector<OUTPUT_TYPE> answer = p.getOutput();
			FITNES_TYPE t;
			if (output.size() == 0) 
			{
				t = 1;
			}
			else 
			{
				t = editDistance(output, answer) / std::max(output.size(), answer.size());
			}
			pSum += 1 - t;
			if (same(output, answer)) {
				cntOk++;
			}
		}

		for (Path p : group.getNegativeTests()) {
			if (!fst.validateNegativeTest(p.getInput())) {
				nSum++;
			}
		}

		int testsSize = group.getTests().size();
		int negativeTestsSize = group.getNegativeTests().size();
		int formulasSize = group.getFormulas().size();

		/*if (testsSize > 0) {
		res += (cntOk == testsSize) ? TESTS_COST : 0.5 * TESTS_COST * (sum / testsSize);
		}
		if (formulasSize > 0) {
		FITNES_TYPE tmp = FORMULAS_COST * verRes[i] / formulasSize / transitionCount;
		if ((tmp > FORMULAS_COST) || (tmp < 0)) {
		throw new RuntimeException(String.valueOf(tmp));
		}
		res += tmp;
		}*/
		FITNES_TYPE testsFF = TESTS_COST;
		if (testsSize > 0) {
			testsFF = (cntOk == testsSize) ? TESTS_COST : TESTS_COST * (pSum / testsSize);
		}
		FITNES_TYPE negativeTestsFF = (negativeTestsSize != 0) ? (nSum * 1.0) / negativeTestsSize : 0;
		FITNES_TYPE ltlFF = FORMULAS_COST;
		if (formulasSize > 0) {
			ltlFF = FORMULAS_COST * verRes[i] / formulasSize / transitionCount;
			if ((ltlFF > FORMULAS_COST) || (ltlFF < 0)) {
				throw new RuntimeException(String.valueOf(ltlFF));
			}
		}
		res += testsFF + testsFF * ltlFF - negativeTestsFF * testsFF;
		i++;
	}
	return res + 0.0001 * (100 - fst.getTransitionsCount());
}

bool TestFitnes::same(std::vector<OUTPUT_TYPE> output, std::vector<OUTPUT_TYPE> answer) {
	if (output.size() == 0 || answer.size() == 0) 
	{
		return false;
	}
	if (output.size() != answer.size()) 
	{
		return false;
	}
	for (int i = 0; i < answer.size(); i++) {
		if (answer[i] != output[i]) 
		{
			return false;
		}
	}
	return true;
}

FITNES_TYPE TestFitnes::editDistance(std::vector<OUTPUT_TYPE> a, std::vector<OUTPUT_TYPE> b) {
	int n = a.size();
	int m = b.size();
	FITNES_TYPE **d = new FITNES_TYPE*[n + 1];

	for (int i = 0; i <= n; i++) {
		d[i] = new FITNES_TYPE[m + 1];
		d[i][0] = i;
	}
	for (int j = 0; j <= m; j++) {
		d[0][j] = j;
	}

	for (int i = 0; i < n; i++) {
		for (int j = 0; j < m; j++) {
			int cost;
			if (a[i] ==b[j]) {
				cost = 0;
			}
			else {
				cost = 1;
			}
			d[i + 1][j + 1] = std::min(std::min(
				d[i][j + 1] + 1,
				d[i + 1][j] + 1),
				d[i][j] + cost);
		}
	}

	FITNES_TYPE res = d[n][m];
	for (int i = 0; i <= n; i++)
	{
		delete[] d[i];
	}
	delete[] d;
	return res;
}