#include "SDKCustom.hpp"
#include "SDKPlatform.hpp"

cl::Context streamsdk::getContext( cl_device_type devType, PlatformInfo& platformInfo ) 
    {
        cl::vector< cl::Platform > platformList;
        cl::Platform::get(&platformList);

        for ( size_t i=0; i<platformList.size(); ++i )
        {
            SDKPlatform platform(platformList[i]);
            platform.getInfo( CL_PLATFORM_PROFILE, &platformInfo.sProfile );
            platform.getInfo( CL_PLATFORM_VENDOR, &platformInfo.sVendor );
            platform.getInfo( CL_PLATFORM_VERSION, &platformInfo.sVersion );
            platform.getInfo( CL_PLATFORM_NAME, &platformInfo.sName );

            cl_context_properties cprops[3] = 
            {CL_CONTEXT_PLATFORM, 
            (cl_context_properties)(platformList[i])(), 0};

            int n = platform.getDevicesCnt( devType );
            if ( n == 0 )
                continue;
            return cl::Context(devType, cprops);
        }
        return cl::Context();
    }
