#pragma once
#include "Engine/Math/IntVec2.hpp"
#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/Vec4.hpp"
#include "Engine/Math/Mat44.hpp"
#include "Engine/Math/LineSegment2.hpp"
#include "Engine/Math/AABB2.hpp"
#include "Engine/Math/AABB3.hpp"
#include "Engine/Math/FloatRange.hpp"
//----------------------------------------------------------------------------------------------
// Forward type declaration
//

struct Vec2;
struct Vec3;
struct AABB2;
struct Capsule2;
struct OBB2;
struct OBB3;
struct Plane3D;
struct LineSegment2;
class Actor;

struct Sphere3D {
	Vec3 center;
	float radius;
	std::vector<Vertex_PCU> vertices;
	Rgba8 color;
	bool overlap = false;

	Sphere3D(const Vec3& center, float radius, const Rgba8& color, const std::vector<Vertex_PCU>& vertices)
		: center(center)
		, radius(radius)
		, color(color)
		, vertices(vertices)
	{

	}
};

struct Cylinder {
	Vec3 baseCenter;
	float radius;
	float height;
	std::vector<Vertex_PCU> vertices;
	Rgba8 color;
	bool overlap = false;

	Cylinder(const Vec3& baseCenter, float radius, float height, const Rgba8& color, const std::vector<Vertex_PCU>& vertices)
		: baseCenter(baseCenter)
		, radius(radius)
		, height(height)
		, color(color)
		, vertices(vertices)
	{
	}
};

struct AABB3D {
	Vec3 m_mins;
	Vec3 m_maxs;
	std::vector<Vertex_PCU> vertices;
	Rgba8 color;
	bool overlap = false;

	AABB3D(const Vec3& m_mins, const Vec3& m_maxs, const Rgba8& color, const std::vector<Vertex_PCU>& vertices)
		: m_mins(m_mins)
		, m_maxs(m_maxs)
		, color(color)
		, vertices(vertices)
	{
	}
};

struct OBB3DBox
{
	Vec3 center;
	Vec3 halfDimensions;
	Vec3 iBasis;
	Vec3 jBasis;
	std::vector<Vertex_PCU> vertices;
	Rgba8 color;
	bool overlap = false;

	OBB3DBox(const Vec3& center, const Vec3& halfDimensions, const Vec3& iBasis, const Vec3& jBasis, const Rgba8& color, const std::vector<Vertex_PCU>& vertices)
		:center(center)
		,halfDimensions(halfDimensions)
		,iBasis(iBasis)
		,jBasis(jBasis)
		,color(color)
		,vertices(vertices)
	{

	}
};
enum Shape
{
	NONE,
	BOX,
	CYLINDER,
	OBB,
	PLANE,
	SPHERE
};

struct RaycastResult2D
{
	// Basic raycast result information (required)
	bool	m_didImpact = false;
	float	m_impactDist = 0.f;
	Vec2	m_impactPos;
	Vec2	m_impactNormal;

	// Responding to different situations
	Vec2	m_dicsCenter;
	LineSegment2 m_line;
	AABB2 m_AABB2;

	// Original raycast information (optional)
	Vec2	m_rayFwdNormal;
	Vec2	m_rayStartPos;
	float	m_rayMaxLength = 1.f;
};

struct RaycastResult3D
{
	// Basic raycast result information (required)
	Vec3	m_rayStartPosition;
	bool	m_didImpact = false;
	float	m_impactDist = 0.f;
	Vec3	m_impactPos;
	Vec3	m_impactNormal;
	Vec3	m_rayDirection;
	float	m_rayLength;

	// Responding to different situations
	Shape m_shape = Shape::NONE;
	Vec3 m_cylinderBaseCenter;
	AABB3 m_box;
	Vec3 m_sphereCenter;
	Vec3 m_obbCenter;	

	// Original raycast information (optional)
	Actor* m_touchedActor = nullptr;
};

enum class BillboardType
{
	NONE = -1,
	WORLD_UP_CAMERA_FACING,
	WORLD_UP_CAMERA_OPPOSING,
	FULL_CAMERA_FACING,
	FULL_CAMERA_OPPOSING,
	COUNT
};

Mat44 GetBillboardMatrix(BillboardType billboardType, Mat44 const& cameraMatrix, const Vec3& billboardPosition, const Vec2& billboardScale = Vec2(1.0f, 1.0f));

RaycastResult2D RaycastVsDisc2D( Vec2 startPos, Vec2 fwdNormal, float maxDist, Vec2 discCenter, float discRadius );
RaycastResult2D RaycastVsLineSegment2D(Vec2 rayStart, Vec2 rayFwdNormal, float rayMaxDist, Vec2 LineSegStart, Vec2 LineSegEnd);
RaycastResult2D RaycastVsAABB2D(Vec2 rayStart, Vec2 rayEnd,  Vec2 rayFwdNormal, float rayMaxDist, AABB2 bound);
RaycastResult3D RaycastVsAABB3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, AABB3 box);
RaycastResult3D RaycastVsOBB3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, OBB3 box);
RaycastResult3D RaycastVsSphere3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Vec3 sphereCenter, float sphereRadius);
RaycastResult3D RaycastVsCylinderZ3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Vec2 const& centerXY, FloatRange const& minMaxZ, float radiusXY);
RaycastResult3D RaycastVsPlane3D(Vec3 rayStart, Vec3 rayForwardNormal, float rayLength, Plane3D plane);

//----------------------------------------------------------------------------------------------
// Clamp and lerp
float Clamp( float value, float minValue, float maxValue );
int	  Clamp(int value, int minValue, int maxValue);
float ClampZeroToOne( float value );
float Interpolate( float start, float end, float fractionTowardEnd );
Vec2 Interpolate(Vec2 pointA, Vec2 pointB, float fraction);
float GetFractionWithinRange( float value, float rangeStart, float rangeEnd );
float RangeMap(float inValue, float inStart, float inEnd, float outStart, float outEnd);
float RangeMapClamped(float inValue, float inStart, float inEnd, float outStart, float outEnd);
int   RoundDownToInt( float value );

//----------------------------------------------------------------------------------------------
// Angle utilities
float ConvertDegreesToRadians( float degrees );
float ConvertRadiansToDegrees( float radians );
float CosDegrees( float degrees );
float SinDegrees( float degrees );
float Atan2Degrees( float y, float x);
float GetShortestAngularDispDegrees( float startDegrees, float endDegrees );
float GetTurnedTowardDegrees( float currentDegrees, float goalDegrees, float maxDeltaDegrees );
float GetAngleDegreesBetweenVectors2D( Vec2 const& a, Vec2 const& b );

//----------------------------------------------------------------------------------------------
// Dot and Cross
float DotProduct2D( Vec2 const& a, Vec2 const& b );
float DotProduct3D( const Vec3& a, const Vec3& b );
float DotProduct4D( Vec4 const& a, Vec4 const& b );
float CrossProduct2D(const Vec2& a, const Vec2& b);
Vec3 CrossProduct3D(const Vec3& a, const Vec3& b );


//----------------------------------------------------------------------------------------------
// Basic 2D & 3D utilities
float GetDistance2D( Vec2 const& positionA, Vec2 const& positionB );
float GetDistanceSquared2D( Vec2 const& positionA, Vec2 const& positionB );
float GetDistance3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceSquared3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceXY3D( Vec3 const& positionA, Vec3 const& positionB );
float GetDistanceXYSquared3D( Vec3 const& positionA, Vec3 const& positionB );
int   GetTaxicabDistance2D( IntVec2 const& pointA, IntVec2 const& pointB );
float GetProjectedLength2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );
float GetProjectedLength3D(Vec3 const& sourceVec, Vec3 const& ontoVec);

Vec2 const GetProjectedOnto2D( Vec2 const& vectorToProject, Vec2 const& vectorToProjectOnto );


//----------------------------------------------------------------------------------------------
// Geometric query utilities
//
bool IsPointInsideDisc2D( Vec2 const& point, Vec2 const& discCenter, float discRadius);
bool IsPointInsideAABB2D( Vec2 const& point, AABB2 const& box);
bool IsPointInsideCapsule2D( Vec2 const& point, Capsule2 const& capsule);
bool IsPointInsideCapsule2D( Vec2 const& point, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
bool IsPointInsideOBB2D( Vec2 const& point, OBB2 const& orientedBox);
bool IsPointInsideOrientedSector2D( Vec2 const& point, Vec2 const& sectorTip, float sectorForwardDegrees, float sectorApertureDegrees, float sectorRadius );
bool IsPointInsideDirectedSector2D( Vec2 const& point, Vec2 const& sectorTip, Vec2 const& sectorForwardNormal, float sectorApertureDegrees, float sectorRadius );

bool DoDiscsOverlap2D( Vec2 const& centerA, float radiusA, Vec2 const& centerB, float radiusB);
bool DoDiscsOverlapOBB22D(Vec2 const& referencePos, OBB2 const& orientedBox, float radiusB);
bool DoDiscsOverlapCapsule2D(Vec2 const& referencePos, Capsule2 const& capsule, float radiusB);
bool DoSpheresOverlap3D( Vec3 const& centerA, float radiusA, Vec3 const& centerB, float radiusB);
bool DoAABBsOverlap3D(AABB3 const& first, AABB3 const& second);
bool DoZCylindersOverlap3D(Vec2 cylinder1CenterXY, float cylinder1Radius, FloatRange cylinder1MinMaxZ, Vec2 cylinder2CenterXY, float cylinder2Radius, FloatRange cylinder2MinMaxZ);
bool DoSphereAndAABBOverlap3D(Vec3 sphereCenter, float sphereRadius, AABB3 box);
bool DoZCylinderAndAABBOverlap3D(Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinMaxZ, AABB3 box);
bool DoZCylinderAndSphereOverlap3D(Vec2 cylinderCenterXY, float cylinderRadius, FloatRange cylinderMinMaxZ, Vec3 sphereCenter, float sphereRadius);


Vec2 const GetNearestPointOnDisc2D( Vec2 const& referencePosition, Vec2 const& discCenter, float discRadius );
Vec2 const GetNearestPointOnAABB2D( Vec2 const& referencePosition, AABB2& box );
Vec2 const GetNearestPointOnInfiniteLine2D( Vec2 const& referencePos, LineSegment2 const& infiniteLine);
Vec2 const GetNearestPointOnInfiniteLine2D( Vec2 const& referencePos, Vec2 const& pointOnLine, Vec2 const& anotherPointOnLine);
Vec2 const GetNearestPointOnLineSegment2D( Vec2 const& referencePos, LineSegment2 const& lineSegment);
Vec2 const GetNearestPointOnLineSegment2D(Vec2 const& referencePos, Vec2 const& lineSegStart, Vec2 const& lineSegEnd);
Vec2 const GetNearestPointOnCapsule2D( Vec2 const& referencePos, Capsule2 const& capsule);
Vec2 const GetNearestPointOnCapsule2D( Vec2 const& referencePos, Vec2 const& boneStart, Vec2 const& boneEnd, float radius);
Vec2 const GetNearestPointOnOBB2D( Vec2 const& referencePos, OBB2 const& orientedBox);

bool PushDiscOutOfFixedPoint2D(Vec2& mobileDiscCenter, float discRadius, Vec2 const& fixedPoint);
bool PushDiscOutOfFixedDisc2D(Vec2& mobileDiscCenter, float mobileDiscRadius, Vec2 const& fixedDiscCenter, float fixedDiscRadius);
bool PushDiscsOutOfEachOther2D(Vec2& aCenter, float aRadius, Vec2& bCenter, float bRadius);
bool PushDiscOutOfFixedAABB2D(Vec2& mobileDiscCenter, float discRadius, AABB2 const& fixedBox);
//----------------------------------------------------------------------------------------------
// Transform utilities
//
void TransformPosition2D( Vec2& posToTransform, float uniformScale, float rotationDegrees, Vec2 const& translation );
void TransformPosition2D( Vec2& posToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation);
void TransformPositionXY3D( Vec3& positionToTransform, float scaleXY, float zRotationDegrees, Vec2 const& translationXY );
void TransformPositionXY3D( Vec3& positionToTransform, Vec2 const& iBasis, Vec2 const& jBasis, Vec2 const& translation );


float NormalizeByte(unsigned char byte);
unsigned char DenormalizeByte(float num);

// 1D Bezier standalone functions
float ComputeCubicBezier1D( float A, float B, float C, float D, float t );
float ComputeQuinticBezier1D( float A, float B, float C, float D, float E, float F, float t );

// SmoothStart2, SmoothStart3, SmoothStart4, SmoothStart5, SmoothStart6
float SmoothStart2(float t);
float SmoothStart3(float t);
float SmoothStart4(float t);
float SmoothStart5(float t);
float SmoothStart6(float t);

// SmoothStop2, SmoothStop3, SmoothStop4, SmoothStop5, SmoothStop6
float SmoothStop2(float t);
float SmoothStop3(float t);
float SmoothStop4(float t);
float SmoothStop5(float t);
float SmoothStop6(float t);

// SmoothStep3 
float SmoothStep3(float t);
float Lerp(float start, float end, float t);

// SmoothStep5 
float SmoothStep5(float t);

// Hesitate3
float Hesitate3(float t);

// Hesitate5
float Hesitate5(float t);

// CustomFunkyEasingFunction
float CustomFunkyEasingFunction(float t);