#pragma once

namespace test_const
{

	static double MUTATION_THRESHOLD = 0.05;

	/**
	* Минимальное количество тестов, используемых при помечании используемых переходов
	*/
	static int MIN_USED_TESTS = 3;

	/**
	* Вероятность мутации перехода, если он оказался в контр примере верификатора
	*/
	static double VERIFIER_MUTATION_PROBABILITY = 0.1;

	/**
	* Вероятность примениния мутации или короссовера
	*/
	static double MUTATION_PROBABILITY = 0.5;
}

class TestParameters 
{
	bool fixedOutput;
	int populationSize;
	double desiredFitness;
	int stateNumber;
	double partStay;
	int timeSmallMutation;
	int timeBigMutation;
	double mutationProbability;

public:
	bool isFixedOutput() const
	{
		return fixedOutput;
	}

	void setFixedOutput(bool output) 
	{
		fixedOutput = fixedOutput;
	}

	int getPopulationSize() const
	{
		return populationSize;
	}

	void setPopulationSize(int size) 
	{
		populationSize = size;
	}

	double getDesiredFitness() const
	{
		return desiredFitness;
	}

	void setDesiredFitness(double fitness) 
	{
		desiredFitness = fitness;
	}

	int getStateNumber() const
	{
		return stateNumber;
	}

	void setStateNumber(int number) 
	{
		stateNumber = number;
	}

	double getPartStay() const
	{
		return partStay;
	}

	void setPartStay(double stay) 
	{
		partStay = stay;
	}

	int getTimeSmallMutation() const
	{
		return timeSmallMutation;
	}

	void setTimeSmallMutation(int time) 
	{
		timeSmallMutation = time;
	}

	int getTimeBigMutation() const
	{
		return timeBigMutation;
	}

	void setTimeBigMutation(int time) 
	{
		timeBigMutation = time;
	}

	double getMutationProbability() const
	{
		return mutationProbability;
	}

	void setMutationProbability(double probability) 
	{
		mutationProbability = probability;
	}
};