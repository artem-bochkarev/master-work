#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#include "TypeDefines.h"
#include "GeneticCommon/KernelRunner.h"

class GeneticChecks : public KernelRunner
{
public:
	GeneticChecks(const char*, size_t size, size_t elitismVal);
	virtual ~GeneticChecks();

	virtual void run() override;

	void prepareData(const cl_float* data);
	void getData(cl_uint* result) const;

protected:
	size_t m_size, eliteCount;
private:

	virtual void initCLBuffers() override;
	virtual void createProgram(const boost::filesystem::path& source, const std::string& params, Tools::Logger* log) override;

	float* cachedResults;

	cl_device_type deviceType;
	

	cl::Buffer clFLoatBuffer;
	cl::Buffer clResultBuffer;

	cl::NDRange globalRange;
	cl::NDRange localRange;
};