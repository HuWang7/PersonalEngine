#include "Engine/Math/LineSegment2.hpp"
#include <cmath>

LineSegment2::LineSegment2(const Vec2& start, const Vec2& end)
	: m_start(start)
	, m_end(end)
{

}

void LineSegment2::Translate(const Vec2& translation)
{
	m_start += translation;
	m_end += translation;
}

void LineSegment2::SetCenter(const Vec2& newCenter) 
{
	Vec2 oldCenter = (m_start + m_end) * 0.5f;
	Vec2 translation = newCenter - oldCenter;
	Translate(translation);
}

void LineSegment2::RotateAboutCenter(float rotationDeltaDegrees)
{
	float radians = rotationDeltaDegrees * (3.14159265358979323846f / 180.0f);
	Vec2 center = (m_start + m_end) * 0.5f;

	Vec2 startRelativeToCenter = m_start - center;
	Vec2 endRelativeToCenter = m_end - center;

	float cosTheta = cos(radians);
	float sinTheta = sin(radians);

	m_start.x = cosTheta * startRelativeToCenter.x - sinTheta * startRelativeToCenter.y + center.x;
	m_start.y = sinTheta * startRelativeToCenter.x + cosTheta * startRelativeToCenter.y + center.y;

	m_end.x = cosTheta * endRelativeToCenter.x - sinTheta * endRelativeToCenter.y + center.x;
	m_end.y = sinTheta * endRelativeToCenter.x + cosTheta * endRelativeToCenter.y + center.y;

}
