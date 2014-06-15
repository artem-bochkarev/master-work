#include "TestsReader.h"
#include <fstream>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/classification.hpp>
#include "Tools/helper.hpp"

GeneticSettings::GeneticSettings()
	:populationSize(256), desiredFitness(1.0), stateNumber(4),
		partStay(0.1), timeSmallMutation(0), timeBigMutation(0), mutationProbability(0.01)
{}

void GeneticSettings::readSettings(const boost::property_tree::ptree& propertyTree)
{
	populationSize = propertyTree.get<size_t>("program.parameters.populationSize", 1024);
	desiredFitness = propertyTree.get<double>("program.parameters.desiredFitness", 1.0);
	stateNumber = propertyTree.get<size_t>("program.parameters.stateNumber", 4);
	partStay = propertyTree.get<double>("program.parameters.partStay", 0.1);
	timeSmallMutation = propertyTree.get<size_t>("program.parameters.timeSmallMutation", 0);
	timeBigMutation = propertyTree.get<size_t>("program.parameters.timeBigMutation", 0);
	mutationProbability = propertyTree.get<double>("program.parameters.mutationProbability", 0.01);

	maxStateOutputCount = propertyTree.get<size_t>("program.parameters.maxStateOutput", 3);
	maxStateTransitions = propertyTree.get<size_t>("program.parameters.maxStateTransitions", 5);
}

Test::Test(const std::string& a, const std::string& b)
{
	boost::algorithm::split(input, a, boost::algorithm::is_any_of(","), boost::algorithm::token_compress_on);
	boost::algorithm::split(output, b, boost::algorithm::is_any_of(","), boost::algorithm::token_compress_on);
	for (std::string& str : input)
		boost::algorithm::trim(str);
	for (std::string& str : output)
		boost::algorithm::trim(str);
}

TestsReader::TestsReader()
{
}


TestsReader::~TestsReader()
{
}

void TestsReader::createMaps(const std::string& str, std::map<std::string, size_t>& from, std::map<size_t, std::string>& to)
{
	std::vector<std::string> inputs;
	boost::algorithm::split(inputs, str, boost::algorithm::is_any_of(","), boost::algorithm::token_compress_on);
	size_t k = 0;
	for (std::string& str : inputs)
	{
		boost::algorithm::trim(str);
		to[k] = str;
		from[str] = k;
		++k;
	}
}

void TestsReader::readTests(const boost::property_tree::ptree& propertyTree, std::vector<Test>& vTests)
{
	boost::property_tree::ptree program = propertyTree.get_child("program");
	for (boost::property_tree::ptree::value_type &group : program)
	{
		if (group.first == "group")
		{
			boost::property_tree::ptree tests = group.second.get_child("tests");
			for (boost::property_tree::ptree::value_type &test : tests)
			{
				if (test.first == "test")
				{
					std::string input = test.second.get<std::string>("input", "null");
					std::string output = test.second.get<std::string>("output", "null");
					Test newTest(input, output);
					vTests.push_back(newTest);
				}
			}
		}
	}
}

void TestsReader::createTests()
{
	size_t needMem = 0;
	size_t maxTestInputLength = 0;
	size_t maxTestOutputLength = 0;

	m_testInfos.resize(m_tests.size());
	for (size_t i = 0; i < m_tests.size(); ++i)
	{
		m_testInfos[i].offset = needMem;

		size_t lenIn = m_tests[i].input.size();
		m_testInfos[i].inputLength = lenIn;
		maxTestInputLength = (maxTestInputLength < lenIn) ? lenIn : maxTestInputLength;
		
		size_t lenOut = m_tests[i].output.size();
		m_testInfos[i].outputLength = lenOut;
		maxTestOutputLength = (maxTestOutputLength < lenOut) ? lenOut : maxTestOutputLength;

		size_t k = (lenIn + lenOut) % 4;
		k = (k > 0) ? 4 - k : 0;
		needMem += (lenIn + lenOut + k);
	}
	m_maxTestInputLength = maxTestInputLength;
	m_maxTestOutputLength = maxTestOutputLength;

	m_testsBuffer.resize(needMem);
	for (size_t i = 0; i < m_tests.size(); ++i)
	{
		size_t k = 0;
		for (const std::string& s : m_tests[i].input)
		{
			m_testsBuffer[m_testInfos[i].offset + k] = m_inputToNumber[s];
			++k;
		}
		for (const std::string& s : m_tests[i].output)
		{
			m_testsBuffer[m_testInfos[i].offset + k] = m_outputToNumber[s];
			++k;
		}
	}
}

void TestsReader::processFile(const std::string& fName)
{
	std::ifstream in(fName);
	boost::property_tree::ptree propertyTree;
	try
	{
		boost::property_tree::read_xml(in, propertyTree);

		m_settings.readSettings(propertyTree);

		std::string input = propertyTree.get<std::string>("program.inputSet");
		std::string output = propertyTree.get<std::string>("program.outputSet");

		createMaps(input, m_inputToNumber, m_numberToInput);
		createMaps(output, m_outputToNumber, m_numberToOutput);

		readTests(propertyTree, m_tests);
	}
	catch (boost::property_tree::xml_parser_error& err)
	{
		Tools::throwDetailedFailed("XML parser error!", err.what(), 0);
	}
	createTests();
}

const GeneticSettings& TestsReader::getGeneticSettings() const
{
	return m_settings;
}

size_t TestsReader::getInputsCount() const
{
	return m_inputToNumber.size();
}

size_t TestsReader::getOutputsCount() const
{
	return m_outputToNumber.size();
}

size_t TestsReader::getTestInfosSize() const
{
	return getTestsCount() * sizeof(TestInfo);
}

size_t TestsReader::getTestsBufferSize() const
{
	return m_testsBuffer.size() * sizeof(cl_uint);
}


const void* TestsReader::getTestInfosPtr() const
{
	return m_testInfos.data();
}

const void* TestsReader::getTestsBufferPtr() const
{
	return m_testsBuffer.data();
}


size_t TestsReader::getTestsCount() const
{
	return m_tests.size();
}

size_t TestsReader::getMaxTestInputLength() const
{
	return m_maxTestInputLength;
}

size_t TestsReader::getMaxTestOutputLength() const
{
	return m_maxTestOutputLength;
}
