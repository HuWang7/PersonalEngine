#include "Engine/Math/Vec2.hpp"
#include "Engine/Math/MathUtils.hpp"
#include <cmath>
#include "Engine/Core/EngineCommon.hpp"
#include "Engine/Core/StringUtils.hpp"


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( const Vec2& copy )
	: x(copy.x)
	, y(copy.y)
{
}


//-----------------------------------------------------------------------------------------------
Vec2::Vec2( float initialX, float initialY )
	: x(initialX)
	, y(initialY)
{
}

const Vec2 Vec2::ZERO = Vec2(0.f, 0.f);
const Vec2 Vec2::ONE = Vec2(1.f, 1.f);

// Static methods
Vec2 const Vec2::MakeFromPolarRadians(float orientationRadians, float length) {
    return Vec2(cos(orientationRadians) * length, sin(orientationRadians) * length);
}

Vec2 const Vec2::MakeFromPolarDegrees(float orientationDegrees, float length) {
    float radians = orientationDegrees * (3.14159265358979323846f / 180.0f);
    return MakeFromPolarRadians(radians, length);
}

// Accessors
float Vec2::GetLength() const {
    return std::sqrt(x*x + y*y);
}

float Vec2::GetLengthSquared() const {
    return x*x + y*y;
}

float Vec2::GetOrientationRadians() const {
    return std::atan2(y, x);
}

float Vec2::GetOrientationDegrees() const {
    return GetOrientationRadians() * (180.0f / 3.14159265358979323846f);
}

Vec2 const Vec2::GetRotated90Degrees() const {
    return Vec2(-y, x);
}

Vec2 const Vec2::GetRotatedMinus90Degrees() const {
    return Vec2(y, -x);
}

Vec2 const Vec2::GetRotatedRadians(float deltaRadians) const {
    float r = GetLength();
    float theta = std::atan2(y, x);

    theta += deltaRadians;

    return Vec2(r * std::cos(theta), r * std::sin(theta));
}

Vec2 const Vec2::GetRotatedDegrees(float deltaDegrees) const {
    return GetRotatedRadians(deltaDegrees * (3.14159265358979323846f / 180.0f));
}

Vec2 const Vec2::GetClamped(float maxLength) const {
    float length = GetLength();
    if (length > maxLength) {
        float scale = maxLength / length;
        return Vec2(x*scale, y*scale);
    }
    return *this;
}

Vec2 const Vec2::GetNormalized() const {
    float length = GetLength();
    if (length == 0.0f) return Vec2();
    return Vec2(x/length, y/length);
}

// Mutators
void Vec2::SetOrientationRadians(float newOrientationRadians) {
    float length = GetLength();
    x = cos(newOrientationRadians) * length;
    y = sin(newOrientationRadians) * length;
}

void Vec2::SetOrientationDegrees(float newOrientationDegrees) {
    SetOrientationRadians(newOrientationDegrees * (3.14159265358979323846f / 180.0f));
}

void Vec2::SetPolarRadians(float newOrientationRadians, float newLength) {
    x = cos(newOrientationRadians) * newLength;
    y = sin(newOrientationRadians) * newLength;
}

void Vec2::SetPolarDegrees(float newOrientationDegrees, float newLength) {
    SetPolarRadians(newOrientationDegrees * (3.14159265358979323846f / 180.0f), newLength);
}

void Vec2::Rotate90Degrees() {
    float temp = x;
    x = -y;
    y = temp;
}

void Vec2::RotateMinus90Degrees() {
    float temp = x;
    x = y;
    y = -temp;
}

void Vec2::RotateRadians(float deltaRadians) {
    float r = GetLength();
    float theta = std::atan2(y, x);
    theta += deltaRadians;
    x = r * std::cos(theta);
    y = r * std::sin(theta);
}

void Vec2::RotateDegrees(float deltaDegrees) {
    RotateRadians(deltaDegrees * (3.14159265358979323846f / 180.0f));
}

void Vec2::SetLength(float newLength) {
    Vec2 normalized = GetNormalized();
    x = normalized.x * newLength;
    y = normalized.y * newLength;
}

void Vec2::ClampLength(float maxLength) {
    float length = GetLength();
    if(length > maxLength) {
        float scale = maxLength / length;
        x *= scale;
        y *= scale;
    }
}

void Vec2::Normalize() {
    float length = GetLength();
    if(length > 0.0f) {
        x /= length;
        y /= length;
    }
}

float Vec2::NormalizeAndGetPreviousLength() {
    float length = GetLength();
    Normalize();
    return length;
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator + ( const Vec2& vecToAdd ) const
{
	return Vec2(x + vecToAdd.x, y + vecToAdd.y);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-( const Vec2& vecToSubtract ) const
{
	return Vec2(x - vecToSubtract.x, y - vecToSubtract.y);
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator-() const
{
	return Vec2(-x, -y);
}


//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( float uniformScale ) const
{
	return Vec2(x * uniformScale, y * uniformScale);
}


//------------------------------------------------------------------------------------------------
const Vec2 Vec2::operator*( const Vec2& vecToMultiply ) const
{
	return Vec2(x * vecToMultiply.x, y * vecToMultiply.y);
}

void Vec2::Reflect(const Vec2& normal)
{
	float dotProduct = DotProduct2D(*this, normal);
	*this -= normal * (2.f * dotProduct);
}

Vec2 Vec2::GetReflected(const Vec2& normal) const
{
	float dotProduct = DotProduct2D(*this, normal);
	return *this - 2.f * dotProduct * normal;
}

void Vec2::SetFromText(const char* text)
{
	Strings parts = SplitStringOnDelimiter(std::string(text), ',');
	if (parts.size() == 2) {
		x = static_cast<float>(atof(parts[0].c_str()));
		y = static_cast<float>(atof(parts[1].c_str()));
	}
}

//-----------------------------------------------------------------------------------------------
const Vec2 Vec2::operator/( float inverseScale ) const
{
	return Vec2(x / inverseScale, y / inverseScale);
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator+=( const Vec2& vecToAdd )
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator-=( const Vec2& vecToSubtract )
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator*=( const float uniformScale )
{
	x *= uniformScale;
	y *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator/=( const float uniformDivisor )
{
	x /= uniformDivisor;
	y /= uniformDivisor;
}


//-----------------------------------------------------------------------------------------------
void Vec2::operator=( const Vec2& copyFrom )
{
	x = copyFrom.x;
	y = copyFrom.y;
}


//-----------------------------------------------------------------------------------------------
const Vec2 operator*( float uniformScale, const Vec2& vecToScale )
{
	return Vec2(vecToScale.x * uniformScale, vecToScale.y * uniformScale);
}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator==( const Vec2& compare ) const
{
	return x == compare.x && y == compare.y;

}


//-----------------------------------------------------------------------------------------------
bool Vec2::operator!=( const Vec2& compare ) const
{
	return !(*this == compare);
}

