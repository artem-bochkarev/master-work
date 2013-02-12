#pragma once
#include <fstream>
#include <string>

namespace Tools
{

	class Logger
	{
	public:
		Logger();
		Logger( const std::string& fileName );
		~Logger();
		Logger& operator << ( const std::string& );
		Logger& operator << ( const char* );
		Logger& operator << ( int n );
	private:
		std::ofstream ofs;
	};

}