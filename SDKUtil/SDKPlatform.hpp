#include "CL/cl.hpp"

class SDKPlatform : public cl::Platform
{
public:

    SDKPlatform( const cl::Platform& platform )
        :Platform(platform) {}

    cl_int getDevicesCnt(
        cl_device_type type ) const
    {
        cl_uint n = 0;
        cl_int err = ::clGetDeviceIDs(object_, type, 0, NULL, &n);
        return n;
    }
};
