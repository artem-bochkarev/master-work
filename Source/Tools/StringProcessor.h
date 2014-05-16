#pragma once
#include <string>
#include <vector>
#include <set>
namespace Tools
{
    class StringProcessor
    {
    public:
        StringProcessor() {}
        virtual ~StringProcessor() {}
        virtual std::string operator () ( const std::string& s ) const = 0;
    };

	typedef std::pair<std::string, std::string> Define;
	typedef std::vector<Define> DefinesVector;

	void changeDefines(std::string& text, const DefinesVector& );
	void changeDefine(std::string& text, const Define&);

	std::string splitToManyLines(const std::string& text, const std::set<char>& separators, size_t prefLength);
}