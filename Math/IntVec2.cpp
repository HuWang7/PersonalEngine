#include "IntVec2.hpp"
#include <cmath>
#include "Engine/Core/StringUtils.hpp"

const IntVec2 IntVec2::ZERO = IntVec2(0, 0);

// Copy constructor
IntVec2::IntVec2(const IntVec2& copyFrom) : x(copyFrom.x), y(copyFrom.y) {}

// Explicit constructor
IntVec2::IntVec2(int initialX, int initialY) : x(initialX), y(initialY) {}

// Accessors
float IntVec2::GetLength() const {
	return static_cast<float>(std::sqrt(x * x + y * y));
}

int IntVec2::GetTaxicabLength() const {
	return std::abs(x) + std::abs(y);
}

int IntVec2::GetLengthSquared() const {
	return x * x + y * y;
}

float IntVec2::GetOrientationRadians() const {
	return std::atan2(static_cast<float>(y), static_cast<float>(x));
}

float IntVec2::GetOrientationDegrees() const {
	return GetOrientationRadians() * (180.0f / 3.14159265358979323846f);
}

IntVec2 const IntVec2::GetRotated90Degrees() const {
	return IntVec2(-y, x);
}

IntVec2 const IntVec2::GetRotatedMinus90Degrees() const {
	return IntVec2(y, -x);
}

// Mutators
void IntVec2::Rotate90Degrees() {
	int oldX = x;
	x = -y;
	y = oldX;
}

void IntVec2::RotateMinus90Degrees() {
	int oldX = x;
	x = y;
	y = -oldX;
}

bool IntVec2::operator==(const IntVec2& compare) const
{
	return x == compare.x && y == compare.y;
}

void IntVec2::SetFromText(const char* text)
{
	Strings parts = SplitStringOnDelimiter(std::string(text), ',');
	if (parts.size() == 2) {
		x = atoi(parts[0].c_str());
		y = atoi(parts[1].c_str());
	}
}

// Operators
void IntVec2::operator=(const IntVec2& copyFrom) {
	x = copyFrom.x;
	y = copyFrom.y;
}

const IntVec2 IntVec2::operator+(const IntVec2& vecToAdd) const
{
	return IntVec2(x + vecToAdd.x, y + vecToAdd.y);
}

const IntVec2 IntVec2::operator-(const IntVec2& vecToSubtract) const
{
	return IntVec2(x - vecToSubtract.x, y - vecToSubtract.y);
}

const IntVec2 IntVec2::operator-() const
{
	return IntVec2(-x, -y);
}

const IntVec2 IntVec2::operator*(int uniformScale) const
{
	return IntVec2(x * uniformScale, y * uniformScale);
}

const IntVec2 IntVec2::operator*(const IntVec2& vecToMultiply) const
{
	return IntVec2(x * vecToMultiply.x, y * vecToMultiply.y);

}

void IntVec2::operator+=(const IntVec2& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator-=(const IntVec2& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
}


//-----------------------------------------------------------------------------------------------
void IntVec2::operator*=(const int uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
}

bool IntVec2::operator!=(const IntVec2& compare) const
{
	if (x == compare.x) {
		if (y == compare.y) {
			return false;
		}
	}
	return true;
}

bool IntVec2::operator<(const IntVec2& comparewith) const
{
	return x < comparewith.x || (x == comparewith.x && y < comparewith.y);
}
