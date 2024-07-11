#pragma once
#include <vector>
#include "Engine/Math/Vec2.hpp"

class CatmullRomSpline {
public:
	

	CatmullRomSpline() = default;
	CatmullRomSpline(const std::vector<Vec2>& points);
	void SetPositions( std::vector<Vec2> const& points);
	Vec2 EvaluateAtParametric(float parametricZeroToNumCurveSections, int segmentIndex) const;
	float GetApproximateLength(int numSubdivisions = 64) const;
	Vec2 EvaluateAtApproximateDistance(float distanceAlongCurve, int numSubdivisions = 64) const;
	
public:
	std::vector<Vec2> m_positions;
	std::vector<Vec2> m_velocities;
};


/*void CalculateVelocities();
Vec2 EvaluateAtParametric(float t) const;
float GetTotalLength(int numSubdivisions = 64) const;
Vec2 GetPositionAtDistance(float distance, int numSubdivisions = 64) const;*/