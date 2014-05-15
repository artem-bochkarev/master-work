#pragma once
#include <boost/chrono.hpp>
#include <string>

class CTimeCounter
{
public:
	CTimeCounter(const std::string&);
	~CTimeCounter();
private:
	CTimeCounter();
	CTimeCounter(const CTimeCounter&);
	CTimeCounter& operator= (const CTimeCounter&);

	std::string m_name;
};

