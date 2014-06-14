#include "TestsReader.h"
#include <fstream>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>


TestsReader::TestsReader()
{
}


TestsReader::~TestsReader()
{
}

void TestsReader::processFile(const std::string& fName)
{
	std::ifstream in(fName);
	boost::property_tree::ptree propertyTree;
	try
	{
		boost::property_tree::read_xml(in, propertyTree);

		m_settings.populationSize = propertyTree.get<size_t>("program.parameters.populationSize", 1024);
		m_settings.desiredFitness = propertyTree.get<double>("program.parameters.desiredFitness", 1.0);
		m_settings.stateNumber = propertyTree.get<size_t>("program.parameters.stateNumber", 4);
		m_settings.partStay = propertyTree.get<double>("program.parameters.partStay", 0.1);
		m_settings.timeSmallMutation = propertyTree.get<size_t>("program.parameters.timeSmallMutation", 0);
		m_settings.timeBigMutation = propertyTree.get<size_t>("program.parameters.timeBigMutation", 0);
		m_settings.mutationProbability = propertyTree.get<double>("program.parameters.mutationProbability", 0.01);

		m_input = propertyTree.get<std::string>("program.inputSet");
		m_output = propertyTree.get<std::string>("program.outputSet");

		//read tests
	}
	catch (boost::property_tree::xml_parser_error)
	{

		std::cout << "XML parser error!" << std::endl;

		throw;
	}
}

size_t TestsReader::getTestSizesSize() const
{
	return 0;
}

size_t TestsReader::getTestsBufferSize() const
{
	return 0;
}


const void* TestsReader::getTestSizesPtr() const
{
	return 0;
}

const void* TestsReader::getTestsBufferPtr() const
{
	return 0;
}


size_t TestsReader::getTestsCount() const
{
	return 0;
}

size_t TestsReader::getTestsSize() const
{
	return 0;
}

