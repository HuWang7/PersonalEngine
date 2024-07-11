#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/MathUtils.hpp"
#include "Engine/Core/StringUtils.hpp"
#include <cmath>

Vec3::Vec3(float initialX, float initialY, float initialZ)
	: x(initialX), y(initialY), z(initialZ) {}

float Vec3::GetLength() const {
	return std::sqrt(x * x + y * y + z * z);
}

float Vec3::GetLengthXY() const {
	return std::sqrt(x * x + y * y);
}

float Vec3::GetLengthSquared() const {
	return x * x + y * y + z * z;
}

float Vec3::GetLengthXYSquared() const {
	return x * x + y * y;
}

float Vec3::GetAngleAboutZRadians() const {
	return std::atan2(y, x);
}

float Vec3::GetAngleAboutZDegrees() const {
	return GetAngleAboutZRadians() * (180.0f / 3.14159265358979323846f); // Conversion from rad to deg
}

Vec3 const Vec3::GetRotatedAboutZRadians(float deltaRadians) const {
	float r = GetLengthXY();
	float theta = std::atan2(y, x);

	theta += deltaRadians;

	return Vec3(r * std::cos(theta), r * std::sin(theta), z);
}

Vec3 const Vec3::GetRotatedAboutZDegrees(float deltaDegrees) const {
	return GetRotatedAboutZRadians(deltaDegrees * (3.14159265358979323846f / 180.0f)); // Conversion from deg to rad
}

Vec3 const Vec3::GetClamped(float maxLength) const {
	float length = GetLength();
	if (length > maxLength) {
		float scale = maxLength / length;
		return Vec3(x * scale, y * scale, z * scale);
	}
	return *this;
}

Vec3 const Vec3::GetNormalized() const {
	float length = GetLength();
	if (length == 0.0f) return Vec3();
	return Vec3(x / length, y / length, z / length);
}

Vec3 Vec3::GetReflected(Vec3& surfaceNormal) const
{
	Vec3 vector = Vec3(x, y, z);

	Vec3 dotWithSurfaceNormal = surfaceNormal * DotProduct3D(vector, surfaceNormal);
	return vector - 2 * dotWithSurfaceNormal;
}

Vec3 const Vec3::MakeFromPolarRadians(float latitudeRadians, float longitudeRadians, float length /*= 1.0f*/)
{
	return Vec3(
		length * cos(latitudeRadians) * cos(longitudeRadians),
		length * cos(latitudeRadians) * sin(longitudeRadians),
		length * sin(latitudeRadians)
	);
}

Vec3 const Vec3::MakeFromPolarDegrees(float latitudeDegrees, float longitudeDegrees, float length /*= 1.0f*/)
{
	return MakeFromPolarRadians(ConvertDegreesToRadians(latitudeDegrees), ConvertDegreesToRadians(longitudeDegrees), length);
}

bool Vec3::operator==(Vec3 const& compare) const {
	return x == compare.x && y == compare.y && z == compare.z;
}

bool Vec3::operator!=(Vec3 const& compare) const {
	return !(*this == compare);
}

Vec3 const Vec3::operator+(Vec3 const& vecToAdd) const {
	return Vec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

Vec3 const Vec3::operator-(Vec3 const& vecToSubtract) const {
	return Vec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}

Vec3 const Vec3::operator*(float uniformScale) const {
	return Vec3(x * uniformScale, y * uniformScale, z * uniformScale);
}

void Vec3::SetFromText(const char* text)
{
	Strings parts = SplitStringOnDelimiter(std::string(text), ',');
	if (parts.size() == 3) {
		x = static_cast<float>(atof(parts[0].c_str()));
		y = static_cast<float>(atof(parts[1].c_str()));
		z = static_cast<float>(atof(parts[2].c_str()));
	}
}

Vec3 const Vec3::operator/(float inverseScale) const {
	return Vec3(x / inverseScale, y / inverseScale, z / inverseScale);
}

void Vec3::operator+=(Vec3 const& vecToAdd) {
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}

void Vec3::operator-=(Vec3 const& vecToSubtract) {
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}

void Vec3::operator*=(float uniformScale) {
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}

void Vec3::operator/=(float uniformDivisor) {
	x /= uniformDivisor;
	y /= uniformDivisor;
	z /= uniformDivisor;
}

void Vec3::operator=(Vec3 const& copyFrom) {
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

Vec3 const operator*(float uniformScale, Vec3 const& vecToScale) {
	return vecToScale * uniformScale;
}
