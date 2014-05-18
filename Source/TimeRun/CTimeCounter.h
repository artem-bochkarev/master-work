#pragma once
#include <boost/chrono.hpp>
#include <string>
#include <map>

struct TimerData
{
	TimerData();
	TimerData(boost::chrono::nanoseconds duration);
	TimerData(boost::chrono::nanoseconds duration, int64_t counter);

	boost::chrono::nanoseconds duration;
	int64_t counter;
};

class CTimeManager
{
	std::map<std::string, TimerData> m_timers;
public:
	void incrementCounter(const std::string& name, boost::chrono::nanoseconds duration);
	void clean();
	const std::map<std::string, TimerData>& getTimers() const;
};

CTimeManager& GetTimeManager();

class CTimeCounter
{
public:
	CTimeCounter(const std::string&);
	~CTimeCounter();
	//force stop
	void stop();
private:
	CTimeCounter();
	CTimeCounter(const CTimeCounter&);
	CTimeCounter& operator= (const CTimeCounter&);

	void startTimer();
	void stopTimer();

	boost::chrono::system_clock::time_point startTime, stopTime;
	std::string m_name;
	bool m_bRunning;
};

