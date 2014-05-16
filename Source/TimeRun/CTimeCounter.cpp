#include "CTimeCounter.h"

static CTimeManager globalTimeManager;

CTimeManager& GetTimeManager()
{
	return globalTimeManager;
}

CTimeCounter::CTimeCounter(const std::string& str)
{
	startTimer();
	m_name = str;
}

CTimeCounter::~CTimeCounter()
{
	stopTimer();
	GetTimeManager().incrementCounter(m_name, boost::chrono::duration_cast<boost::chrono::nanoseconds>(stopTime - startTime));
}

void CTimeCounter::startTimer()
{
	startTime = boost::chrono::system_clock::now();
}

void CTimeCounter::stopTimer()
{
	stopTime = boost::chrono::system_clock::now();
}

void CTimeManager::incrementCounter(const std::string& name, boost::chrono::nanoseconds duration)
{
	if (m_timers.find(name) == m_timers.end())
	{
		m_timers[name] = TimerData(duration);
	}
	else
	{
		m_timers[name].counter++;
		m_timers[name].duration += duration;
	}
}

void CTimeManager::clean()
{
	m_timers.clear();
}

const std::map<std::string, TimerData>& CTimeManager::getTimers() const
{
	return m_timers;
}

TimerData::TimerData()
:counter(0), duration(0) {}

TimerData::TimerData(boost::chrono::nanoseconds duration)
:counter(1), duration(duration){}

TimerData::TimerData(boost::chrono::nanoseconds duration, int64_t counter)
: counter(counter), duration(duration){}