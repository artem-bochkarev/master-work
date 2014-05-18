#include "TimeRun/CTimeCounter.h"
#include <string>

//static const std::string perfFitnesFunctionCPU("Fitnes function CPU");
static const std::string perfFitnesFunctionCL_CPU("Fitnes function CL on CPU");
static const std::string perfFitnesFunctionCL_GPU("Fitnes function CL on GPU");
static const std::string perfFitnesFunctionCLJustKernelCPU("Fitnes function CL on CPU(only kernel time)");
static const std::string perfFitnesFunctionCLJustKernelGPU("Fitnes function CL on GPU(only kernel time)");
static const std::string perfFitnesFunctionBoth("Fitnes function CPU+GPU");
static const std::string perfNextGeneration("Next generation function");
static const std::string perfNextGenerationNF("Next generation(no fitnes)");
static const std::string perfFitnesFunctionCPU("Fitnes function no CL");