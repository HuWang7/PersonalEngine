#include "MathUtils.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/Capsule2.hpp"
#include "Engine/Math//LineSegment2.hpp"
#include "Engine/Math/OBB2.hpp"
#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/Plane3.hpp"
#include <cmath>

constexpr float PI = 3.14159265358979323846f;

Mat44 GetLookAtMatrix(const Vec3& forward, const Vec3& billboardPosition) 
{
	Vec3 forwardNormal = forward.GetNormalized();

	Vec3 arbitraryVector = (fabs(forwardNormal.x) < 0.99f) ? Vec3(1, 0, 0) : Vec3(0, 1, 0);
	Vec3 leftNormal = CrossProduct3D(arbitraryVector, forwardNormal).GetNormalized();
	Vec3 upNormal = CrossProduct3D(forwardNormal, leftNormal);

	Mat44 lookAtMatrix;
	lookAtMatrix.SetIJKT3D(
		forwardNormal,
		leftNormal,
		upNormal,
		billboardPosition
	);

	return lookAtMatrix;
}

Mat44 GetBillboardMatrix(BillboardType billboardType, Mat44 const& cameraMatrix, const Vec3& billboardPosition, const Vec2& billboardScale /*= Vec2(1.0f, 1.0f)*/)
{
	Vec3 billboardIBasis = Vec3(1.f, 0.f, 0.f);
	Vec3 billboardJBasis = Vec3(0.f, 1.f, 0.f);
	Vec3 billboardKBasis = Vec3(0.f, 0.f, 1.f);
	Vec3 billboardTranslation = billboardPosition;

	Vec3 cameraIBasis = cameraMatrix.GetIBasis3D();
	Vec3 cameraJBasis = cameraMatrix.GetJBasis3D();
	Vec3 cameraKBasis = cameraMatrix.GetKBasis3D();
	Vec3 cameraPosition = cameraMatrix.GetTranslation3D();

	Vec3 Zaxis = Vec3(0.f, 0.f, 1.f);
	Vec3 Yaxis = Vec3(0.f, 1.f, 0.f);

	switch (billboardType)
	{
	case BillboardType::NONE:
		break;
	case BillboardType::WORLD_UP_CAMERA_FACING:
		billboardIBasis = -1.f * cameraIBasis;
		billboardJBasis = -1.f * cameraJBasis;
		billboardKBasis = Zaxis;
		break;
	case BillboardType::WORLD_UP_CAMERA_OPPOSING:
		billboardKBasis = Zaxis;
		billboardIBasis = -1.f * cameraIBasis;
		billboardIBasis.z = 0.f;
		billboardIBasis = billboardIBasis.GetNormalized();
		// 		billboardJBasis = CrossProduct3D(billboardKBasis, billboardIBasis); 
		// 		billboardJBasis = billboardJBasis.GetNormalized();
		billboardJBasis = -1.f * cameraJBasis;
		break;
	case BillboardType::FULL_CAMERA_FACING:
		billboardIBasis = billboardTranslation - cameraPosition;
		billboardIBasis = billboardIBasis.GetNormalized();
		if (abs(DotProduct3D(billboardIBasis, Zaxis) < 1.f))
		{
			billboardJBasis = CrossProduct3D(Zaxis, billboardIBasis);
			billboardJBasis = billboardJBasis.GetNormalized();
			billboardKBasis = CrossProduct3D(billboardIBasis, billboardJBasis);
			billboardKBasis = billboardKBasis.GetNormalized();
		}
		else
		{
			billboardKBasis = CrossProduct3D(billboardIBasis, Yaxis);
			billboardKBasis = billboardKBasis.GetNormalized();
			billboardJBasis = CrossProduct3D(billboardKBasis, billboardIBasis);
			billboardJBasis = billboardJBasis.GetNormalized();
		}
		break;
	case BillboardType::FULL_CAMERA_OPPOSING:
		billboardIBasis = -1.f * cameraIBasis;
		billboardJBasis = -1.f * cameraJBasis;
		billboardKBasis = cameraKBasis;
		break;
	case BillboardType::COUNT:
		break;
	default:
		break;
	}
	Mat44 result = Mat44(billboardIBasis, billboardJBasis, billboardKBasis, billboardTranslation);
	result.AppendScaleNonUniform2D(billboardScale);
	return result;
}

RaycastResult2D RaycastVsDisc2D(Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius)
{
	RaycastResult2D result;
	Vec2 vtclNormal = fwdNormal.GetRotated90Degrees();
	Vec2 startToCenterPos = discCenter - startPos;
	float startToCenterVtcl = DotProduct2D(startToCenterPos, vtclNormal);
	if (startToCenterVtcl >= discRadius)
	{
		return result;
	}
	if (startToCenterVtcl <= -discRadius)
	{
		return result;
	}
	//#ToDo: Check if Disc is too early or too late
	float a = std::sqrtf((discRadius*discRadius) - (startToCenterVtcl*startToCenterVtcl));
	float startToCenterFwd = DotProduct2D(startToCenterPos, fwdNormal);
	if (startToCenterFwd >= maxDist + discRadius)
	{
		return result;
	}
	if (startToCenterFwd <= -discRadius)
	{
		return result;
	}
	result.m_impactDist = startToCenterFwd - a;
	result.m_impactPos = startPos + (fwdNormal * result.m_impactDist);
	result.m_impactNormal = (startPos + (result.m_impactDist * fwdNormal) - discCenter).GetNormalized();
	if (result.m_impactDist >= maxDist)
	{
		return result;
	}
	if (result.m_impactDist <= 0 || result.m_impactDist <= -2*a)
	{
		return result;
	}
	result.m_dicsCenter = discCenter;
	result.m_didImpact = true;

	return result;
}

RaycastResult2D RaycastVsLineSegment2D(Vec2 rayStart, Vec2 rayFwdNormal, float rayMaxDist, Vec2 LineSegStart, Vec2 LineSegEnd)
{
	RaycastResult2D result;
	Vec2 rayStartToLineStart = LineSegStart - rayStart; // SP->
	Vec2 rayStartToLineEnd = LineSegEnd - rayStart;// SQ->
	Vec2 rayVtclNormal = rayFwdNormal.GetRotated90Degrees();
	float rayStartToLineStart_J = DotProduct2D(rayVtclNormal, rayStartToLineStart);// SPj
	float rayStartToLineEnd_J = DotProduct2D(rayVtclNormal, rayStartToLineEnd);// SQj
	if (rayStartToLineStart_J * rayStartToLineEnd_J >= 0)
	{
		return result;
	}
	float t = rayStartToLineStart_J / (rayStartToLineStart_J - rayStartToLineEnd_J);
	Vec2 intersectionPoint = LineSegStart + t * (LineSegEnd - LineSegStart); // I
	Vec2 rayStartToIntersectionPoint = intersectionPoint - rayStart;
	float impactDist = DotProduct2D(rayStartToIntersectionPoint, rayFwdNormal);
	if (impactDist >= rayMaxDist)
	{
		return result;// too far!
	}
	if (impactDist <= 0)
	{
		return result; // too early
	}
	Vec2 LineSegStartToLineSegEnd = LineSegEnd - LineSegStart;// PQ->
	Vec2 NormalLine = LineSegStartToLineSegEnd.GetRotated90Degrees();// N->
	if (rayStartToLineEnd_J < 0)
	{
		NormalLine *= -1.f;
	}
	NormalLine.Normalize();
	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactPos = intersectionPoint;
	result.m_impactNormal = NormalLine;
	result.m_line = LineSegment2(LineSegStart, LineSegEnd);
	return result;
}

RaycastResult2D RaycastVsAABB2D(Vec2 rayStart, Vec2 rayEnd, Vec2 rayFwdNormal, float rayMaxDist, AABB2 bound)
{

	RaycastResult2D result;
	AABB2 rayBound(Vec2(std::min(rayStart.x, rayEnd.x), std::min(rayStart.y, rayEnd.y)), Vec2(std::max(rayStart.x, rayEnd.x), std::max(rayStart.y, rayEnd.y)));
	
	if (rayBound.m_mins.x > bound.m_maxs.x || rayBound.m_maxs.x < bound.m_mins.x || rayBound.m_mins.y > bound.m_maxs.y || rayBound.m_maxs.y < bound.m_mins.y)
	{
		return result;
	}

	if (IsPointInsideAABB2D(rayStart, bound)) {
		result.m_didImpact = true;
		result.m_impactDist = 0.0;
		result.m_impactPos = rayStart;
		result.m_AABB2 = bound;
		return result;
	}
	else
	{
		Vec2 rayDir = rayEnd - rayStart;
		Vec2 invDir = Vec2(1.0f / rayDir.x, 1.0f / rayDir.y);

		Vec2 tMin = (bound.m_mins - rayStart) * invDir;
		Vec2 tMax = (bound.m_maxs - rayStart) * invDir;

		if (rayDir.x < 0.0f) std::swap(tMin.x, tMax.x);
		if (rayDir.y < 0.0f) std::swap(tMin.y, tMax.y);

		float tEntry = std::max(tMin.x, tMin.y);
		float tExit = std::min(tMax.x, tMax.y);

		if (tEntry > tExit || tEntry > rayMaxDist) {
			return result;
		}

		Vec2 intersectionPoint = rayStart + rayDir * tEntry;
		float impactDist = (intersectionPoint - rayStart).GetLength();

		Vec2 impactNormal(0, 0);
		if (tEntry == tMin.x) {
			impactNormal = invDir.x < 0 ? Vec2(1, 0) : Vec2(-1, 0);
		}
		else if (tEntry == tMin.y) {
			impactNormal = invDir.y < 0 ? Vec2(0, 1) : Vec2(0, -1);
		}

		result.m_didImpact = true;
		result.m_impactDist = impactDist;
		result.m_impactPos = intersectionPoint;
		result.m_impactNormal = impactNormal;
		result.m_AABB2 = bound;

		return result;
	}
}

RaycastResult3D RaycastVsAABB3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, AABB3 box)
{
	RaycastResult3D result;

	Vec3 rayEnd = rayStart + rayForwardNormal * rayLength;
	AABB3 rayAABB3(std::min(rayStart.x, rayEnd.x), std::min(rayStart.y, rayEnd.y), std::min(rayStart.z, rayEnd.z),
		std::max(rayStart.x, rayEnd.x), std::max(rayStart.y, rayEnd.y), std::max(rayStart.z, rayEnd.z));
	if (!DoAABBsOverlap3D(box, rayAABB3))
	{
		return result;
	}
	float rayMaxz = std::max(rayStart.z, rayEnd.z);
	float rayMinz = std::min(rayStart.z, rayEnd.z);
	if (rayMaxz < box.m_mins.z || rayMinz > box.m_maxs.z)
	{
		return result;
	}
	float tMin = std::numeric_limits<float>::lowest();
	float tMax = std::numeric_limits<float>::max();
	Vec3 normal = Vec3(0, 0, 0); // Initialize normal

	for (int i = 0; i < 3; ++i) {
		float slabMin = (i == 0) ? box.m_mins.x : ((i == 1) ? box.m_mins.y : box.m_mins.z);
		float slabMax = (i == 0) ? box.m_maxs.x : ((i == 1) ? box.m_maxs.y : box.m_maxs.z);
		float rayOriginComponent = (i == 0) ? rayStart.x : ((i == 1) ? rayStart.y : rayStart.z);
		float rayDirectionComponent = (i == 0) ? rayForwardNormal.x : ((i == 1) ? rayForwardNormal.y : rayForwardNormal.z);

		if (fabs(rayDirectionComponent) < std::numeric_limits<float>::epsilon()) {
			if (rayOriginComponent < slabMin || rayOriginComponent > slabMax) return result;
		}
		else {
			float t1 = (slabMin - rayOriginComponent) / rayDirectionComponent;
			float t2 = (slabMax - rayOriginComponent) / rayDirectionComponent;

			if (t1 > t2) std::swap(t1, t2);
			if (t1 > tMin) {
				tMin = t1;
				// Reset normal and set the appropriate component based on i
				normal = Vec3(0, 0, 0);
				if (i == 0) normal.x = rayDirectionComponent < 0.f ? 1.f : -1.f;
				if (i == 1) normal.y = rayDirectionComponent < 0.f ? 1.f : -1.f;
				if (i == 2) normal.z = rayDirectionComponent < 0.f ? 1.f : -1.f;
			}
			tMax = std::min(tMax, t2);
			if (tMin > tMax) return result;
		}
	}

	if (tMin < 0 || tMin > rayLength) return result; // Check if the intersection is within the ray segment

	result.m_didImpact = true;
	result.m_impactDist = tMin;
	result.m_impactPos = rayStart + rayForwardNormal * tMin;
	result.m_impactNormal = normal;
	result.m_shape = Shape::BOX;
	result.m_box = box;
	return result;
}

RaycastResult3D RaycastVsOBB3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, OBB3 obb)
{
	RaycastResult3D result;

	Vec3 localRayStart = rayStart - obb.center;
	Vec3 iBasis = obb.iBasis.GetNormalized();
	Vec3 jBasis = obb.jBasis.GetNormalized();
	Vec3 kBasis = CrossProduct3D(iBasis, jBasis);

	Vec3 localStart(
		DotProduct3D(localRayStart, iBasis),
		DotProduct3D(localRayStart, jBasis),
		DotProduct3D(localRayStart, kBasis)
	);

	Vec3 localDirection(
		DotProduct3D(rayForwardNormal, iBasis),
		DotProduct3D(rayForwardNormal, jBasis),
		DotProduct3D(rayForwardNormal, kBasis)
	);

	AABB3 localAABB3(-1.f * obb.halfDimensions, obb.halfDimensions);
	RaycastResult3D localResult = RaycastVsAABB3D(localStart, localDirection, rayLength, localAABB3);

	if (!localResult.m_didImpact) {
		return result;
	}

	result.m_didImpact = true;
	result.m_impactDist = localResult.m_impactDist;
	result.m_impactPos = obb.center + localResult.m_impactPos.x * iBasis
		+ localResult.m_impactPos.y * jBasis
		+ localResult.m_impactPos.z * kBasis;
	result.m_impactNormal = localResult.m_impactNormal.x * iBasis
		+ localResult.m_impactNormal.y * jBasis
		+ localResult.m_impactNormal.z * kBasis;
	result.m_shape = Shape::OBB;
	result.m_obbCenter = obb.center;

	return result;
}

RaycastResult3D RaycastVsSphere3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Vec3 sphereCenter, float sphereRadius)
{
	RaycastResult3D result;

	Vec2 startToCenter = Vec2(sphereCenter.x, sphereCenter.y) - Vec2(rayStart.x, rayStart.y); // SC->
	float startToCenterDistance = DotProduct2D(startToCenter, Vec2(rayForwardNormal.x, rayForwardNormal.y)); // SCi
	if (startToCenterDistance >= rayLength + sphereRadius)
	{
		return result;
	}
	Vec2 startToCenter_i = startToCenterDistance * Vec2(rayForwardNormal.x, rayForwardNormal.y); // SCi->
	Vec2 startToCenter_jk = startToCenter - startToCenter_i; //SCjk->
	if (startToCenterDistance <= 0)
	{
		return result;
	}
	float startToCenter_jkLength = startToCenter_jk.GetLength() * startToCenter_jk.GetLength();
	if (startToCenter_jkLength >= sphereRadius * sphereRadius)
	{
		return result;
	}
	if (startToCenterDistance < sphereRadius)
	{
		result.m_didImpact = true;
		result.m_impactDist = 0.f;
		result.m_impactNormal = (rayStart - sphereCenter).GetNormalized();
		result.m_impactPos = rayStart;
		result.m_shape = Shape::SPHERE;
		result.m_sphereCenter = sphereCenter;
		return result;
	}
	float a = std::sqrtf(sphereRadius * sphereRadius - startToCenter_jkLength);
	float impactDist = startToCenterDistance - a;
	if (impactDist >= rayLength)
	{
		return result;
	}
	Vec3 impactPos = rayStart + impactDist * rayForwardNormal;
	Vec3 impactNormal = (impactPos - sphereCenter).GetNormalized();
	result.m_didImpact = true;
	result.m_impactDist = impactDist;
	result.m_impactNormal = impactNormal;
	result.m_impactPos = impactPos;
	result.m_shape = Shape::SPHERE;
	result.m_sphereCenter = sphereCenter;
	return result;
}

RaycastResult3D RaycastVsCylinderZ3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Vec2 const& centerXY, FloatRange const& minMaxZ, float radiusXY)
{
	RaycastResult3D result;

	Vec3 rayEnd = rayStart + rayForwardNormal * rayLength;
	AABB3 cylinderAABB3(Vec3(centerXY.x - radiusXY, centerXY.y - radiusXY, minMaxZ.m_min), Vec3(centerXY.x + radiusXY, centerXY.y + radiusXY, minMaxZ.m_max));
	AABB3 rayAABB3(std::min(rayStart.x, rayEnd.x), std::min(rayStart.y, rayEnd.y), std::min(rayStart.z, rayEnd.z),
		std::max(rayStart.x, rayEnd.x), std::max(rayStart.y, rayEnd.y), std::max(rayStart.z, rayEnd.z));
	if (!DoAABBsOverlap3D(cylinderAABB3, rayAABB3))
	{
		return result;
	}
	//Check if ray hit disc in xy space
	Vec2 rayStartXY(rayStart.x, rayStart.y);
	Vec2 rayDirectionXY(rayForwardNormal.x, rayForwardNormal.y);
	float rayLengthXY = rayDirectionXY.GetLength() * rayLength;
	RaycastResult2D result2D = RaycastVsDisc2D(rayStartXY, rayDirectionXY, rayLengthXY, centerXY, radiusXY);
	if (!result2D.m_didImpact)
	{
		return result;
	}

	float zImpact = rayStart.z + rayForwardNormal.z * result2D.m_impactDist;
	if (zImpact >= minMaxZ.m_min && zImpact <= minMaxZ.m_max) {
		result.m_didImpact = true;
		result.m_impactDist = result2D.m_impactDist;
		result.m_impactPos = rayStart + rayForwardNormal * result.m_impactDist;
		result.m_impactNormal = (result.m_impactPos - Vec3(centerXY.x, centerXY.y, result.m_impactPos.z)).GetNormalized();
		result.m_shape = Shape::CYLINDER;
		result.m_cylinderBaseCenter = Vec3(centerXY.x, centerXY.y, minMaxZ.m_min);
		return result;
	}

	if (rayStart.z < minMaxZ.m_min || rayStart.z > minMaxZ.m_max || rayEnd.z < minMaxZ.m_min || rayEnd.z > minMaxZ.m_max) {
		float tImpactTop = (minMaxZ.m_max - rayStart.z) / rayForwardNormal.z; 
		Vec3 impactPosTop = rayStart + rayForwardNormal * tImpactTop; 
		Vec2 impactPosXYTop(impactPosTop.x, impactPosTop.y);
		if (IsPointInsideDisc2D(impactPosXYTop, centerXY, radiusXY)) {
			result.m_didImpact = true;
			result.m_impactDist = tImpactTop;
			result.m_impactPos = impactPosTop;
			result.m_impactNormal = Vec3(0, 0, 1); 
			result.m_shape = Shape::CYLINDER;
			result.m_cylinderBaseCenter = Vec3(centerXY.x, centerXY.y, minMaxZ.m_min);
			return result;
		}
	}

	float tImpactBottom = (minMaxZ.m_min - rayStart.z) / rayForwardNormal.z; 
	Vec3 impactPosBottom = rayStart + rayForwardNormal * tImpactBottom;
	Vec2 impactPosXYBottom(impactPosBottom.x, impactPosBottom.y);
	if (IsPointInsideDisc2D(impactPosXYBottom, centerXY, radiusXY)) {
		result.m_didImpact = true;
		result.m_impactDist = tImpactBottom;
		result.m_impactPos = impactPosBottom;
		result.m_impactNormal = Vec3(0, 0, -1); 
		result.m_shape = Shape::CYLINDER;
		result.m_cylinderBaseCenter = Vec3(centerXY.x, centerXY.y, minMaxZ.m_min);
		return result;
	}

	return result;
}

RaycastResult3D RaycastVsPlane3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Plane3D plane)
{
	RaycastResult3D result;
	float rayDirDotPlaneNormal = DotProduct3D(rayForwardNormal, plane.normal);
	if (fabs(rayDirDotPlaneNormal) < std::numeric_limits<float>::epsilon()) {
		return result; 
	}
	float distanceFromRayStartToPlane = (plane.distanceFromOriginAlongNormal - DotProduct3D(rayStart, plane.normal)) / rayDirDotPlaneNormal;
	if (distanceFromRayStartToPlane < 0.f || distanceFromRayStartToPlane > rayLength) {
		return result; 
	}
	result.m_didImpact = true;
	result.m_impactDist = distanceFromRayStartToPlane;
	result.m_impactPos = rayStart + rayForwardNormal * distanceFromRayStartToPlane;

	if (DotProduct3D(rayForwardNormal, plane.normal) > 0) {
		result.m_impactNormal = -1.f * plane.normal;
	}
	else {
		result.m_impactNormal = plane.normal;
	}

	result.m_shape = Shape::PLANE;
	return result;
}

// Clamp and Lerp Functions
float Clamp(float value, float minValue, float maxValue) {
	if (value < minValue) return minValue;
	if (value > maxValue) return maxValue;
	return value;
}

int Clamp(int value, int minValue, int maxValue) {
	if (value < minValue) return minValue;
	if (value > maxValue) return maxValue;
	return value;
}

float ClampZeroToOne(float value) {
	return Clamp(value, 0.0f, 1.0f);
}

float Interpolate(float start, float end, float fractionTowardEnd) {
	return start + fractionTowardEnd * (end - start);
}

Vec2 Interpolate(Vec2 pointA, Vec2 pointB, float fraction)
{
	Vec2 returnValue;
	returnValue.x = pointA.x + ((pointB.x - pointA.x) * fraction);
	returnValue.y = pointA.y + ((pointB.y - pointA.y) * fraction);
	return returnValue;
}

float GetFractionWithinRange(float value, float rangeStart, float rangeEnd) {
	if (rangeEnd - rangeStart == 0) return 0; // Avoid division by zero
	return (value - rangeStart) / (rangeEnd - rangeStart);
}

float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd) {
	float fraction = GetFractionWithinRange(inValue, inStart, inEnd);
	return Interpolate(outStart, outEnd, fraction);
}

float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd) {
	float fraction = ClampZeroToOne(GetFractionWithinRange(inValue, inStart, inEnd));
	return Interpolate(outStart, outEnd, fraction);
}

int RoundDownToInt(float value) {
	return static_cast<int>(std::floor(value));
}

// Angle utilities
float ConvertDegreesToRadians(float degrees) {
	return degrees * (3.14159265358979323846f / 180.0f);
}

float ConvertRadiansToDegrees(float radians) {
	return radians * (180.0f / 3.14159265358979323846f);
}

float CosDegrees(float degrees) {
	return std::cos(ConvertDegreesToRadians(degrees));
}

float SinDegrees(float degrees) {
	return std::sin(ConvertDegreesToRadians(degrees));
}

float Atan2Degrees(float y, float x) {
	return ConvertRadiansToDegrees(std::atan2(y, x));
}

float GetShortestAngularDispDegrees(float startDegrees, float endDegrees) {
	float delta = endDegrees - startDegrees;
	while (delta > 180.0f) delta -= 360.0f;
	while (delta < -180.0f) delta += 360.0f;
	return delta;
}

float GetTurnedTowardDegrees(float currentDegrees, float goalDegrees, float maxDeltaDegrees) {
	float deltaDegrees = GetShortestAngularDispDegrees(currentDegrees, goalDegrees);
	if (std::abs(deltaDegrees) <= maxDeltaDegrees) {
		return goalDegrees;
	}
	else if (deltaDegrees > 0) {
		return currentDegrees + maxDeltaDegrees;
	}
	else {
		return currentDegrees - maxDeltaDegrees;
	}
}

float GetAngleDegreesBetweenVectors2D(Vec2 const& a, Vec2 const& b)
{
	float dotProd = DotProduct2D(a, b);
	float magnitudeA = a.GetLength(); 
	float magnitudeB = b.GetLength();
	float cosTheta = dotProd / (magnitudeA * magnitudeB);
	float angleInRadians = acos(cosTheta); // This is the angle in radians
	return ConvertRadiansToDegrees(angleInRadians);
}

// Dot and Cross
float DotProduct2D(const Vec2& a, const Vec2& b) {
	return a.x * b.x + a.y * b.y;
}

// Basic 2D & 3D utilities
float GetDistance2D(const Vec2& positionA, const Vec2& positionB) {
	float dx = positionB.x - positionA.x;
	float dy = positionB.y - positionA.y;
	return std::sqrt(dx * dx + dy * dy);
}

float GetDistanceSquared2D(const Vec2& positionA, const Vec2& positionB) {
	float dx = positionB.x - positionA.x;
	float dy = positionB.y - positionA.y;
	return dx * dx + dy * dy;
}

float GetDistance3D(const Vec3& positionA, const Vec3& positionB) {
	float dx = positionB.x - positionA.x;
	float dy = positionB.y - positionA.y;
	float dz = positionB.z - positionA.z;
	return std::sqrt(dx * dx + dy * dy + dz * dz);
}

float GetDistanceSquared3D(const Vec3& positionA, const Vec3& positionB) {
	float dx = positionB.x - positionA.x;
	float dy = positionB.y - positionA.y;
	float dz = positionB.z - positionA.z;
	return dx * dx + dy * dy + dz * dz;
}

float GetDistanceXY3D(const Vec3& positionA, const Vec3& positionB) {
	float dx = positionB.x - positionA.x;
	float dy = positionB.y - positionA.y;
	return std::sqrt(dx * dx + dy * dy);
}

float GetDistanceXYSquared3D(const Vec3& positionA, const Vec3& positionB) {
	float dx = positionB.x - positionA.x;
	float dy = positionB.y - positionA.y;
	return dx * dx + dy * dy;
}

int GetTaxicabDistance2D(IntVec2 const& pointA, IntVec2 const& pointB)
{
	return abs(pointA.x - pointB.x) + abs(pointA.y - pointB.y);
}

float GetProjectedLength2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	float dotProd = DotProduct2D(vectorToProject, vectorToProjectOnto);
	float magnitudeB = vectorToProjectOnto.GetLength();
	return dotProd / magnitudeB;
}

float GetProjectedLength3D(Vec3 const& sourceVec, Vec3 const& ontoVec)
{
	float ontoLength = ontoVec.GetLength();
	if (ontoLength == 0.f) {
		return 0.f;
	}
	float dotProduct = DotProduct3D(sourceVec, ontoVec);
	return dotProduct / ontoLength;
}

Vec2 const GetProjectedOnto2D(Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto)
{
	float projectedLength = GetProjectedLength2D(vectorToProject, vectorToProjectOnto);
	Vec2 unitVectorB = vectorToProjectOnto.GetNormalized();
	return unitVectorB * projectedLength;
}

bool IsPointInsideDisc2D(Vec2 const& point, Vec2 const& discCenter, float discRadius)
{
	float distanceSquared = GetDistanceSquared2D(point, discCenter);
	return distanceSquared <= (discRadius * discRadius);
}

bool IsPointInsideAABB2D(Vec2 const& point, AABB2 const& box)
{
	bool insideX = (point.x >= box.m_mins.x) && (point.x <= box.m_maxs.x);
	bool insideY = (point.y >= box.m_mins.y) && (point.y <= box.m_maxs.y);
	return insideX && insideY;
}

bool IsPointInsideCapsule2D(Vec2 const& point, Capsule2 const& capsule)
{
	return IsPointInsideCapsule2D(point, capsule.m_start, capsule.m_end, capsule.m_radius);
}

bool IsPointInsideCapsule2D(Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	Vec2 nearestPoint = GetNearestPointOnLineSegment2D(point, boneStart, boneEnd);
	float distanceSquared = (point - nearestPoint).GetLengthSquared();
	return distanceSquared <= (radius * radius);
}

bool IsPointInsideOBB2D(Vec2 const& point, OBB2 const& orientedBox)
{
	Vec2 localPoint = orientedBox.GetLocalPosForWorldPos(point);
	Vec2 halfDimensions = orientedBox.m_halfDimensions;
	return std::abs(localPoint.x) <= halfDimensions.x && std::abs(localPoint.y) <= halfDimensions.y;
}

bool IsPointInsideOrientedSector2D(Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius)
{
	Vec2 displacement = point - sectorTip; 
	float distanceSquared = displacement.GetLengthSquared(); 

	if (distanceSquared > sectorRadius * sectorRadius) {
		return false; 
	}

	float forwardRadians = sectorForwardDegrees * (3.14159265f / 180.0f); // Convert degrees to radians
	Vec2 forwardDirection = Vec2(std::cos(forwardRadians), std::sin(forwardRadians)); // Create forward vector

	float angleBetween = std::acos(DotProduct2D(displacement.GetNormalized(), forwardDirection)); // Get angle in radians
	float halfApertureRadians = sectorApertureDegrees * 0.5f * (3.14159265f / 180.0f); // Half aperture in radians

	return angleBetween <= halfApertureRadians;
}

bool IsPointInsideDirectedSector2D(Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius)
{
	Vec2 displacement = point - sectorTip;
	if (displacement.GetLengthSquared() > (sectorRadius * sectorRadius)) {
		return false; // Point is outside the sector's radius
	}

	displacement.Normalize();
	float dotProduct = DotProduct2D(displacement, sectorForwardNormal);
	float cosHalfAperture = cos(ConvertDegreesToRadians(sectorApertureDegrees) * 0.5f);
	return dotProduct >= cosHalfAperture;
}

// Geometric query utilities
bool DoDiscsOverlap2D(const Vec2& centerA, float radiusA, const Vec2& centerB, float radiusB) {
	return GetDistanceSquared2D(centerA, centerB) <= (radiusA + radiusB) * (radiusA + radiusB);
}

bool DoDiscsOverlapOBB22D(Vec2 const& referencePos, OBB2 const& orientedBox, float radius)
{
	Vec2 nearestPos = GetNearestPointOnOBB2D(referencePos, orientedBox);
	float length = (nearestPos - referencePos).GetLength();
	if (length >= radius)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DoDiscsOverlapCapsule2D(Vec2 const& referencePos, Capsule2 const& capsule, float radius)
{
	Vec2 nearestPos = GetNearestPointOnCapsule2D(referencePos, capsule);
	float length = (nearestPos - referencePos).GetLength();
	if (length >= radius)
	{
		return false;
	}
	else
	{
		return true;
	}
}

bool DoSpheresOverlap3D(const Vec3& centerA, float radiusA, const Vec3& centerB, float radiusB) {
	return GetDistanceSquared3D(centerA, centerB) <= (radiusA + radiusB) * (radiusA + radiusB);
}

bool DoZCylindersOverlap3D(Vec2 cylinder1CenterXY, float cylinder1Radius, FloatRange cylinder1MinMaxZ, Vec2 cylinder2CenterXY, float cylinder2Radius, FloatRange cylinder2MinMaxZ)
{
	float dx = cylinder1CenterXY.x - cylinder2CenterXY.x;
	float dy = cylinder1CenterXY.y - cylinder2CenterXY.y;
	float distanceSquared = dx * dx + dy * dy;
	float radiusSum = cylinder1Radius + cylinder2Radius;
	float radiusSumSquared = radiusSum * radiusSum;
	bool overlapInXY = distanceSquared <= radiusSumSquared;
	bool overlapInZ = cylinder1MinMaxZ.IsOverlappingWith(cylinder2MinMaxZ);
	return overlapInXY && overlapInZ;
}

bool DoSphereAndAABBOverlap3D(Vec3 sphereCenter, float spherRadius, AABB3 box)
{
	float nearestX = std::max(box.m_mins.x, std::min(sphereCenter.x, box.m_maxs.x));
	float nearestY = std::max(box.m_mins.y, std::min(sphereCenter.y, box.m_maxs.y));
	float nearestZ = std::max(box.m_mins.z, std::min(sphereCenter.z, box.m_maxs.z));

	float dx = nearestX - sphereCenter.x;
	float dy = nearestY - sphereCenter.y;
	float dz = nearestZ - sphereCenter.z;
	float distanceSquared = dx * dx + dy * dy + dz * dz;

	return distanceSquared <= (spherRadius * spherRadius);
}

bool DoZCylinderAndAABBOverlap3D(Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinMaxZ, AABB3 box)
{
	float nearestX = std::max(box.m_mins.x, std::min(cylinderCenterXY.x, box.m_maxs.x));
	float nearestY = std::max(box.m_mins.y, std::min(cylinderCenterXY.y, box.m_maxs.y));
	float dx = nearestX - cylinderCenterXY.x;
	float dy = nearestY - cylinderCenterXY.y;
	bool overlapInXY = (dx * dx + dy * dy) <= (cylinderRadius * cylinderRadius);

	bool overlapInZ = (cylinderMinMaxZ.m_min <= box.m_maxs.z) && (cylinderMinMaxZ.m_max >= box.m_mins.z);

	return overlapInXY && overlapInZ;
}

bool DoZCylinderAndSphereOverlap3D(Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinMaxZ, Vec3 sphereCenter, float sphereRadius)
{
	float dx = cylinderCenterXY.x - sphereCenter.x;
	float dy = cylinderCenterXY.y - sphereCenter.y;
	bool overlapInXY = (dx * dx + dy * dy) <= ((cylinderRadius + sphereRadius) * (cylinderRadius + sphereRadius));

	bool overlapInZ = (cylinderMinMaxZ.m_min <= sphereCenter.z + sphereRadius) && (cylinderMinMaxZ.m_max >= sphereCenter.z - sphereRadius);

	return overlapInXY && overlapInZ;
}

bool DoAABBsOverlap3D(AABB3 const& first, AABB3 const& second)
{
	bool overlapX = first.m_maxs.x >= second.m_mins.x && first.m_mins.x <= second.m_maxs.x;
	bool overlapY = first.m_maxs.y >= second.m_mins.y && first.m_mins.y <= second.m_maxs.y;
	bool overlapZ = first.m_maxs.z >= second.m_mins.z && first.m_mins.z <= second.m_maxs.z;
	return overlapX && overlapY && overlapZ;
}

Vec2 const GetNearestPointOnDisc2D(Vec2 const& referencePosition, Vec2 const& discCenter, float discRadius)
{
	Vec2 centerToPoint = referencePosition - discCenter;
	centerToPoint.ClampLength(discRadius);
	return discCenter + centerToPoint;
}

Vec2 const GetNearestPointOnAABB2D(Vec2 const& referencePosition, AABB2& box)
{
	Vec2 nearestPoint;
	nearestPoint.x = Clamp(referencePosition.x, box.m_mins.x, box.m_maxs.x);
	nearestPoint.y = Clamp(referencePosition.y, box.m_mins.y, box.m_maxs.y);
	return nearestPoint;
}

Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePos, LineSegment2 const& infiniteLine)
{
	Vec2 displacement = infiniteLine.m_end - infiniteLine.m_start;
	Vec2 normalizedDisplacement = displacement.GetNormalized();
	float projectionLength = DotProduct2D(referencePos - infiniteLine.m_start, normalizedDisplacement);
	return infiniteLine.m_start + normalizedDisplacement * projectionLength;
}

Vec2 const GetNearestPointOnInfiniteLine2D(Vec2 const& referencePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine)
{
	Vec2 lineDirection = anotherPointOnLine - pointOnLine;
	Vec2 pointToLine = referencePos - pointOnLine;

	float projectionLength = DotProduct2D(pointToLine, lineDirection) / DotProduct2D(lineDirection, lineDirection);
	Vec2 nearestPointVector = lineDirection * projectionLength;

	Vec2 nearestPoint = pointOnLine + nearestPointVector;
	return nearestPoint;
}

Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePos, LineSegment2 const& lineSegment)
{
	return GetNearestPointOnLineSegment2D(referencePos, lineSegment.m_start, lineSegment.m_end);
}

Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePos, Vec2 const& lineSegStart, Vec2 const& lineSegEnd)
{
	Vec2 displacement = lineSegEnd - lineSegStart;
	Vec2 normalizedDisplacement = displacement.GetNormalized();
	float projectionLength = DotProduct2D(referencePos - lineSegStart, normalizedDisplacement);
	if (projectionLength < 0) {
		return lineSegStart;
	}
	else if (projectionLength > displacement.GetLength()) {
		return lineSegEnd;
	}
	return lineSegStart + normalizedDisplacement * projectionLength;
}

Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePos, Capsule2 const& capsule)
{
	return GetNearestPointOnCapsule2D(referencePos, capsule.m_start, capsule.m_end, capsule.m_radius);
}

Vec2 const GetNearestPointOnCapsule2D(Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius)
{
	Vec2 nearestPointOnLine = GetNearestPointOnLineSegment2D(referencePos, boneStart, boneEnd);
	Vec2 displacement = referencePos - nearestPointOnLine;
	return nearestPointOnLine + displacement.GetClamped(radius);
}

Vec2 const GetNearestPointOnOBB2D(Vec2 const& referencePos, OBB2 const& orientedBox)
{
	Vec2 localPoint = orientedBox.GetLocalPosForWorldPos(referencePos);
	Vec2 clampedLocalPoint;

	clampedLocalPoint.x = std::max(-orientedBox.m_halfDimensions.x, std::min(localPoint.x, orientedBox.m_halfDimensions.x));
	clampedLocalPoint.y = std::max(-orientedBox.m_halfDimensions.y, std::min(localPoint.y, orientedBox.m_halfDimensions.y));

	return orientedBox.GetWorldPosForLocalPos(clampedLocalPoint);
}

bool PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint)
{
	if(!IsPointInsideDisc2D(fixedPoint, mobileDiscCenter, discRadius)) return false;
	Vec2 displacement = mobileDiscCenter - fixedPoint;
	float overlapDepth = discRadius - displacement.GetLength(); 
	displacement.SetLength(overlapDepth);
	mobileDiscCenter += displacement;
	return true;
}

bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius)
{
	float combinedRadii = mobileDiscRadius + fixedDiscRadius;
	Vec2 centerToCenter = mobileDiscCenter - fixedDiscCenter;
	float distanceSquared = centerToCenter.GetLengthSquared();

	if (distanceSquared < combinedRadii * combinedRadii) {
		float distance = std::sqrt(distanceSquared);
		Vec2 correctionDirection = centerToCenter.GetNormalized();
		float correctionDistance = combinedRadii - distance;
		mobileDiscCenter += correctionDirection * correctionDistance;
		return true;
	}
	return false;
}

bool PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius)
{
	float combinedRadii = aRadius + bRadius;
	Vec2 centerToCenter = aCenter - bCenter;
	float distanceSquared = centerToCenter.GetLengthSquared(); 

	if (distanceSquared < combinedRadii * combinedRadii) {
		float distance = std::sqrt(distanceSquared);
		Vec2 correctionDirection = centerToCenter.GetNormalized();
		float correctionDistance = combinedRadii - distance;
		Vec2 correctionVector = correctionDirection * correctionDistance;
		aCenter += correctionVector * 0.5f;
		bCenter -= correctionVector * 0.5f;
		return true;
	}
	return false;
}

bool PushDiscOutOfFixedAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox)
{
	Vec2 nearestPointOnBox = fixedBox.GetNearestPoint(mobileDiscCenter);
	Vec2 displacementFromBox = mobileDiscCenter - nearestPointOnBox;
	float distanceFromBox = displacementFromBox.GetLength(); 
	if (distanceFromBox <= discRadius) {
		Vec2 correctionDirection = displacementFromBox.GetNormalized();
		Vec2 correctionVector = correctionDirection * (discRadius - distanceFromBox);
		mobileDiscCenter += correctionVector;
		return true;
	}
	return false;
}

// Transform utilities
void TransformPosition2D(Vec2& posToTransform, float uniformScale, float rotationDegrees, const Vec2& translation) {
	// Scale
	posToTransform.x *= uniformScale;
	posToTransform.y *= uniformScale;

	float distance = std::sqrt(posToTransform.x * posToTransform.x + posToTransform.y * posToTransform.y);
	float currentAngleRadians = std::atan2(posToTransform.y, posToTransform.x);
	float rotationRadians = ConvertDegreesToRadians(rotationDegrees);
	float newAngleRadians = currentAngleRadians + rotationRadians;

	posToTransform.x = distance * std::cos(newAngleRadians);
	posToTransform.y = distance * std::sin(newAngleRadians);

	// Translate
	posToTransform.x += translation.x;
	posToTransform.y += translation.y;
}


void TransformPosition2D(Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	Vec2 newPosition;
	newPosition.x = posToTransform.x * iBasis.x + posToTransform.y * jBasis.x;
	newPosition.y = posToTransform.x * iBasis.y + posToTransform.y * jBasis.y;

	// Apply translation
	posToTransform = newPosition + translation;
}

void TransformPositionXY3D(Vec3& positionToTransform, float scaleXY, float zRotationDegrees, const Vec2& translationXY) {
	// Scale
	positionToTransform.x *= scaleXY;
	positionToTransform.y *= scaleXY;

	float distance = std::sqrt(positionToTransform.x * positionToTransform.x + positionToTransform.y *positionToTransform.y);
	float currentAngleRadians = std::atan2(positionToTransform.y,positionToTransform.x);
	float rotationRadians = ConvertDegreesToRadians(zRotationDegrees);
	float newAngleRadians = currentAngleRadians + rotationRadians;
	
	positionToTransform.x = distance * std::cos(newAngleRadians);
	positionToTransform.y = distance * std::sin(newAngleRadians);
	// Translate
	positionToTransform.x += translationXY.x;
	positionToTransform.y += translationXY.y;
}

void TransformPositionXY3D(Vec3& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation)
{
	Vec2 pos2D(positionToTransform.x, positionToTransform.y);

	// Use the 2D transformation for the x and y components
	TransformPosition2D(pos2D, iBasis, jBasis, translation);

	// Assign the transformed coordinates back to the original 3D position
	positionToTransform.x = pos2D.x;
	positionToTransform.y = pos2D.y;
}

float NormalizeByte(unsigned char byte)
{
	return static_cast<float>(byte) / 255.0f;
}

unsigned char DenormalizeByte(float num)
{
	return (unsigned char)(Clamp( num * 256.f, 0.f, 255.f ));
}


float ComputeCubicBezier1D(float A, float B, float C, float D, float t)
{
	// First level of interpolation
	float AB = A + (B - A) * t;
	float BC = B + (C - B) * t;
	float CD = C + (D - C) * t;

	// Second level of interpolation
	float ABC = AB + (BC - AB) * t;
	float BCD = BC + (CD - BC) * t;

	// Final interpolation
	float ABCD = ABC + (BCD - ABC) * t;

	return ABCD;
}

float ComputeQuinticBezier1D(float A, float B, float C, float D, float E, float F, float t)
{
	// First level of interpolation
	float AB = A + (B - A) * t;
	float BC = B + (C - B) * t;
	float CD = C + (D - C) * t;
	float DE = D + (E - D) * t;
	float EF = E + (F - E) * t;

	// Second level of interpolation
	float ABC = AB + (BC - AB) * t;
	float BCD = BC + (CD - BC) * t;
	float CDE = CD + (DE - CD) * t;
	float DEF = DE + (EF - DE) * t;

	// Third level of interpolation
	float ABCD = ABC + (BCD - ABC) * t;
	float BCDE = BCD + (CDE - BCD) * t;
	float CDEF = CDE + (DEF - CDE) * t;

	// Fourth level of interpolation
	float ABCDE = ABCD + (BCDE - ABCD) * t;
	float BCDEF = BCDE + (CDEF - BCDE) * t;

	// Final interpolation
	float ABCDEF = ABCDE + (BCDEF - ABCDE) * t;

	return ABCDEF;
}

float SmoothStart2(float t)
{
	return t * t;
}

float SmoothStart3(float t)
{
	return t * t * t;
}

float SmoothStart4(float t)
{
	return t * t * t * t;
}

float SmoothStart5(float t)
{
	return t * t * t * t * t;
}

float SmoothStart6(float t)
{
	return t * t * t * t * t * t;
}

float SmoothStop2(float t)
{
	float inverseT = 1 - t;
	return 1 - inverseT * inverseT;
}

float SmoothStop3(float t)
{
	float inverseT = 1 - t;
	return 1 - inverseT * inverseT * inverseT;
}

float SmoothStop4(float t)
{
	float inverseT = 1 - t;
	return 1 - inverseT * inverseT * inverseT * inverseT;
}

float SmoothStop5(float t)
{
	float inverseT = 1 - t;
	return 1 - inverseT * inverseT * inverseT * inverseT * inverseT;
}

float SmoothStop6(float t)
{
	float inverseT = 1 - t;
	return 1 - inverseT * inverseT * inverseT * inverseT * inverseT * inverseT;
}

float SmoothStep3(float t)
{
	return ComputeCubicBezier1D(0, 0, 1, 1, t);
}

//float SmoothStep3(float t)
//{
//	float smoothStart = t * t; // SmoothStart2
//	float smoothStop = 1 - (1 - t) * (1 - t); // SmoothStop2
//	return Lerp(smoothStart, smoothStop, t);
//}

float Lerp(float start, float end, float t)
{
	return start + (end - start) * t;
}

float SmoothStep5(float t)
{
	return ComputeQuinticBezier1D(0, 0, 0, 1, 1, 1, t);
}

float Hesitate3(float t)
{
	return ComputeCubicBezier1D(0, 1, 0, 1, t);
}

float Hesitate5(float t)
{
	return ComputeQuinticBezier1D(0, 1, 0, 1, 0, 1, t);
}

float CustomFunkyEasingFunction(float t)
{
	float wave = sinf(t * PI * 25.f); 
	float fade = t * (1 - t); 

	return t + wave * fade * 0.25f;
}

float DotProduct3D(const Vec3& a, const Vec3& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z;
}

float DotProduct4D(Vec4 const& a, Vec4 const& b)
{
	return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
}

float CrossProduct2D(const Vec2& a, const Vec2& b)
{
	return a.x * b.y - a.y * b.x;
}

Vec3 CrossProduct3D(const Vec3& a, const Vec3& b)
{
	return Vec3(
		a.y * b.z - a.z * b.y,
		a.z * b.x - a.x * b.z,
		a.x * b.y - a.y * b.x
	);
}