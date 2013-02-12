#pragma once
#include "StringProcessor.h"
#include <vector>
namespace Tools
{
    class StringProcessorSimple : public StringProcessor
    {
        StringProcessorSimple();
        std::vector<size_t> m_values;
    public:
        StringProcessorSimple( const std::vector<size_t>& vals );
        virtual ~StringProcessorSimple();
        std::string operator () ( const std::string& s ) const;
    };
}