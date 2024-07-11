#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/MathUtils.hpp" 

OBB2::OBB2() 
	: m_center(0.f, 0.f)
	, m_iBasisNormal(1.f, 0.f)
	, m_halfDimensions(0.f, 0.f)
{

}

OBB2::OBB2(const Vec2& center, const Vec2& iBasisNormal, const Vec2& halfDimensions)
	: m_center(center)
	, m_iBasisNormal(iBasisNormal.GetNormalized())
	, m_halfDimensions(halfDimensions)
{

}

void OBB2::GetCornerPoints(Vec2* out_fourCornerWorldPositions) const 
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();
	Vec2 halfIBasis = m_iBasisNormal * m_halfDimensions.x;
	Vec2 halfJBasis = jBasisNormal * m_halfDimensions.y;

	out_fourCornerWorldPositions[0] = m_center - halfIBasis - halfJBasis; 
	out_fourCornerWorldPositions[1] = m_center + halfIBasis - halfJBasis; 
	out_fourCornerWorldPositions[2] = m_center + halfIBasis + halfJBasis; 
	out_fourCornerWorldPositions[3] = m_center - halfIBasis + halfJBasis; 
}

Vec2 OBB2::GetLocalPosForWorldPos(const Vec2& worldPos) const 
{
	Vec2 dispFromCenter = worldPos - m_center;
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();
	float x = DotProduct2D(dispFromCenter, m_iBasisNormal);
	float y = DotProduct2D(dispFromCenter, jBasisNormal);
	return Vec2(x, y);
}

Vec2 OBB2::GetWorldPosForLocalPos(const Vec2& localPos) const 
{
	Vec2 jBasisNormal = m_iBasisNormal.GetRotated90Degrees();
	return m_center + (m_iBasisNormal * localPos.x) + (jBasisNormal * localPos.y);
}

void OBB2::RotateAboutCenter(float rotationDeltaDegrees) 
{
	m_iBasisNormal.RotateDegrees(rotationDeltaDegrees);
}
