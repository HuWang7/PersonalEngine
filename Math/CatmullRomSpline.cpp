#include "Engine/Math/CatmullRomSpline.hpp"
#include "Engine/Math/CubicHermiteCurve.hpp"

CatmullRomSpline::CatmullRomSpline(const std::vector<Vec2>& points)
	:m_positions(points)
{
	SetPositions(points);
}

void CatmullRomSpline::SetPositions(std::vector<Vec2> const& points)
{
	m_positions = points;
	m_velocities.resize(m_positions.size());
	if (m_positions.empty())
	{
		return;
	}
	m_velocities[0] = Vec2::ZERO;
	m_velocities.back() = Vec2::ZERO;

	for (int i = 1; i < (int)m_positions.size() - 1; ++i)
	{
		Vec2 prePos = m_positions[i - 1];
		Vec2 thisPos = m_positions[i + 0];
		Vec2 nextPos = m_positions[i + 1];
		Vec2 dispInto = thisPos - prePos;
		Vec2 disOutof = nextPos - thisPos;
		m_velocities[i] = (dispInto + disOutof) * 0.5f;
	}
}

Vec2 CatmullRomSpline::EvaluateAtParametric(float parametricZeroToNumCurveSections, int segmentIndex) const
{
	if (m_positions.size() < 2) return Vec2();

	float localT = parametricZeroToNumCurveSections - segmentIndex;

	Vec2 p0 = m_positions[segmentIndex];
	Vec2 p1 = m_positions[segmentIndex + 1];
	Vec2 v0 = m_velocities[segmentIndex];
	Vec2 v1 = m_velocities[segmentIndex + 1];

	return CubicHermiteCurve2D(p0, v0, p1, v1).EvaluateAtParametric(localT);
}

float CatmullRomSpline::GetApproximateLength(int numSubdivisions /*= 64*/) const
{
	if ((int)m_positions.size() < 2) return 0.0f;

	float totalLength = 0.0f;
	for (int i = 0; i < (int)m_positions.size() - 1; ++i) {
		Vec2 previousPoint = EvaluateAtParametric((float)i + 0.f, i);
		for (int j = 1; j <= numSubdivisions; ++j) {
			float t = static_cast<float>(j) / numSubdivisions;
			Vec2 currentPoint = EvaluateAtParametric(i + t, i);
			totalLength += (currentPoint - previousPoint).GetLength();
			previousPoint = currentPoint;
		}
	}
	return totalLength;
}

Vec2 CatmullRomSpline::EvaluateAtApproximateDistance(float distance, int numSubdivisions /*= 64*/) const
{
	if ((int)m_positions.size() < 2) return Vec2();

	float totalLength = 0.0f;
	Vec2 pointOnCurve;
	for (int i = 0; i < (int)m_positions.size() - 1 && totalLength < distance; ++i) {
		Vec2 previousPoint = EvaluateAtParametric((float)i + 0.f, i);
		for (int j = 1; j <= numSubdivisions; ++j) {
			float t = static_cast<float>(j) / numSubdivisions;
			Vec2 currentPoint = EvaluateAtParametric(i + t, i);
			float segmentLength = (currentPoint - previousPoint).GetLength();
			if (totalLength + segmentLength >= distance) {
				float ratio = (distance - totalLength) / segmentLength;
				pointOnCurve = previousPoint + (currentPoint - previousPoint) * ratio;
				return pointOnCurve;
			}
			totalLength += segmentLength;
			previousPoint = currentPoint;
		}
	}
	return m_positions.back();
}

//CatmullRomSpline::CatmullRomSpline(const std::vector<Vec2>& points)
//	:m_positions(points)
//{
//	CalculateVelocities(); 
//}
//
//void CatmullRomSpline::CalculateVelocities()
//{
//	m_velocities.resize(m_positions.size(), Vec2(0, 0));
//
//	for (int i = 1; i < (int)m_positions.size() - 1; ++i) {
//		m_velocities[i] = (m_positions[i + 1] - m_positions[i - 1]) / 2.0f;
//	}
//}
//
//Vec2 CatmullRomSpline::EvaluateAtParametric(float t) const
//{
//	if (m_positions.empty()) return Vec2();
//
//	int numSegments = static_cast<int>(m_positions.size()) - 1;
//	if (numSegments <= 0) return m_positions[0];
//
//	// Normalize t to be within the number of segments
//	t *= numSegments;
//	int segmentIndex = static_cast<int>(t);
//	if (segmentIndex >= numSegments) {
//		segmentIndex = numSegments - 1; // Clamp to the last segment
//		t = 1.0f; // Ensure we are at the end of the last segment
//	}
//	else {
//		t -= segmentIndex; // Normalize t for the current segment
//	}
//
//	// Calculate the indices for the Catmull-Rom spline calculation
//	int i0 = std::max(segmentIndex - 1, 0);
//	int i1 = segmentIndex;
//	int i2 = std::min(segmentIndex + 1, numSegments);
//	int i3 = std::min(segmentIndex + 2, numSegments);
//
//	// Calculate tangents for i1 and i2
//	Vec2 tangent1 = (m_positions[i2] - m_positions[i0]) / 2.0f;
//	Vec2 tangent2 = (m_positions[i3] - m_positions[i1]) / 2.0f;
//
//	// Calculate basis functions for the Hermite form of Catmull-Rom spline
//	float h00 = 2.0f * t * t * t - 3.0f * t * t + 1.0f;
//	float h10 = t * t * t - 2.0f * t * t + t;
//	float h01 = -2.0f * t * t * t + 3.0f * t * t;
//	float h11 = t * t * t - t * t;
//
//	// Compute the interpolated position
//	Vec2 position = h00 * m_positions[i1] + h10 * tangent1 + h01 * m_positions[i2] + h11 * tangent2;
//
//	return position;
//}
//
//float CatmullRomSpline::GetTotalLength(int numSubdivisions /*= 64*/) const
//{
//	float totalLength = 0.0f;
//	Vec2 prevPoint = EvaluateAtParametric(0.f);
//	for (int i = 1; i <= numSubdivisions; ++i) {
//		float t = static_cast<float>(i) / static_cast<float>(numSubdivisions);
//		Vec2 currentPoint = EvaluateAtParametric(t);
//		totalLength += (currentPoint - prevPoint).GetLength();
//		prevPoint = currentPoint;
//	}
//	return totalLength;
//}
//
//Vec2 CatmullRomSpline::GetPositionAtDistance(float distance, int numSubdivisions /*= 64*/) const {
//	float totalDistance = 0.0f;
//	Vec2 previousPoint = EvaluateAtParametric(0.0f);
//	for (int i = 1; i <= numSubdivisions; ++i) {
//		float t = static_cast<float>(i) / numSubdivisions;
//		Vec2 currentPoint = EvaluateAtParametric(t);
//		float segmentLength = (currentPoint - previousPoint).GetLength();
//		if (totalDistance + segmentLength >= distance) {
//			float ratio = (distance - totalDistance) / segmentLength;
//			return previousPoint + (currentPoint - previousPoint) * ratio;
//		}
//		totalDistance += segmentLength;
//		previousPoint = currentPoint;
//	}
//	return m_positions.back();
//}


