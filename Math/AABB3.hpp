#pragma once
#include"Engine/Math/Vec3.hpp"

struct AABB3
{
public:
	Vec3 m_mins;
	Vec3 m_maxs;

public:
	// Construction/Destruction
	~AABB3(){}
	AABB3(){}
	AABB3(AABB3 const& copyFrom);
	explicit AABB3(float mixX, float minY, float minZ, float maxX, float maxY, float maxZ);
	explicit AABB3(Vec3 const& mins, Vec3 const& maxs);
	Vec3  GetCenter() const;
	Vec3  GetDimensions() const;

};