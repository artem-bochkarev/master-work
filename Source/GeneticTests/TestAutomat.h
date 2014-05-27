#include "GeneticAPI/CRandom.h"
#include <set>
#include "testCommon.h"
#include "Transition.hpp"

template<typename T>
T getNfromSet(const std::set<T>& set, size_t n)
{
	std::set<T>::const_iterator iter;
	std::advance(iter, n);
	return *iter;
}

class TestAutomat {

public:
	//static FitnessCalculator fitnessCalculator = null;

	//static int cntFitnessRun = 0;

private:
	CRandom* rand;

	int initialState;
	int stateNumber;
	//Transition **states;
	std::vector<std::vector<Transition>> states;

	std::set<INPUT_TYPE> setOfInputs;
	std::set<OUTPUT_TYPE> setOfOutputs;

	bool isLabelled;

public:
	TestAutomat(const std::vector<std::vector<Transition>>& states, int stateNumber, int initialState, const std::set<INPUT_TYPE>& setOfInputs, const std::set<OUTPUT_TYPE>& setOfOutputs)
		:initialState(initialState), stateNumber(stateNumber), states(states), setOfInputs(setOfInputs), setOfOutputs(setOfOutputs), isLabelled(false)
	{
	}

	std::vector<std::vector<Transition>>& getStates() {
		return states;
	}

	int getInitialState() {
		return initialState;
	}

	static TestAutomat randomIndividual(int stateNumber, const std::set<INPUT_TYPE>& setOfInputs, const std::set<OUTPUT_TYPE>& setOfOutputs, CRandom* rand) {
		int inputsCnt = setOfInputs.size;
		int outputsCnt = setOfOutputs.size;
		std::vector<std::vector<Transition>> states(stateNumber);

		for (int i = 0; i < stateNumber; i++) {
			std::vector<int> p(inputsCnt);
			for (int j = 0; j < inputsCnt; j++) {
				p[j] = j;
			}
			for (int j = inputsCnt - 1; j >= 1; j--) {
				int k = rand->nextUINT() % j;
				int t = p[j];
				p[j] = p[k];
				p[k] = t;
			}

			int degree = rand->nextUINT() % std::min(inputsCnt, max_transitions);
			states[i].resize(degree);

			for (int j = 0; j < degree; j++) {
				states[i][j] = Transition(getNfromSet(setOfInputs, p[j]), 1, rand->nextINT(stateNumber));
			}
		}
		return new TestAutomat(states, stateNumber, rand->nextINT(stateNumber), setOfInputs, setOfOutputs);
	}

	/*static TestAutomat[] crossOver(FST fst1, FST fst2, List<TestGroup> groups) {
		switch (RANDOM.nextInt(3)) {
		case 0:
			return fst1.crossOver(fst2);
		case 1:
			return fst1.crossOverBasedOnTests(fst2, groups);
		case 2:
			return fst1.crossOverBasedOnLtl(fst2);
		default:
			throw new RuntimeException("Unexpected crossover type.");
		}
	}*/

protected:

	/*void markUsedTransitions(std::vector<TEST_TYPE> tests) 
	{
		ArrayList<Pair> tlist = new ArrayList<Pair>();
		for (Path t : tests) {
			tlist.add(new Pair(t, fitnessCalculator.calcFitnessForTest(this, t) * t.size()));
		}

		Collections.sort(tlist);

		int testsCnt = Math.max(Const.MIN_USED_TESTS, tests.size() / 5);

		for (int i = 0; i < testsCnt; i++) {
			Path test = tlist.get(i).getTest();
			int currentState = initialState;
			for (String s : test.getInput()) {
				boolean found = false;
				for (Transition t : states[currentState]) {
					if (t.accepts(s)) {
						currentState = t.getNewState();
						found = true;
						t.markUsed();
						break;
					}
				}
				if (!found) {
					break;
				}
			}
		}
	}*/

	void markUnused() {
		for (std::vector<Transition>& state : states) {
			for (Transition t : state) {
				t.markUnused();
			}
		}
	}

	/*public FST[] crossOverBasedOnTests(FST that, List<TestGroup> groups) {
		this.markUnused();
		that.markUnused();
		for (TestGroup g : groups) {
			this.markUsedTransitions(g.getTests());
			that.markUsedTransitions(g.getTests());
		}
		int initialState1 = this.initialState;
		int initialState2 = that.initialState;

		Transition[][] states1 = new Transition[stateNumber][];
		Transition[][] states2 = new Transition[stateNumber][];

		for (int i = 0; i < stateNumber; i++) {

			ArrayList<Transition> toStates1 = new ArrayList<Transition>();
			ArrayList<Transition> toStates2 = new ArrayList<Transition>();

			{
				for (Transition t : this.states[i]) {
					if (t.used && !t.isUsedByNegativeTest()) {
						toStates1.add(t.copy(setOfInputs, setOfOutputs, stateNumber));
					}
				}
				for (Transition t : that.states[i]) {
					if (t.used && !t.isUsedByNegativeTest()) {
						toStates1.add(t.copy(setOfInputs, setOfOutputs, stateNumber));
					}
				}
				for (Transition t : this.states[i]) {
					if (!t.used || t.isUsedByNegativeTest()) {
						toStates1.add(t.copy(setOfInputs, setOfOutputs, stateNumber));
					}
				}
			}

			{
				for (Transition t : that.states[i]) {
					if (t.used && !t.isUsedByNegativeTest()) {
						toStates2.add(t.copy(setOfInputs, setOfOutputs, stateNumber));
					}
				}
				for (Transition t : this.states[i]) {
					if (t.used && !t.isUsedByNegativeTest()) {
						toStates2.add(t.copy(setOfInputs, setOfOutputs, stateNumber));
					}
				}
				for (Transition t : that.states[i]) {
					if (!t.used || t.isUsedByNegativeTest()) {
						toStates2.add(t.copy(setOfInputs, setOfOutputs, stateNumber));
					}
				}
			}

			states1[i] = removeDuplicates(toStates1.toArray(new Transition[toStates1.size()]));
			states2[i] = removeDuplicates(toStates2.toArray(new Transition[toStates2.size()]));
		}

		return new FST[] {
			new FST(states1, initialState1, setOfInputs, setOfOutputs),
				new FST(states2, initialState2, setOfInputs, setOfOutputs)
		};

	}

	public FST[] crossOverBasedOnLtl(FST that) {
		return crossOverByMarked(that, new ITransitionChecker(){
			public boolean isMarked(Transition t) {
				return t.isVerified() && !t.isUsedByVerifier();
			}
		});
	}

	public FST[] crossOverByMarked(FST that, ITransitionChecker check) {
		int initialState1;
		int initialState2;

		if (RANDOM.nextBoolean()) {
			initialState1 = this.initialState;
			initialState2 = that.initialState;
		}
		else {
			initialState1 = that.initialState;
			initialState2 = this.initialState;
		}

		Transition[][] states1 = new Transition[stateNumber][];
		Transition[][] states2 = new Transition[stateNumber][];

		for (int i = 0; i < stateNumber; i++) {
			int degree1 = this.states[i].length;
			int degree2 = that.states[i].length;

			states1[i] = new Transition[degree1];
			states2[i] = new Transition[degree2];
			int p1 = 0;
			int p2 = 0;

			ArrayList<Transition> list = new ArrayList<Transition>();
			for (Transition t : this.states[i]) {
				if (check.isMarked(t)) {
					states1[i][p1++] = t.copy(setOfInputs, setOfOutputs, stateNumber);
				}
				else {
					list.add(t);
				}
			}
			for (Transition t : that.states[i]) {
				if (check.isMarked(t)) {
					states2[i][p2++] = t.copy(setOfInputs, setOfOutputs, stateNumber);
				}
				else {
					list.add(t);
				}
			}
			Collections.shuffle(list);

			degree1 -= p1;
			degree2 -= p2;

			for (int j = 0; j < degree1; j++) {
				states1[i][p1++] = list.get(j).copy(setOfInputs, setOfOutputs, stateNumber);
			}
			for (int j = 0; j < degree2; j++) {
				states2[i][p2++] = list.get(degree1 + j).copy(setOfInputs, setOfOutputs, stateNumber);
			}
			states1[i] = removeDuplicates(states1[i]);
			states2[i] = removeDuplicates(states2[i]);
		}

		return new FST[] {
			new FST(states1, initialState1, setOfInputs, setOfOutputs),
				new FST(states2, initialState2, setOfInputs, setOfOutputs)
		};
	}*/

	/*public FST[] crossOver(FST that) {
		int initialState1;
		int initialState2;

		if (RANDOM.nextBoolean()) {
			initialState1 = this.initialState;
			initialState2 = that.initialState;
		}
		else {
			initialState1 = that.initialState;
			initialState2 = this.initialState;
		}

		Transition[][] states1 = new Transition[stateNumber][];
		Transition[][] states2 = new Transition[stateNumber][];

		for (int i = 0; i < stateNumber; i++) {
			int degree1 = this.states[i].length;
			int degree2 = that.states[i].length;

			ArrayList<Transition> list = new ArrayList<Transition>();
			for (Transition t : this.states[i]) {
				list.add(t);
			}
			for (Transition t : that.states[i]) {
				list.add(t);
			}
			Collections.shuffle(list);

			if (RANDOM.nextBoolean()) {
				int t = degree1;
				degree1 = degree2;
				degree2 = t;
			}
			states1[i] = new Transition[degree1];
			states2[i] = new Transition[degree2];

			for (int j = 0; j < degree1; j++) {
				states1[i][j] = list.get(j).copy(setOfInputs, setOfOutputs, stateNumber);
			}
			for (int j = 0; j < degree2; j++) {
				states2[i][j] = list.get(degree1 + j).copy(setOfInputs, setOfOutputs, stateNumber);
			}
			states1[i] = removeDuplicates(states1[i]);
			states2[i] = removeDuplicates(states2[i]);
		}

		return new FST[] {
			new FST(states1, initialState1, setOfInputs, setOfOutputs),
				new FST(states2, initialState2, setOfInputs, setOfOutputs)
		};
	}

	private Transition[] removeDuplicates(Transition[] transitions) {
		ArrayList<Transition> list = new ArrayList<Transition>();
		//		HashSet<String> usedInputs = new HashSet<String>();
		ComplianceChecker complianceChecker = ComplianceChecker.getComplianceChecker();
		for (Transition t : transitions) {
			boolean compliant = true;
			for (Transition t1 : list) {
				compliant &= complianceChecker.checkCompliancy(t.input, t1.input);
			}
			if (compliant) {
				list.add(t);
			}
		}
		Transition[] res = new Transition[list.size()];
		{
			int i = 0;
			for (Transition t : list) {
				res[i++] = t;
			}
		}
		return res;
	}

	public FST mutate() {
		int newInitialState;
		if (RANDOM.nextDouble() < Const.MUTATION_THRESHOLD) {
			newInitialState = RANDOM.nextInt(stateNumber);
		}
		else {
			newInitialState = initialState;
		}
		Transition[][] newStates = new Transition[stateNumber][];
		for (int i = 0; i < stateNumber; i++) {
			newStates[i] = new Transition[states[i].length];
			for (int j = 0; j < states[i].length; j++) {
				newStates[i][j] = states[i][j].copy(setOfInputs, setOfOutputs, stateNumber);

				Transition t = states[i][j];
				if ((t.isUsedByVerifier() || t.isUsedByNegativeTest())
					&& (RANDOM.nextDouble() < Const.VERIFIER_MUTATION_PROBABILITY)) {
					//mutate if is in counterexample or last transition in negative test
					newStates[i][j] = t.mutate(setOfInputs, setOfOutputs, stateNumber);
				}
			}
		}
		for (int i = 0; i < stateNumber; i++) {
			for (int j = 0; j < newStates[i].length; j++) {
				if (RANDOM.nextDouble() < Const.MUTATION_THRESHOLD) {
					newStates[i][j] = newStates[i][j].mutate(setOfInputs, setOfOutputs, stateNumber);
				}
			}
		}
		for (int i = 0; i < stateNumber; i++) {
			if (RANDOM.nextDouble() < Const.MUTATION_THRESHOLD) {
				if (RANDOM.nextBoolean()) {
					// ???????? ????????
					if (newStates[i].length > 0) {
						newStates[i] = deleteTransition(newStates[i]);
					}
				}
				else {
					// ?????????? ????????
					if (newStates[i].length < setOfInputs.length) {
						newStates[i] = addTransition(newStates[i]);
					}
				}
			}
		}
		for (int i = 0; i < stateNumber; i++) {
			newStates[i] = removeDuplicates(newStates[i]);
		}
		return new FST(newStates, newInitialState, setOfInputs, setOfOutputs);
	}

	private Transition[] addTransition(Transition[] t) {
		Transition[] res = new Transition[t.length + 1];
		HashSet<String> set = new HashSet<String>();
		for (int i = 0; i < t.length; i++) {
			res[i] = t[i];
			set.add(t[i].input);
		}
		ArrayList<String> notUsed = new ArrayList<String>();
		for (String input : setOfInputs) {
			if (!set.contains(input)) {
				notUsed.add(input);
			}
		}
		String input = notUsed.get(RANDOM.nextInt(notUsed.size()));
		res[t.length] = new Transition(input, 1, RANDOM.nextInt(stateNumber));
		return res;
	}

	private Transition[] deleteTransition(Transition[] s) {
		Transition[] res = new Transition[s.length - 1];
		int toDelete;
		//if transition is in verifier stack or is last transition in negative test than remove it
		for (toDelete = 0; (toDelete < s.length); toDelete++) {
			Transition t = s[toDelete];
			if (t.isUsedByVerifier() || t.isUsedByNegativeTest()) {
				break;
			}
		}
		if (toDelete >= s.length) {
			toDelete = RANDOM.nextInt(s.length);
		}

		int j = 0;
		for (int i = 0; i < s.length; i++) {
			if (i != toDelete) {
				res[j++] = s[i];
			}
		}
		return res;
	}*/
protected:
	bool fitnessCalculated;
	double fitness;

public:

	/**
	* Get all transitions count
	* @return transition count
	*/
	int getTransitionsCount() 
	{
		int res = 0;
		for (int i = 0; i < stateNumber; i++) {
			res += states[i].size();
		}
		return res;
	}

	/**
	* Get reached transitions count
	* @return reached transtions count
	*/
	int getUsedTransitionsCount() 
	{
		bool *visited = new bool[states.size()];
		for (int i = 0; i < states.size(); ++i)
			visited[i] = false;
		int res = getUsedTransitionsCount(initialState, visited);
		delete[] visited;
		return res;
	}

protected:
	int getUsedTransitionsCount(int state, bool *vizited) 
	{
		vizited[state] = true;
		int res = states[state].size();
		for (Transition t : states[state]) 
		{
			if (!vizited[t.getNextState()]) 
			{
				res += getUsedTransitionsCount(t.getNextState(), vizited);
			}
		}
		return res;
	}

public:
	/*void doLabelling(ArrayList<Path> tests) {
		if (isLabelled) {
			return;
		}
		for (Transition[] state : states) {
			for (Transition t : state) {
				t.beginLabeling();
			}
		}
		for (Path test : tests) {
			int currentState = initialState;
			String[] inputSequence = test.getInput();
			String[] outputSequence = test.getFixedOutput();
			boolean fixedOutput = true;
			if (outputSequence.length == 0) {
				outputSequence = test.getOutput();
				fixedOutput = false;
			}

			assert !fixedOutput || (inputSequence.length == outputSequence.length);

			int pOutput = 0;
			for (String s : inputSequence) {
				boolean found = false;
				for (Transition t : states[currentState]) {
					if (t.accepts(s)) {
						if (fixedOutput) {
							t.setOutputSize(1);
							t.addOutputSequence(outputSequence[pOutput++]);
						}
						else {
							StringBuilder sequence = new StringBuilder();
							for (int i = 0; i < t.getOutputSize(); i++) {
								if (i > 0) {
									sequence.append(",");
								}
								if (pOutput < outputSequence.length) {
									sequence.append(outputSequence[pOutput++]);
								}
								else {
									sequence.append("???");
								}
							}
							t.addOutputSequence(sequence.toString());
						}
						currentState = t.getNewState();
						found = true;
						break;
					}
				}
				if (!found) {
					break;
				}
			}
		}
		for (Transition[] state : states) {
			for (Transition t : state) {
				t.labelByMostFrequent();
			}
		}
		isLabelled = true;
	}*/

	std::vector<OUTPUT_TYPE> transform(const std::vector<INPUT_TYPE>& inputSequence) {
		std::vector<OUTPUT_TYPE> list;
		int currentState = initialState;
		for (INPUT_TYPE s : inputSequence) {
			bool found = false;
			for (Transition t : states[currentState]) {
				if (t.accepts(s)) {
					for (INPUT_TYPE s1 : t.getOutput()) {
						list.push_back(s1);
					}
					currentState = t.getNextState();
					found = true;
					break;
				}
			}
			if (!found) {
				return list;
			}
		}
		return list;
	}

	/*public boolean validateNegativeTest(String[] inputSequence) {
		if (ArrayUtils.isEmpty(inputSequence)) {
			throw new IllegalArgumentException("Unexpected inputSequence");
		}
		int currentState = initialState;
		Transition lastTransition = null;
		for (String s : inputSequence) {
			lastTransition = null;
			for (Transition t : states[currentState]) {
				if (t.accepts(s)) {
					currentState = t.getNewState();
					lastTransition = t;
					break;
				}
			}
			if (lastTransition == null) {
				break;
			}
		}
		if (lastTransition == null) {
			return true;
		}
		else {
			lastTransition.setUsedByNegativeTest(true);
			return false;
		}
	}*/

	void unmarkAllTransitions() {
		for (std::vector<Transition>& s : states) {
			for (Transition t : s) {
				t.markUnused();
				t.setUsedByNegativeTest(false);
				t.setUsedByVerifier(false);
				t.setVerified(false);
			}
		}
	}

	/*static void printAutomaton(std::ostream& out, const TestAutomat& fst, const std::vector<TEST_TYPE>& tests) {
		/*out.println(fst.getFitness());
		out.println(fst.stateNumber + " " + fst.initialState);
		for (int i = 0; i < fst.setOfInputs.length; i++) {
		out.print(fst.setOfInputs[i] + " ");
		}
		out.println();
		for (int i = 0; i < fst.stateNumber; i++) {
		for (int j = 0; j < fst.states[i].length; j++) {
		out.print(fst.states[i][j] + " | ");
		}
		out.println();
		}
		for (Path p : tests) {
		String[] output = fst.transform(p.getInput());
		String[] answer = p.getOutput();
		out.println(Arrays.toString(p.getInput()));
		out.println(Arrays.toString(answer));
		out.println(Arrays.toString(output));
		out.println();
		}*/
		/*try {
			UnimodModelWriter.write(out, fst, "ru.ifmo.ControlledObjectStub", "ru.ifmo.EventPrividerStub");
		}
		catch (IOException e) {
			e.printStackTrace();
		}
	}*/

};