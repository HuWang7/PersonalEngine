#include "Engine/Math/CubicHermiteCurve.hpp"
#include "Engine/Math/CubicBezierCurve2D.hpp"

CubicHermiteCurve2D::CubicHermiteCurve2D(Vec2 startPos, Vec2 startVel, Vec2 endPos, Vec2 endVel)
	: m_startPos(startPos)
	, m_startVel(startVel)
	, m_endPos(endPos)
	, m_endVel(endVel)
{
}

CubicHermiteCurve2D::CubicHermiteCurve2D(CubicBezierCurve2D const& fromBezier)
{
	m_startPos = fromBezier.m_startPos;
	m_startVel = 3.f * (fromBezier.m_guidePos1 - fromBezier.m_startPos);
	m_endPos = fromBezier.m_endPos;
	m_endVel = 3.f * (fromBezier.m_endPos - fromBezier.m_guidePos2);
}

Vec2 CubicHermiteCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	CubicHermiteCurve2D local = CubicHermiteCurve2D(m_startPos, m_startVel, m_endPos, m_endVel);
	CubicBezierCurve2D bezier = CubicBezierCurve2D(local);
	return bezier.EvaluateAtParametric(parametricZeroToOne);
}

float CubicHermiteCurve2D::GetApproximateLength(int numSubdivisions /*= 64*/) const
{
	CubicHermiteCurve2D local = CubicHermiteCurve2D(m_startPos, m_startVel, m_endPos, m_endVel);
	CubicBezierCurve2D bezier = CubicBezierCurve2D(local);
	return bezier.GetApproximateLength(numSubdivisions);
}

Vec2 CubicHermiteCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions /*= 64*/) const
{
	CubicHermiteCurve2D local = CubicHermiteCurve2D(m_startPos, m_startVel, m_endPos, m_endVel);
	CubicBezierCurve2D bezier = CubicBezierCurve2D(local);
	return bezier.EvaluateAtApproximateDistance(distanceAlongCurve, numSubdivisions);
}

