#pragma once
#include <string>
#include <boost/filesystem.hpp>
#include "Logger.h"
#include "StringProcessor.h"
#include "errorMsg.hpp"

namespace Tools
{
	inline void readFileToString(std::string& outStr, const boost::filesystem::path& sourceFile, Logger* logger = 0, StringProcessor* processStr = 0)
	{
		outStr.clear();
		if (logger != 0)
			*logger << "[INIT] Start building program.\n";
		using namespace std;
		ifstream file(sourceFile.native().c_str());
		if (!file.is_open())
			throwFailed("File not found", logger);

		while (file.good())
		{
			char str[300];
			file.getline(str, 290);
			size_t len = strlen(str);

			std::string s(str);
			if (processStr != 0)
				outStr.append((*processStr)(s));
			else
				outStr.append(s);
			outStr.append("\n");
		}
		file.close();
	}
}