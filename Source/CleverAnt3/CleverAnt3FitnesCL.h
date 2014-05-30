#pragma once
#include "CleverAnt3Fitnes.h"
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#include "CL/cl.hpp"
#include <boost/filesystem.hpp>
#include "SDKUtil/include/SDKCommon.hpp"
#include "Tools/Logger.h"
#include <string>

class CCleverAnt3FitnesCL : public CCleverAnt3Fitnes
{
public:
	CCleverAnt3FitnesCL(const std::vector< std::string >& strings, Tools::Logger& log);
	virtual ~CCleverAnt3FitnesCL();

	virtual ANT_FITNES_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const override;
	virtual void fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result) override;

	virtual void setMaps(std::vector<CMapPtr> maps) override;
	std::string getInfo() const override;

	virtual void fitnes(const AUTOMAT* individs, ANT_FITNES_TYPE* result);
	
protected:
	void setFromStrings(const std::vector< std::string >& strings);
	void prepareData(const AUTOMAT* individs);
	void getData(ANT_FITNES_TYPE* result) const;
	void run() const;

	size_t m_size, mapSize, bufSize;
private:

	std::string getOptions() const;
	void initCLBuffers();
	void createProgram(const boost::filesystem::path& source, const std::string& params);
	void checkProgrmType(const std::string &source);

	Tools::Logger& logger;
	int* mapsBuffer;
	float* cachedResults;

	cl_device_type deviceType;
	streamsdk::SDKDeviceInfo deviceInfo;

	cl::Buffer statesBufCL1;
	cl::Buffer mapBufCL, mapCL, mapBufConst;
	cl::Buffer sizesBuf;
	cl::Buffer resultCache;

	cl::Kernel kernelGen;
	cl::Context context;
	cl::CommandQueue queue;
	std::vector<cl::Device> devices;
	cl::NDRange globalRange;
	cl::NDRange localRange;
};

typedef boost::shared_ptr<CCleverAnt3FitnesCL> CCleverAnt3FitnesCLPtr;


class CCleverAnt3FitnesCLBoth : public CCleverAnt3Fitnes
{
public:
	CCleverAnt3FitnesCLBoth(const std::vector< std::string >& strings, Tools::Logger& log);
	virtual ~CCleverAnt3FitnesCLBoth();

	virtual ANT_FITNES_TYPE fitnes(const CAutomat<COUNTERS_TYPE, INPUT_TYPE>* automat) const override;
	virtual void fitnes(const std::vector<AUTOMAT>& individs, std::vector<ANT_FITNES_TYPE>& result) override;

	virtual void setMaps(std::vector<CMapPtr> maps) override;
	virtual const CMapPtr getMap(size_t i) override;
	virtual size_t getMapsCount() override;
	std::string getInfo() const override;
protected:
	void checkSizes();
	CCleverAnt3FitnesCLPtr clCPUFitnes, clGPUFitnes;
	size_t gpuCount, cpuCount, m_size;
};
