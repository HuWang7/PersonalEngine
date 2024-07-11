#include "Engine/Math/Vec4.hpp"
#include <cmath>

Vec4::Vec4()
	: x(0.0f), y(0.0f), z(0.0f), w(0.0f) {}

Vec4::Vec4(float initialX, float initialY, float initialZ, float initialW)
	: x(initialX), y(initialY), z(initialZ), w(initialW) {}

void Vec4::Set(float newX, float newY, float newZ, float newW) {
	x = newX;
	y = newY;
	z = newZ;
	w = newW;
}

float Vec4::DotProduct(const Vec4& other) const {
	return x * other.x + y * other.y + z * other.z + w * other.w;
}

Vec4 Vec4::operator+(const Vec4& vecToAdd) const {
	return Vec4(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z, w + vecToAdd.w);
}

Vec4 Vec4::operator-(const Vec4& vecToSubtract) const {
	return Vec4(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z, w - vecToSubtract.w);
}

Vec4 Vec4::operator*(float uniformScale) const {
	return Vec4(x * uniformScale, y * uniformScale, z * uniformScale, w * uniformScale);
}

Vec4 Vec4::operator/(float inverseScale) const {
	return Vec4(x / inverseScale, y / inverseScale, z / inverseScale, w / inverseScale);
}
