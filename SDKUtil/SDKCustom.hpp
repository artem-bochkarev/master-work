#pragma once
#define __CL_ENABLE_EXCEPTIONS //enable excpetions
#define __NO_STD_VECTOR //no std vector
#define __MAX_DEFAULT_VECTOR_SIZE 100
#include <CL/cl.hpp>
#include <vector>

namespace streamsdk
{
    struct PlatformInfo
    {
        std::string sProfile;
        std::string sName;
        std::string sVendor;
        std::string sVersion;
    };

    cl::Context getContext( cl_device_type devType, PlatformInfo& );
    void countRanges( cl::NDRange& localRange, cl::NDRange& globalRange, std::vector<size_t>& vals );
}