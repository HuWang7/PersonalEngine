#include "Engine/Input/AnalogJoystick.hpp"

Vec2 AnalogJoystick::GetPosition() const
{
	return m_correctedPosition;
}

float AnalogJoystick::GetMagnitude() const
{
	return m_correctedPosition.GetLength();
}

float AnalogJoystick::GetOrientationDegrees() const
{
	return m_correctedPosition.GetOrientationDegrees();
}

Vec2 AnalogJoystick::GetRawUncorrectedPosition() const
{
	return m_rawPosition;
}

float AnalogJoystick::GetInnerDeadZoneFraction() const
{
	return m_innerDeadZoneFraction;
}

float AnalogJoystick::GetOuterDeadZoneFraction() const
{
	return m_outerDeadZoneFraction;
}

void AnalogJoystick::Reset()
{
	m_rawPosition = Vec2(0.0f, 0.0f);
	m_correctedPosition = Vec2(0.0f, 0.0f);
}

void AnalogJoystick::SetDeadZoneThresholds(float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold)
{
	m_innerDeadZoneFraction = normalizedInnerDeadzoneThreshold;
	m_outerDeadZoneFraction = normalizedOuterDeadzoneThreshold;
}

void AnalogJoystick::updatePosition(float rawNormalizedX, float rawNormalizedY)
{
	m_rawPosition = Vec2(rawNormalizedX, rawNormalizedY);
	float rawMagnitude = m_rawPosition.GetLength();

	if (rawMagnitude < m_innerDeadZoneFraction)
	{
		m_correctedPosition = Vec2(0.f, 0.f);
	}
	else if (rawMagnitude > m_outerDeadZoneFraction)
	{
		m_correctedPosition = m_rawPosition.GetNormalized();
	}
	else
	{
		float scale = (rawMagnitude - m_innerDeadZoneFraction) / (m_outerDeadZoneFraction - m_innerDeadZoneFraction);
		m_correctedPosition = m_rawPosition.GetNormalized() * scale;
	}
}

