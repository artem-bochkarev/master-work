#pragma once
#include <string>
#include <map>
#include <vector>
#include "TypeDefines.h"
#include <boost/property_tree/ptree.hpp>

struct GeneticSettings
{
	size_t populationSize;
	double desiredFitness;
	size_t stateNumber;
	double partStay;
	size_t timeSmallMutation;
	size_t timeBigMutation;
	double mutationProbability;

	GeneticSettings();
	void readSettings(const boost::property_tree::ptree& pt);
};

struct Test
{
	std::vector<std::string> input;
	std::vector<std::string> output;
	Test(const std::string& a, const std::string& b);
};

class TestsReader
{
	GeneticSettings m_settings;
	std::map<std::string, size_t> m_inputToNumber;
	std::map<std::string, size_t> m_outputToNumber;
	std::map<size_t, std::string> m_numberToInput;
	std::map<size_t, std::string> m_numberToOutput;

	std::vector<Test> m_tests;

	std::vector<TestInfo> m_testInfos;

	std::vector<cl_uint> m_testsBuffer;

	void createTests();

	static void createMaps(const std::string& str, std::map<std::string, size_t>& from, std::map<size_t, std::string>& to);
	static void readTests(const boost::property_tree::ptree& propertyTree, std::vector<Test>& vTests);
public:
	TestsReader();
	~TestsReader();
	
	void processFile(const std::string& fName);

	size_t getInputsCount() const;
	size_t getOutputsCount() const;

	size_t getTestInfosSize() const;
	size_t getTestsBufferSize() const;

	const void* getTestInfosPtr() const;
	const void* getTestsBufferPtr() const;

	size_t getTestsCount() const;
};

