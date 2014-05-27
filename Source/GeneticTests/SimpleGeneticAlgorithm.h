#include "TestAutomat.h"
#include "TestGroup.hpp"
#include "Test.hpp"
#include "TestFitnes.h"

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
	std::vector< Test<INPUT_TYPE, OUTPUT_TYPE> > tests;

	TestFitnes* fitnesCalculator;

	void init();

public:
	SimpleGeneticAlgorithm(Parameters parameters,
		const std::set<INPUT_TYPE>& inputs, const std::set<OUTPUT_TYPE>& outputs, std::vector< TestGroup<TEST_TYPE, FORMULA_TYPE> > groups);

	std::vector<FITNES_TYPE> generations;

public:
	TestAutomat go();

	std::vector<TEST_TYPE> getTests();

	FITNES_TYPE getDesiredFitness();

};