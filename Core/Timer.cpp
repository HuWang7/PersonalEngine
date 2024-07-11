#include "Engine/Core/Timer.hpp"
#include "Engine/Core/Clock.hpp"


Timer::Timer(float period, const Clock* clock /*= nullptr*/)
	: m_period(period)
	,m_clock(clock ? clock : &Clock::GetSystemClock()) 
{
	Start();
}

void Timer::Start()
{
	m_startTime = m_clock->GetTotalSeconds();
}

void Timer::Stop()
{
	m_startTime = 0.0f;

}

float Timer::GetElapsedTime() const
{
	if (IsStopped()) return 0.0f;
	return m_clock->GetTotalSeconds() - m_startTime;
}

float Timer::GetElapsedFraction() const
{
	if (IsStopped()) return 0.0f;
	return GetElapsedTime() / m_period;
}

bool Timer::IsStopped() const
{
	return m_startTime == 0.0f;
}

bool Timer::HasPeriodElapsed() const
{
	return !IsStopped() && GetElapsedTime() >= m_period;

}

bool Timer::DecrementPeriodIfElapsed()
{
	if (HasPeriodElapsed()) {
		if (GetElapsedTime() >= m_period) {
			m_startTime += m_period;
		}
		return true;
	}
	return false;
}
