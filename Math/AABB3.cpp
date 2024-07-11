#include "Engine/Math/AABB3.hpp"

AABB3::AABB3(AABB3 const& copyFrom)
	: m_mins(copyFrom.m_mins)
	, m_maxs(copyFrom.m_maxs)
{

}

AABB3::AABB3(float minX, float minY, float minZ, float maxX, float maxY, float maxZ)
	:m_mins(minX, minY, minZ)
	,m_maxs(maxX, maxY, maxZ)
{

}

AABB3::AABB3(Vec3 const& mins, Vec3 const& maxs)
	: m_mins(mins)
	, m_maxs(maxs)
{

}

Vec3 AABB3::GetCenter() const
{
	return (m_mins + m_maxs) * .5f;
}

Vec3 AABB3::GetDimensions() const
{
	return m_maxs - m_mins;
}
