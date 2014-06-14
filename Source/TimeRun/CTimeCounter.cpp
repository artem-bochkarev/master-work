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
}

void CTimeCounter::stop()
{
	stopTimer();
}

void CTimeCounter::startTimer()
{
	m_bRunning = true;
	startTime = CLOCK_TYPE::now();
}

void CTimeCounter::stopTimer()
{
	if (m_bRunning)
	{
		m_bRunning = false;
		stopTime = CLOCK_TYPE::now();
		GetTimeManager().incrementCounter(m_name, boost::chrono::duration_cast<boost::chrono::nanoseconds>(stopTime - startTime));
	}
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