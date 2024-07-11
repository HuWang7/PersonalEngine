#include "Engine/Core/Clock.hpp"
#include "Engine/Core/Time.hpp"

Clock* g_systemClock = new Clock();

Clock::Clock()
{
	if (g_systemClock == nullptr) {
		m_parent = nullptr; 
	}
	else {
		m_parent = g_systemClock; 
		m_parent->AddChild(this);
	}
}

Clock::Clock(Clock& parent)
	: m_parent(&parent)
{
	m_parent->AddChild(this);
}

Clock::~Clock()
{
	if (m_parent) {
		m_parent->RemoveChild(this);
	}

	for (Clock* child : m_children) {
		child->m_parent = nullptr;
	}
}

void Clock::Reset()
{
	m_lastUpdateTimeInSeconds = (float)GetCurrentTimeSeconds();
	m_totalSeconds = 0.0f;
	m_deltaSeconds = 0.0f;
	m_frameCount = 0;
	m_isPaused = false;
	m_stepSingleFrame = false;
}

bool Clock::IsPaused() const
{
	return m_isPaused;
}

void Clock::Pause()
{
	m_isPaused = true;
}

void Clock::Unpause()
{
	m_isPaused = false;
}

void Clock::TogglePause()
{
	m_isPaused = !m_isPaused;
}

void Clock::StepSingleFrame()
{
	m_stepSingleFrame = true;
	Unpause();
}

void Clock::SetTimeScale(float timeScale)
{
	m_timeScale = timeScale;
}

float Clock::GetTimeScale() const
{
	return m_timeScale;
}

float Clock::GetDeltaSeconds() const
{
	return m_deltaSeconds;
}

float Clock::GetTotalSeconds() const
{
	return m_totalSeconds;
}

size_t Clock::GetFrameCount() const
{
	return m_frameCount;
}

Clock& Clock::GetSystemClock()
{
	if (!g_systemClock) {
		static Clock systemClock;
		g_systemClock = &systemClock;
	}
	return *g_systemClock;
}

void Clock::TickSystemClock()
{
	if (g_systemClock) {
		g_systemClock->Tick();
	}
}

void Clock::Tick()
{
	if (m_parent == nullptr) { // System clock updates based on real time
		float currentTime = (float)GetCurrentTimeSeconds(); // Placeholder function
		m_deltaSeconds = currentTime - m_lastUpdateTimeInSeconds;
		m_lastUpdateTimeInSeconds = currentTime;
	}
	Advance(m_deltaSeconds);
}

void Clock::Advance(float deltaTimeSeconds)
{
	if (!m_isPaused) {
		m_deltaSeconds = deltaTimeSeconds * m_timeScale;
		m_totalSeconds += m_deltaSeconds;
		m_frameCount++;
	}
	else {
		m_deltaSeconds = 0;
	}

	// If stepping through a single frame, pause again after advancing
	if (m_stepSingleFrame) {
		Pause();
		m_stepSingleFrame = false; // Reset single-frame stepping
	}

	// Advance children clocks
	for (Clock* child : m_children) {
		child->Advance(m_deltaSeconds); // Pass scaled delta time to children
	}
}


void Clock::AddChild(Clock* childClock)
{
	m_children.push_back(childClock);
}

void Clock::RemoveChild(Clock* childClock)
{
	m_children.erase(std::remove(m_children.begin(), m_children.end(), childClock), m_children.end());
}
