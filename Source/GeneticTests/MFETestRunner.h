#pragma once
#include "GeneticAPI/CGeneticAlgorithm.h"
#include "TypeDefines.h"
#include "GeneticCommon/KernelRunner.h"

class MFETestRunner : public KernelRunner
{
public:
	MFETestRunner(const char*, size_t size, size_t checkElements, size_t mfeSize);
	virtual ~MFETestRunner();

	virtual void run() override;

	void prepareData(const cl_float* data);
	void getData(cl_float* result) const;

protected:
	size_t m_size, m_checkElements, m_mfeSize;
private:

	virtual void initCLBuffers() override;
	virtual void createProgram(const boost::filesystem::path& source, const std::string& params) override;

	float* cachedResults;

	cl_device_type deviceType;


	cl::Buffer clFLoatBuffer;
	cl::Buffer clResultBuffer;

	cl::NDRange globalRange;
	cl::NDRange localRange;
};