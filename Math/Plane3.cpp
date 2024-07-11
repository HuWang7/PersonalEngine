#include "Engine/Math/Plane3.hpp"
#include "Engine/Math/MathUtils.hpp"

Plane3D::Plane3D(Vec3 const& normal, Vec3 const& pointOnPlane)
	: normal(normal)
	, distanceFromOriginAlongNormal(DotProduct3D(normal, pointOnPlane))
{
}

float Plane3D::GetDistance(Vec3 const& point) const
{
	return GetProjectedLength3D(point, normal) - distanceFromOriginAlongNormal;
}

bool Plane3D::IsPointInFront(Vec3 const& point) const
{
	return GetDistance(point) > 0.f;
}
