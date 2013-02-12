#pragma once
#include <string>
namespace Tools
{
    class StringProcessor
    {
    public:
        StringProcessor() {}
        virtual ~StringProcessor() {}
        virtual std::string operator () ( const std::string& s ) const = 0;
    };
}