#pragma once
#include "Engine/Math/Vec2.hpp"

struct OBB2 {
public:
	// Constructors
	OBB2();
	OBB2(const Vec2& center, const Vec2& iBasisNormal, const Vec2& halfDimensions);

	// Methods
	void GetCornerPoints(Vec2* out_fourCornerWorldPositions) const;
	Vec2 GetLocalPosForWorldPos(const Vec2& worldPos) const;
	Vec2 GetWorldPosForLocalPos(const Vec2& localPos) const;
	void RotateAboutCenter(float rotationDeltaDegrees);

	// Member Variables
	Vec2 m_center;
	Vec2 m_iBasisNormal;
	Vec2 m_halfDimensions;
	float m_elasticity;
};

