#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/MathUtils.hpp"

AABB2::AABB2(AABB2 const& copyFrom)
	: m_mins(copyFrom.m_mins),
	m_maxs(copyFrom.m_maxs) {}

AABB2::AABB2(float minX, float minY, float maxX, float maxY)
	: m_mins(minX, minY),
	m_maxs(maxX, maxY) {}

AABB2::AABB2(Vec2 const& mins, Vec2 const& maxs)
	: m_mins(mins),
	m_maxs(maxs) {}

const AABB2 AABB2::ZERO_TO_ONE = AABB2(Vec2(0.f, 0.f), Vec2(1.f, 1.f));

bool AABB2::IsPointInside(Vec2 const& point) const {
	return (point.x >= m_mins.x && point.x <= m_maxs.x) &&
		(point.y >= m_mins.y && point.y <= m_maxs.y);
}

Vec2 const AABB2::GetCenter() const {
	return (m_mins + m_maxs) * 0.5f;
}

Vec2 const AABB2::GetDimensions() const {
	return m_maxs - m_mins;
}

Vec2 const AABB2::GetNearestPoint(Vec2 const& referencePosition) const {
	Vec2 nearestPoint;
	nearestPoint.x = Clamp(referencePosition.x, m_mins.x, m_maxs.x);
	nearestPoint.y = Clamp(referencePosition.y, m_mins.y, m_maxs.y);
	return nearestPoint;
}

Vec2 const AABB2::GetPointAtUV(Vec2 const& uv) const {
	return Vec2(m_mins.x + uv.x * (m_maxs.x - m_mins.x),
		m_mins.y + uv.y * (m_maxs.y - m_mins.y));
}

Vec2 const AABB2::GetUVForPoint(Vec2 const& point) const {
	Vec2 dimensions = GetDimensions();
	return Vec2((point.x - m_mins.x) / dimensions.x,
		(point.y - m_mins.y) / dimensions.y);
}

void AABB2::Translate(Vec2 const& translationToApply) {
	m_mins += translationToApply;
	m_maxs += translationToApply;
}

void AABB2::SetCenter(Vec2 const& newCenter) {
	Vec2 dimensions = GetDimensions() * 0.5f;
	m_mins = newCenter - dimensions;
	m_maxs = newCenter + dimensions;
}

void AABB2::SetDimensions(Vec2 const& newDimensions) {
	Vec2 center = GetCenter();
	Vec2 halfDimensions = newDimensions * 0.5f;
	m_mins = center - halfDimensions;
	m_maxs = center + halfDimensions;
}

void AABB2::StretchToIncludePoint(Vec2 const& point) {
	if (point.x < m_mins.x) m_mins.x = point.x;
	if (point.x > m_maxs.x) m_maxs.x = point.x;
	if (point.y < m_mins.y) m_mins.y = point.y;
	if (point.y > m_maxs.y) m_maxs.y = point.y;
}

bool AABB2::operator==(const AABB2& compare) const
{
	return m_mins == compare.m_mins && m_maxs == compare.m_maxs;
}

