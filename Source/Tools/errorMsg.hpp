#pragma once
#include <stdexcept>
#include <string>
#include <sstream>
#include "Logger.h"

/*#ifdef _DEBUG
#include <..\um\debugapi.h>
#endif*/

namespace Tools
{
	class detailed_exception : public std::runtime_error
	{
	public:
		detailed_exception(const std::string& msg, const char* descr)
			: runtime_error(std::string(msg.c_str()).append(": ").append(descr)) {}
		detailed_exception(const char* msg, const char* descr)
			: runtime_error(std::string(msg).append(": ").append(descr)) {}
	};

	inline void printFailed(const std::string& msg, Tools::Logger* logger)
	{
		std::stringstream str;
		str << msg << std::endl;
/*#ifdef _DEBUG
		OutputDebugStringA(str.str().c_str());
#endif*/
		std::clog << str.str();
		//std::clo
		if (logger != 0)
			*logger << "[FAILED] " << msg << "\n";
	}

	inline void printDetailedFailed(const std::string& msg, const char* descr, Tools::Logger* logger)
	{
		printFailed(msg, logger);
		if (logger != 0)
			*logger << "[DESCRIPTION] " << descr << "\n";
		std::stringstream str;
		str << "    :" << descr << std::endl;
/*#ifdef _DEBUG
		OutputDebugStringA(str.str().c_str());
#endif*/
	}

    inline void throwFailed( const std::string& msg, Tools::Logger* logger )
    {
		printFailed(msg, logger);
        throw std::runtime_error( msg );
    }

    inline void throwDetailedFailed( const std::string& msg, const char* descr, Tools::Logger* logger )
    {
		printDetailedFailed(msg, descr, logger);
		throw detailed_exception(msg, descr);
    }
}