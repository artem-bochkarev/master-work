#pragma once
#include <string>

struct GeneticSettings
{
	size_t populationSize;
	double desiredFitness;
	size_t stateNumber;
	double partStay;
	size_t timeSmallMutation;
	size_t timeBigMutation;
	double mutationProbability;

	GeneticSettings()
		:populationSize(256), desiredFitness(1.0), stateNumber(4),
		partStay(0.1), timeSmallMutation(0), timeBigMutation(0), mutationProbability(0.01)
	{}
};

class TestsReader
{
	GeneticSettings m_settings;
	std::string m_input, m_output;
public:
	TestsReader();
	~TestsReader();
	
	void processFile(const std::string& fName);

	size_t getTestSizesSize() const;
	size_t getTestsBufferSize() const;

	const void* getTestSizesPtr() const;
	const void* getTestsBufferPtr() const;

	size_t getTestsCount() const;
	size_t getTestsSize() const;
};

