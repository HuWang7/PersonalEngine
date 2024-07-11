#pragma once

class FloatRange {
public:
	float m_min;
	float m_max;

	FloatRange(); 
	explicit FloatRange(float min, float max);

												
	bool operator==(const FloatRange& other) const;
	bool operator!=(const FloatRange& other) const;
	bool IsOnRange(float value) const;
	bool IsOverlappingWith(const FloatRange& other) const;

	static const FloatRange ZERO;
	static const FloatRange ONE;
	static const FloatRange ZERO_TO_ONE;
};
