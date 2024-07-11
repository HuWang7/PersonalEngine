#pragma 
#include "Engine/Math/Vec3.hpp"

struct AABB3;

struct OBB3
{
public:
	Vec3 center;
	Vec3 halfDimensions;
	Vec3 iBasis = Vec3(1.f, 0.f, 0.f);
	Vec3 jBasis = Vec3(0.f, 1.f, 0.f);

public:
	OBB3(Vec3 const& center, Vec3 const& halfDimensions, Vec3 const& iBasis = Vec3(1.f, 0.f, 0.f), Vec3 const& jBasis = Vec3(0.f, 1.f, 0.f));
	explicit OBB3(AABB3 const& aabb);

	Vec3 GetForwardVector() const;
};