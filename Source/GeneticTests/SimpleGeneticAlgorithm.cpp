#include "SimpleGeneticAlgorithm.h"

SimpleGeneticAlgorithm::SimpleGeneticAlgorithm(const TestParameters& parameters,
	const std::set<INPUT_TYPE>& inputs, const std::set<OUTPUT_TYPE>& outputs, std::vector< TestGroup<TEST_TYPE, FORMULA_TYPE> > groups)
{
	populationSize = parameters.getPopulationSize();
	maxStates = parameters.getStateNumber();
	partStay = parameters.getPartStay();
	mutationProb = parameters.getMutationProbability();
	timeSmallMutation = parameters.getTimeSmallMutation();
	timeBigMutation = parameters.getTimeBigMutation();
	setOfInputs = inputs;
	setOfOutputs = outputs;
	groups = groups;

	for (TestGroup<TEST_TYPE, FORMULA_TYPE>& g : groups)
	{
		tests.insert(tests.end(), g.getTests().begin(), g.getTests().end());
	}

	/*IVerifierFactory verifier = new VerifierFactory(setOfInputs, setOfOutputs);
	try {
	verifier.prepareFormulas(groups.toArray(new TestGroup[groups.size()]));
	}
	catch (LtlParseException e) {
	throw new RuntimeException(e);   //TODO: do something less stupid
	}*/
	fitnesCalculator = new TestFitnes();// (verifier, groups, tests);

	//desiredFitness = fitnessCalculator->evaluateDesiredFitness(groups) + parameters.getDesiredFitness();
}

void SimpleGeneticAlgorithm::init() {
	curGeneration.resize(populationSize);
	for (int i = 0; i < populationSize; i++) {
		curGeneration[i] = TestAutomat::randomIndividual(maxStates, setOfInputs, setOfOutputs, rand);
	}
}

TestAutomat SimpleGeneticAlgorithm::go() 
{
	init();
	int genCount = 0;

	double lastBest = -1;
	int cntBestEqual = 0;

	std::vector<FITNES_TYPE> meanFitness;

	while (true) {
		double sumFitness = 0;
		
		std::vector<FITNES_TYPE> results(populationSize);
		fitnesCalculator->calcFitness(curGeneration, results);
		int bestPos = 0;

		FITNES_TYPE minFitness = results[0], maxFitness = results[0];
		for (int i = 0; i < populationSize; i++) {
			sumFitness += results[i];
			if (maxFitness < results[i]) {
				maxFitness = results[i];
				bestPos = i;
			}
			minFitness = std::min(minFitness, results[i]);
		}

		generations.push_back(maxFitness);
		meanFitness.push_back(sumFitness / populationSize);


		/*System.out.println("Generation " + genCount + " Max fitness = " + maxFitness + " Min fitness = " + minFitness + " Sum fitness = " + sumFitness);
		if (genCount % 200 == 0) {
			try {
				PrintWriter out = new PrintWriter(new File("gen" + genCount + ".xml"));
				FST.printAutomaton(out, bestAutomaton, tests);
				out.close();
			}
			catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
		}*/
		/*if (maxFitness >= desiredFitness) {
			try {
				PrintWriter out = new PrintWriter(new File("fitness_graph"));
				for (double x : generations) {
					out.println(x);
				}
				out.close();
			}
			catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			try {
				PrintWriter out = new PrintWriter(new File("mean_fitness_graph"));
				for (double x : meanFitness) {
					out.println(x);
				}
				out.close();
			}
			catch (FileNotFoundException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}

			return bestAutomaton;
		}*/

		if (std::abs(maxFitness - lastBest) < 1E-15) {
			cntBestEqual++;
		}
		else {
			lastBest = maxFitness;
			cntBestEqual = 0;
		}

		// Генерируем новое поколение
		std::vector<TestAutomat> nextGeneration(populationSize);
		

		// Применяем метод элитизма
		std::vector< std::pair<FITNES_TYPE, int> > fitToOrigin(results.size());
		for (int i = 0; i < results.size(); ++i)
		{
			fitToOrigin[i] = std::make_pair(results[i], i);
		}

		std::sort(fitToOrigin.begin(), fitToOrigin.end());

		int cntStay = (int)(partStay * populationSize);
		if ((populationSize - cntStay) % 2 == 1) {
			cntStay++;
		}

		int nextCnt = 0;
		for (int i = 0; i < cntStay; i++) {
			nextGeneration[nextCnt++] = curGeneration[fitToOrigin[fitToOrigin.size() - i - 1].second];
		}

		std::vector<TestAutomat> best(cntStay);
		for (int i = 0; i < cntStay; i++) {
			best[i] = curGeneration[fitToOrigin[fitToOrigin.size() - i - 1].second];
		}

		std::vector<bool> can(populationSize);
		can.assign(populationSize, true);
		// Теперь делаем кроссоверы (при этом используем всех)
		int cntAdd = populationSize - cntStay;
		for (int i = 0; i < cntAdd / 2; i++) {
			int num1 = rand->nextINT(populationSize);
			int num2 = num1;
			while (num1 == num2) {
				num2 = rand->nextINT(populationSize);
			}
			if ((double)rand->nextUINT(BIG_INT)/(double)BIG_INT > test_const::MUTATION_PROBABILITY) {
				/*FST[] toAdd = FST.crossOver(curGeneration[num1], curGeneration[num2], groups);
				for (int j = 0; j < 2; j++) {
					nextGeneration[nextCnt++] = toAdd[j];
				}*/
			}
			else {
				//nextGeneration[nextCnt++] = curGeneration[num1].mutate();
				//nextGeneration[nextCnt++] = curGeneration[num2].mutate();
			}
		}

		// Делаем мутации 
		for (int i = 0; i < populationSize; i++) {
			/*if (random.nextDouble() < mutationProb) {
				nextGeneration[i] = nextGeneration[i].mutate();
			}*/
		}

		if (cntBestEqual >= timeSmallMutation) {
			// "Малая" мутация поколения
			for (int i = populationSize / 10; i < populationSize; i++) {
				//nextGeneration[i] = nextGeneration[i].mutate();
			}
		}

		if (cntBestEqual >= timeBigMutation) {
			// "Большая" мутация поколения
			int start = 0;
			for (int i = start; i < populationSize; i++) {
				//nextGeneration[i] = FST.randomIndividual(maxStates, setOfInputs, setOfOutputs);
			}
		}

		// Новое поколение готово
		curGeneration = nextGeneration;
		genCount++;
	}
}