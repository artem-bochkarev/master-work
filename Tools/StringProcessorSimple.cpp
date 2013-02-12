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
