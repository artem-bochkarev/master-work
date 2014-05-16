#include "STringProcessorSimple.h"
#include <boost/lexical_cast.hpp>

Tools::StringProcessorSimple::StringProcessorSimple( const std::vector<size_t>& vals )
:m_values(vals)
{}

Tools::StringProcessorSimple::~StringProcessorSimple()
{}

std::string Tools::StringProcessorSimple::operator ()( const std::string &str ) const
{
    int i = str.find("#define");
    if ( i >= 0 )
    {
        int j = str.find("max_linear_size");
        if ( j > i )
        {
            if ( m_values[0] > 0 )
                return "#define max_linear_size " + boost::lexical_cast<std::string, size_t>(m_values[0]) + "\n";
        }
        j = str.find("max_squared_size");
        if ( j > i )
        {
            if ( m_values[1] > 0 )
                return "#define max_squared_size " + boost::lexical_cast<std::string, size_t>(m_values[1]) + "\n";
        }
        j = str.find("max_tmp_buffer_size");
        if ( j > i )
        {
            if ( m_values[2] > 0 )
                return "#define max_tmp_buffer_size " + boost::lexical_cast<std::string, size_t>(m_values[2]) + "\n";
        }
    }
    return str;
}

void Tools::changeDefines(std::string& text, const DefinesVector&)
{

}

void Tools::changeDefine(std::string& text, const Define& def)
{
	std::string toFind = "#define " + def.first;
	int j = text.find(toFind);
	if (j > 0)
	{
		int a = text.find('\n', j+1);
		if (a > j)
		{
			std::string firstPart = text.substr(0, j);
			std::string secondPart = text.substr(a, text.length() - a);
			text = firstPart + toFind + " " + def.second + "\n" + secondPart;
		}
	}
}

std::string Tools::splitToManyLines(const std::string& text, const std::set<char>& separators, size_t prefLength)
{
	std::string result;

	size_t curLength = 0;
	for (size_t i = 0; i < text.length(); ++i)
	{
		result.push_back(text[i]);
		++curLength;
		if (separators.find(text[i]) != separators.end())
		{
			if (curLength >= prefLength)
			{
				result.push_back('\n');
				curLength = 0;
				if ( (i+1 < text.length()) && (text[i + 1] == ' ') )
					++i;
			}
		}
		else
		{
			if (text[i] == '\n')
				curLength = 0;
		}
	}
	return result;
}
