#include <math.h>
#include "Engine/Math/Vec3.hpp"
#include "Engine/Math/IntVec3.hpp"
#include "Engine/Core/StringUtils.hpp"
#include "Engine/Core/ErrorWarningAssert.hpp"


IntVec3::IntVec3(const IntVec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}

IntVec3::IntVec3(int initialX, int initialY, int initialZ)
{
	x = initialX;
	y = initialY;
	z = initialZ;
}

float IntVec3::GetLength() const
{
	return sqrtf(static_cast<float>(x * x + y * y + z * z));
}

int IntVec3::GetTaxicabLength() const
{
	return abs(x) + abs(y) + abs(z);
}

int IntVec3::GetLengthSquared() const
{
	return x * x + y * y + z * z;
}

void IntVec3::SetFromText(const char* text)
{
	std::string strText = text;

	Strings splitStrings = SplitStringOnDelimiter(text, ',');
	GUARANTEE_OR_DIE(splitStrings.size() == 3, Stringf("IntVec3 can't construct from improper string \"%s\"", text));

	x = atoi(splitStrings[0].c_str());
	y = atoi(splitStrings[1].c_str());
	z = atoi(splitStrings[2].c_str());
}

const IntVec3 IntVec3::operator+(const IntVec3& vecToAdd) const
{
	return IntVec3(x + vecToAdd.x, y + vecToAdd.y, z + vecToAdd.z);
}

const IntVec3 IntVec3::operator-(const IntVec3& vecToSubtract) const
{
	return IntVec3(x - vecToSubtract.x, y - vecToSubtract.y, z - vecToSubtract.z);
}

const IntVec3 IntVec3::operator-() const
{
	return IntVec3(-x, -y, -z);
}

const IntVec3 IntVec3::operator*(int uniformScale) const
{
	return IntVec3(x * uniformScale, y * uniformScale, z * uniformScale);
}

const IntVec3 IntVec3::operator*(const IntVec3& vecToMultiply) const
{
	return IntVec3(x * vecToMultiply.x, y * vecToMultiply.y, z * vecToMultiply.z);

}

void IntVec3::operator+=(const IntVec3& vecToAdd)
{
	x += vecToAdd.x;
	y += vecToAdd.y;
	z += vecToAdd.z;
}


//-----------------------------------------------------------------------------------------------
void IntVec3::operator-=(const IntVec3& vecToSubtract)
{
	x -= vecToSubtract.x;
	y -= vecToSubtract.y;
	z -= vecToSubtract.z;
}


//-----------------------------------------------------------------------------------------------
void IntVec3::operator*=(const int uniformScale)
{
	x *= uniformScale;
	y *= uniformScale;
	z *= uniformScale;
}


//-----------------------------------------------------------------------------------------------
void IntVec3::operator=(const IntVec3& copyFrom)
{
	x = copyFrom.x;
	y = copyFrom.y;
	z = copyFrom.z;
}


bool IntVec3::operator!=(const IntVec3& compare) const
{
	if (x == compare.x) {
		if (y == compare.y) {
			if (z == compare.z)
			{
				return false;
			}
		}
	}
	return true;
}

bool IntVec3::operator==(const IntVec3& compare) const
{
	if (x == compare.x) {
		if (y == compare.y) {
			if (z == compare.z)
			{
				return true;
			}

		}
	}
	return false;
}

bool IntVec3::operator<(const IntVec3& comparewith) const
{
	return x < comparewith.x || (x == comparewith.x && y < comparewith.y) || (x == comparewith.x && y == comparewith.y && z < comparewith.z);
}


