#pragma once

class Vec4 {
public:
	float x;
	float y;
	float z;
	float w;

	Vec4();
	Vec4(float initialX, float initialY, float initialZ, float initialW);

	void Set(float newX, float newY, float newZ, float newW);
	float DotProduct(const Vec4& other) const;

	Vec4 operator+(const Vec4& vecToAdd) const;
	Vec4 operator-(const Vec4& vecToSubtract) const;
	Vec4 operator*(float uniformScale) const;
	Vec4 operator/(float inverseScale) const;
};
