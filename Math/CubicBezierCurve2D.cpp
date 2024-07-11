#include "Engine/Math/CubicBezierCurve2D.hpp"

CubicBezierCurve2D::CubicBezierCurve2D(Vec2 startPos, Vec2 guidePos1, Vec2 guidePos2, Vec2 endPos)
	:m_startPos(startPos)
	, m_guidePos1(guidePos1)
	, m_guidePos2(guidePos2)
	, m_endPos(endPos)
{

}

CubicBezierCurve2D::CubicBezierCurve2D(CubicHermiteCurve2D const& fromHermite)
	:m_startPos(fromHermite.m_startPos)
	, m_endPos(fromHermite.m_endPos)
{
	m_guidePos1 = fromHermite.m_startPos + fromHermite.m_startVel / 3.0f;
	m_guidePos2 = fromHermite.m_endPos - fromHermite.m_endVel / 3.0f;
}

Vec2 CubicBezierCurve2D::EvaluateAtParametric(float parametricZeroToOne) const
{
	Vec2 AB, BC, CD, ABC, BCD, ABCD;
	AB = Interpolate(m_startPos, m_guidePos1, parametricZeroToOne);
	BC = Interpolate(m_guidePos1, m_guidePos2, parametricZeroToOne);
	CD = Interpolate(m_guidePos2, m_endPos, parametricZeroToOne);

	ABC = Interpolate(AB, BC, parametricZeroToOne);
	BCD = Interpolate(BC, CD, parametricZeroToOne);

	ABCD = Interpolate(ABC, BCD, parametricZeroToOne);
	return ABCD;
}

float CubicBezierCurve2D::GetApproximateLength(int numSubdivisions /*= 64*/) const
{
	float tperStep = 1.f / float(numSubdivisions);
	float curveLength = 0.0f;
	Vec2 endPos;
	Vec2 startPos = m_startPos;
	for (int i = 1; i <= numSubdivisions; i++)
	{
		endPos = EvaluateAtParametric(tperStep * float(i));
		curveLength += GetDistance2D(endPos, startPos);
		startPos = endPos;
	}
	return curveLength;
}

Vec2 CubicBezierCurve2D::EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions /*= 64*/) const
{
	float tperStep = 1.f / float(numSubdivisions);
	Vec2 startPos = m_startPos;
	Vec2 endPos = m_startPos;
	float distanceTravelled = 0.0f;
	float subdivLength = 0.0f;
	for (int i = 1; i <= numSubdivisions; i++)
	{
		endPos = EvaluateAtParametric(tperStep * (float)i);
		subdivLength = GetDistance2D(endPos, startPos);
		float lengthFraction = (distanceAlongCurve - distanceTravelled) / subdivLength;
		if (distanceTravelled + subdivLength > distanceAlongCurve)
		{
			return Interpolate(startPos, endPos, lengthFraction);
		}
		startPos = endPos;
		distanceTravelled += subdivLength;
	}
	return Vec2();
}

