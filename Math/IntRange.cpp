#include "Engine/Math/IntRange.hpp"

const IntRange IntRange::ZERO = IntRange(0, 0);
const IntRange IntRange::ONE = IntRange(1, 1);
const IntRange IntRange::ZERO_TO_ONE = IntRange(0, 1);

IntRange::IntRange() 
	: m_min(0)
	, m_max(0) 
{

}

IntRange::IntRange(int min, int max) 
	: m_min(min)
	, m_max(max)
{

}

bool IntRange::operator==(const IntRange& other) const 
{
	return (m_min == other.m_min) && (m_max == other.m_max);
}

bool IntRange::operator!=(const IntRange& other) const
{
	return !(*this == other);
}

bool IntRange::IsOnRange(int value) const 
{
	return (value >= m_min) && (value <= m_max);
}

bool IntRange::IsOverlappingWith(const IntRange& other) const 
{
	return (m_min <= other.m_max) && (m_max >= other.m_min);
}
