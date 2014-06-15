#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include <boost/filesystem.hpp>
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/Logger.h"
#include <string>
#include "TypeDefines.h"
#include "GeneticCommon/KernelRunner.h"
#include "TestsReader.h"

class TestBuildRunner : /*public CGeneticAlgorithm<COUNTERS_TYPE, INPUT_TYPE>,*/ public KernelRunner
{
public:
	TestBuildRunner(const std::string& clFileName, const std::string& xmlFileName, Tools::Logger& log);
	virtual ~TestBuildRunner();

	std::string getInfo() const;
	virtual void run() override;

	void prepareData();
	void getData(FITNES_TYPE* result) const;

	const GeneticSettings& getGeneticSettings() const;
	const TestsReader& getTestReader() const;

	const std::vector<TransitionListAutomat> getCurrentAutomats() const;
	const std::vector<TransitionListAutomat> getStartAutomats() const;

	static int compareLabelling(const TransitionListAutomat& a, const TransitionListAutomat& b, const TestsReader& testReader);
	static void clearLabels(TransitionListAutomat& automat, const TestsReader& testReader);
	static void doLabelling(std::vector<TransitionListAutomat>& automats, const TestsReader& testReader);
	static float calcFitness(const TransitionListAutomat& automat, const TestsReader& testReader);
	static void transform(const TransitionListAutomat& automat, std::vector<int>& result, size_t testNumber, const TestsReader& testReader);
	static float hammDist(const std::vector<int>& a, const std::vector<int>& b);

protected:
	void prepareFirstGeneration();
	
	virtual void createProgram(const boost::filesystem::path& source, const std::string& params) override;
	virtual void initCLBuffers() override;
	virtual std::string getOptions() const override;

	void setFromStrings(const std::vector< std::string >& strings);

	size_t m_size, bufSize;
	static const size_t constArraySize = 5;
private:
	TestsReader m_testReader;
	void checkProgrmType(const std::string &source);

	Tools::Logger& logger;
	std::vector<TransitionListAutomat> m_automatBuffer;
	std::vector<TransitionListAutomat> m_automatInitialBuffer;
	std::vector<cl_float> m_cachedResultBuffer;
	std::vector<cl_uint> m_srandsBuffer;

	cl_device_type deviceType;

	cl::Buffer clAutomatBuffer;
	cl::Buffer clSrandsBuffer;
	cl::Buffer clResultCacheBuffer;
	cl::Buffer clTestInfoBuffer, clTestsBuffer;

	cl::NDRange globalRange;
	cl::NDRange localRange;
};