#pragma once
#include <stdexcept>
#include <string>
#include "Logger.h"

namespace Tools
{
    inline void throwFailed( const std::string& msg, Tools::Logger* logger )
    {
        OutputDebugStringA( msg.c_str() );
        OutputDebugStringA( "\n" );
        if ( logger != 0 )
            *logger << "[FAILED] " << msg << "\n";
        throw std::runtime_error( msg );
    }

    inline void throwDetailedFailed( const std::string& msg, const char* descr, Tools::Logger* logger )
    {
        OutputDebugStringA( msg.c_str() );
        OutputDebugStringA( "\n" );
        if ( logger != 0 )
        {
            *logger << "[FAILED] " << msg << "\n";
            *logger << "[DESCRIPTION] " << descr << "\n";
        }
        throw std::runtime_error( msg );
    }
}