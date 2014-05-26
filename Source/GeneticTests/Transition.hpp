#include "testCommon.h"
#include <map>
#include <set>
#include <sstream>
#include "GeneticAPI/CRandom.h"
#include <boost/algorithm/string/trim.hpp>

template<typename T>
T getRandomFromSet(const std::set<T>& set, CRandom* rand)
{
	std::set<T>::const_iterator iter;
	std::advance(iter, rand->nextUINT(set.size()));
	return *iter;
}

class Transition 
{
private:
	INPUT_TYPE input;
	int outputSize;
	std::vector<OUTPUT_TYPE> output;

	int nextState;

	std::map<INPUT_TYPE, int> map;

	bool used;
	bool usedByNegativeTest;
	bool usedByVerifier;
	bool verified;

public:
	Transition(INPUT_TYPE input, int outputSize, int newState) 
		:input(input), outputSize(outputSize), nextState(newState)
	{}

	bool accepts(INPUT_TYPE s) {
		return input == s || input == input_any;
	}

	INPUT_TYPE getInput() {
		return input;
	}

	const std::vector<OUTPUT_TYPE>& getOutput() {
		return output;
	}

	int getNextState() {
		return nextState;
	}

	Transition* mutate(const std::set<INPUT_TYPE>& setOfInputs, const std::set<OUTPUT_TYPE>& setOfOutputs, int stateNumber, CRandom* rand) {
		int type = rand->nextINT(3);
		if (type == 0) {
			// Меняем конечное состояние
			return new Transition(input, outputSize, rand->nextINT(stateNumber));
		}
		else if (type == 1) {
			// Меняем входное воздействие
			return new Transition(getRandomFromSet(setOfInputs, rand), outputSize, nextState);
		}
		else {
			// Change output size
			return new Transition(input, mutateOutputSize(outputSize, setOfOutputs.size(), rand), nextState);
		}
	}

	Transition* copy(const std::set<INPUT_TYPE>& setOfInputs, const std::set<OUTPUT_TYPE>& setOfOutputs, int stateNumber) {
		/*if (isUsedByVerifier() && !used && (RANDOM.nextDouble() < Const.VERIFIER_MUTATION_PROBABILITY)) {
		//if transition is in verifiers stack, change all
		return new Transition(setOfInputs[RANDOM.nextInt(setOfInputs.length)],
		mutateOutputSize(outputSize, setOfOutputs.length),
		RANDOM.nextInt(stateNumber));
		}*/
		return new Transition(input, outputSize, nextState);
	}

	std::string toString() 
	{
		
		std::stringstream ss;
		if (output.size() == 0) {
			ss << " null";
		}
		else {
			for (OUTPUT_TYPE& s1 : output) {
				ss << " " << s1;
			}
		}
		std::string s = ss.str();
		boost::algorithm::trim(s);
		ss.clear();
		ss << input << " / " << s << ", " << nextState;
		return ss.str();
	}

protected:
	int mutateOutputSize(int outputSize, int outputCnt, CRandom* rand) {
		if (rand->nextUINT() & 256) {
			return std::min(outputSize + 1, outputCnt);
		}
		else {
			return std::max(0, outputSize - 1);
		}
	}

	/*String[] mutateArray(String[] array, String[] setOfOutputs) {
		ArrayList<String> list = new ArrayList<String>();
		for (String s : array) {
			list.add(s);
		}
		if (RANDOM.nextBoolean() && list.size() > 0) {
			// Удаление
			list.remove(RANDOM.nextInt(list.size()));
		}
		else {
			// Добавление
			list.add(RANDOM.nextInt(list.size() + 1), setOfOutputs[RANDOM.nextInt(setOfOutputs.length)]);
		}
		return list.toArray(new String[list.size()]);
	}*/
public:

	int getOutputSize() 
	{
		return outputSize;
	}

	void setOutputSize(int size) 
	{
		outputSize = size;
	}

	void addOutputSequence(INPUT_TYPE s) 
	{
		int i;
		if (map.find(s) == map.end())
			i = 0;
		else
			i = map.find(s)->second;
		map[s] = i + 1;
	}

	void labelByMostFrequent() 
	{
		int max = -1;
		INPUT_TYPE best = input_null;
		for (auto s : map) {
			int cur = s.second;
			if (cur > max) {
				max = cur;
				best = s.first;
			}
		}
		output.clear();
		if (best != input_null) 
		{
			output.push_back(best);
		}
	}

	void beginLabeling() 
	{
		BOOST_ASSERT(map.size() == 0);
		map.clear();
	}

	void markUnused() 
	{
		used = false;
	}

	void markUsed() 
	{
		used = true;
	}

	bool isUsedByNegativeTest() 
	{
		return usedByNegativeTest;
	}

	void setUsedByNegativeTest(bool used) 
	{
		usedByNegativeTest = used;
	}

	/**
	* is this transition in the counterexample
	* @return true if transition in the counterexample
	*/
	bool isUsedByVerifier() 
	{
		return usedByVerifier;
	}

	void setUsedByVerifier(bool used) 
	{
		usedByVerifier = used;
	}

	/**
	* Is this transition satisfy the LTL statement
	* @return true, if trastition satisfy
	*/
	bool isVerified() 
	{
		return verified;
	}

	void setVerified(bool ver) 
	{
		verified = ver;
	}
};