#include "TestAutomat.h"
#include "TestGroup.hpp"

class SimpleGeneticAlgorithm 
{
	double desiredFitness;
	int populationSize;
	int maxStates;

	std::vector<TestAutomat> curGeneration;
	CRandom* rand;

	/**
	* ƒол€ особей, переход€щих в следующее поколение.
	* —оответственно, остальные особи следующего поколени€
	* будут получены с помощью кроссовера из неперешедших
	* в новое поколение.
	*/
	double partStay;

	/**
	* ¬еро€тность, с которой происход€т мутации.
	*/
	double mutationProb;

	/*
	*  оличество поколений, по прошествии которого при отсутствии прогресса фитнес-функции
	* происходит "мала€" мутаци€ поколени€
	*/
	int timeSmallMutation;

	/*
	*  оличество поколений, по прошествии которого при отсутствии прогресса фитнес-функции
	* происходит "больша€" мутаци€ поколени€
	*/
	int timeBigMutation;

	std::set<INPUT_TYPE> setOfInputs;
	std::set<OUTPUT_TYPE> setOfOutputs;

	std::vector< TestGroup<TEST_TYPE, FORMULA_TYPE> > groups;
	std::vector<Test> tests;

public:
	SimpleGeneticAlgorithm(Parameters parameters,
		String[] setOfInputs, String[] setOfOutputs, List<TestGroup> groups) {
		this.populationSize = parameters.getPopulationSize();
		this.maxStates = parameters.getStateNumber();
		this.partStay = parameters.getPartStay();
		this.mutationProb = parameters.getMutationProbability();
		this.timeSmallMutation = parameters.getTimeSmallMutation();
		this.timeBigMutation = parameters.getTimeBigMutation();
		this.setOfInputs = setOfInputs;
		this.setOfOutputs = setOfOutputs;
		this.groups = groups;
		this.tests = new ArrayList<Path>();
		for (TestGroup g : groups) {
			tests.addAll(g.getTests());
		}

		IVerifierFactory verifier = new VerifierFactory(setOfInputs, setOfOutputs);
		try {
			verifier.prepareFormulas(groups.toArray(new TestGroup[groups.size()]));
		}
		catch (LtlParseException e) {
			throw new RuntimeException(e);   //TODO: do something less stupid
		}
		FST.fitnessCalculator = new FitnessCalculator(verifier, groups, tests);

		this.desiredFitness = FST.fitnessCalculator.evaluateDesiredFitness(groups) + parameters.getDesiredFitness();
	}

	ArrayList<Double> generations = new ArrayList<Double>();

	/**
	* »нициализации попул€ции случайными особ€ми.
	*
	*/
	private void init() {
		curGeneration = new FST[populationSize];
		for (int i = 0; i < populationSize; i++) {
			FST toAdd = FST.randomIndividual(maxStates, setOfInputs, setOfOutputs);
			curGeneration[i] = toAdd;
		}
	}

	public FST go() {
		init();
		int genCount = 0;

		double lastBest = -1;
		int cntBestEqual = 0;

		ArrayList<Double> meanFitness = new ArrayList<Double>();

		while (true) {

			double maxFitness = Integer.MIN_VALUE;
			double minFitness = Integer.MAX_VALUE;
			double sumFitness = 0;
			FST bestAutomaton = null;
			for (int i = 0; i < populationSize; i++) {
				sumFitness += curGeneration[i].getFitness();
				if (maxFitness < curGeneration[i].getFitness()) {
					maxFitness = curGeneration[i].getFitness();
					bestAutomaton = curGeneration[i];
				}
				minFitness = Math.min(minFitness, curGeneration[i].getFitness());
			}

			generations.add(maxFitness);
			meanFitness.add(sumFitness / populationSize);


			System.out.println("Generation " + genCount + " Max fitness = " + maxFitness + " Min fitness = " + minFitness + " Sum fitness = " + sumFitness);
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
			}
			if (maxFitness >= desiredFitness) {
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
			}

			if (Math.abs(maxFitness - lastBest) < 1E-15) {
				cntBestEqual++;
			}
			else {
				lastBest = maxFitness;
				cntBestEqual = 0;
			}

			// √енерируем новое поколение
			FST[] nextGeneration = new FST[populationSize];
			int nextCnt = 0;

			// ѕримен€ем метод элитизма
			Arrays.sort(curGeneration, new Comparator<FST>() {
				public int compare(FST a1, FST a2) {
					return Double.compare(a2.getFitness(), a1.getFitness());
				}
			}
			);

			int cntStay = (int)(partStay * populationSize);
			if ((populationSize - cntStay) % 2 == 1) {
				cntStay++;
			}
			for (int i = 0; i < cntStay; i++) {
				nextGeneration[nextCnt++] = curGeneration[i];
			}

			FST[] best = new FST[cntStay];
			for (int i = 0; i < cntStay; i++) {
				best[i] = curGeneration[i];
			}

			boolean[] can = new boolean[populationSize];
			Arrays.fill(can, true);
			// “еперь делаем кроссоверы (при этом используем всех)
			int cntAdd = populationSize - cntStay;
			for (int i = 0; i < cntAdd / 2; i++) {
				int num1 = random.nextInt(populationSize);
				int num2 = num1;
				while (num1 == num2) {
					num2 = random.nextInt(populationSize);
				}
				if (random.nextDouble() > Const.MUTATION_PROBABILITY) {
					FST[] toAdd = FST.crossOver(curGeneration[num1], curGeneration[num2], groups);
					for (int j = 0; j < 2; j++) {
						nextGeneration[nextCnt++] = toAdd[j];
					}
				}
				else {
					nextGeneration[nextCnt++] = curGeneration[num1].mutate();
					nextGeneration[nextCnt++] = curGeneration[num2].mutate();
				}
			}

			// ƒелаем мутации 
			for (int i = 0; i < populationSize; i++) {
				if (random.nextDouble() < mutationProb) {
					nextGeneration[i] = nextGeneration[i].mutate();
				}
			}

			if (cntBestEqual >= timeSmallMutation) {
				// "ћала€" мутаци€ поколени€
				for (int i = populationSize / 10; i < populationSize; i++) {
					nextGeneration[i] = nextGeneration[i].mutate();
				}
			}

			if (cntBestEqual >= timeBigMutation) {
				// "Ѕольша€" мутаци€ поколени€
				int start = 0;
				for (int i = start; i < populationSize; i++) {
					nextGeneration[i] = FST.randomIndividual(maxStates, setOfInputs, setOfOutputs);
				}
			}

			// Ќовое поколение готово
			curGeneration = nextGeneration;
			genCount++;
		}
	}

	std::vector<TEST_TYPE> getTests();

	double getDesiredFitness();

};