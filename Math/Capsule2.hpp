#pragma once
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/Vec2.hpp"

struct Capsule2 {
public:
	// Constructors
	Capsule2() = default;
	Capsule2(const Vec2& start, const Vec2& end, float radius);
	Capsule2(const LineSegment2& bone, float radius);

	// Methods
	void Translate(const Vec2& translation);
	void SetCenter(const Vec2& newCenter);
	void RotateAboutCenter(float rotationDeltaDegrees);

	// Member Variables
	LineSegment2 m_bone;
	Vec2 m_start;
	Vec2 m_end;
	float m_radius = 0.f;
	float m_elasticity;
};
