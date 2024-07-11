#include "Engine/Math/OBB3.hpp"
#include "Engine/Math/MathUtils.hpp"

OBB3::OBB3(Vec3 const& center, Vec3 const& halfDimensions, Vec3 const& iBasis /*= Vec3(1.f, 0.f, 0.f)*/, Vec3 const& jBasis /*= Vec3(0.f, 1.f, 0.f)*/)
	:center(center)
	, halfDimensions(halfDimensions)
	, iBasis(iBasis)
	, jBasis(jBasis) {

}

OBB3::OBB3(AABB3 const& aabb)
{
	center = aabb.GetCenter();
	halfDimensions = aabb.GetDimensions() * .5f;
}

Vec3 OBB3::GetForwardVector() const
{
	return CrossProduct3D(iBasis, jBasis);
}

