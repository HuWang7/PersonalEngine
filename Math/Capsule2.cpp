#include "Engine/Math/Capsule2.hpp"

Capsule2::Capsule2(const Vec2& start, const Vec2& end, float radius)
	: m_start(start)
	, m_end(end)
	, m_radius(radius)
{

}

Capsule2::Capsule2(const LineSegment2& bone, float radius)
	: m_bone(bone)
	, m_radius(radius) 
{

}

void Capsule2::Translate(const Vec2& translation) 
{
	m_bone.Translate(translation);
}

void Capsule2::SetCenter(const Vec2& newCenter)
{
	m_bone.SetCenter(newCenter);
}

void Capsule2::RotateAboutCenter(float rotationDeltaDegrees) 
{
	m_bone.RotateAboutCenter(rotationDeltaDegrees);
}
