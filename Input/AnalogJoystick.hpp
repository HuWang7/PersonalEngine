#pragma once
#include "Engine/Math/Vec2.hpp"

class AnalogJoystick
{

public:
	Vec2 GetPosition() const;
	float GetMagnitude() const;
	float GetOrientationDegrees() const;

	Vec2 GetRawUncorrectedPosition() const;
	float GetInnerDeadZoneFraction() const;
	float GetOuterDeadZoneFraction() const;

	//For use by xboxConter, et al.
	void Reset();
	void SetDeadZoneThresholds( float normalizedInnerDeadzoneThreshold, float normalizedOuterDeadzoneThreshold );
	void updatePosition( float rawNormalizedX, float rawNormalizedY );

protected:
	Vec2      m_rawPosition; // Flaky doesn't rest at zero (or consistently snap to rest position)
	Vec2      m_correctedPosition; // Deadzone-corrected position
	float     m_innerDeadZoneFraction     = 0.30f; // if R < this%, R = 0; "input range Start" for corrective range map
	float     m_outerDeadZoneFraction     = 0.95f; // if R > this%, R = 1; "input range end" for corrective range map
};